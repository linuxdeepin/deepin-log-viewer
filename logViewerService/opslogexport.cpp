// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "opslogexport.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QBuffer>
#include <QDirIterator>
#include <QFileInfo>

using namespace std;

static void appendToFile(const QString &filePath, const QByteArray &content)
{
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        file.write(content);
    } else {
        qWarning() << "Error opening file for writing:" << file.errorString();
    }
}

/**
 * @brief 在 QByteArray 数据中查找包含指定模式的行，并返回该行及其后面的N行。
 *        功能类似于 `grep -A N pattern`，但操作的是内存中的数据。
 *
 * @param data     包含文本数据的 QByteArray。
 * @param pattern  要搜索的字符串模式。
 * @param afterLines 匹配成功后，要额外包含的后续行数 (N)。
 * @return QList<QStringList> 每个 QStringList 代表一个匹配块。
 */
QList<QStringList> grepA(const QByteArray &data, const QString &pattern, int afterLines)
{
    QList<QStringList> allMatches;

    // 使用 QBuffer 将 QByteArray 包装成一个 QIODevice
    QBuffer buffer;
    buffer.setData(data);
    if (!buffer.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open QBuffer for reading.";
        return allMatches;
    }

    // QTextStream 可以直接从任何 QIODevice 读取数据
    QTextStream in(&buffer);
    // 假设数据是 UTF-8 编码，如果不是，需要设置正确的 codec
    // in.setCodec("UTF-8");

    while (!in.atEnd()) {
        QString line = in.readLine();

        // 检查当前行是否包含目标模式
        if (line.contains(pattern)) {
            QStringList currentMatchGroup;
            currentMatchGroup.append(line); // 添加匹配到的行

            // 读取并添加后续的 'afterLines' 行
            for (int i = 0; i < afterLines && !in.atEnd(); ++i) {
                currentMatchGroup.append(in.readLine());
            }

            // 将这个匹配块添加到总的结果列表中
            allMatches.append(currentMatchGroup);
        }
    }

    return allMatches;
}

static void appendToFileWithGrepA(const QString &filePath, const QByteArray &content, const QString &pattern, int afterLines)
{
    QList<QStringList> matches = grepA(content, pattern, afterLines);

    if (matches.isEmpty()) {
        appendToFile(filePath, {});
    } else {
        QFile file(filePath);

        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            bool firstGroup = true;
            for (const QStringList &group : matches) {
                if (!firstGroup) {
                    file.write("--\n");
                }
                for (const QString &line : group) {
                    file.write(line.toLocal8Bit());
                    file.write("\n");
                }
                firstGroup = false;
            }
        } else {
            qWarning() << "Error opening file for writing:" << file.errorString();
        }
    }
}

static int runProcess(const QStringList &args, const char *outputFile = nullptr, const QString &pattern = "", int afterLines = -1)
{
    if (args.isEmpty()) {
        qWarning() << "runProcess: args is empty";
        return -1;
    }

    qDebug() << "runProcess" << args;

    QProcess proc;
    proc.setProgram(args.at(0));
    proc.setArguments(args.mid(1));
    proc.start(QIODevice::ReadWrite);
    proc.waitForFinished(-1);

    int exitcode = proc.exitCode();
    if (outputFile) {
        if (pattern != "" && afterLines >= 0) {
            appendToFileWithGrepA(outputFile, proc.readAllStandardOutput(), pattern, afterLines);
        } else {
            appendToFile(outputFile, proc.readAllStandardOutput());
        }
    }

    qDebug() << "runProcess exitcode:" << exitcode << proc.errorString();
    proc.close();
    return exitcode;
}

static QStringList expandPathWithWildcardIterator(const QString &pathWithWildcard)
{
    // 1. 分离目录和文件名模式
    QFileInfo fileInfo(pathWithWildcard);
    QString dirPath = fileInfo.path();
    QString fileNamePattern = fileInfo.fileName();

    QStringList matchedFiles;
    QStringList nameFilters;
    nameFilters << fileNamePattern;

    // 2. 创建 QDirIterator
    // 构造函数参数：目录, 名称过滤器, 实体类型过滤器, 迭代器标志
    QDirIterator it(dirPath, nameFilters, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

    // 3. 循环遍历所有匹配项
    while (it.hasNext()) {
        // it.next() 直接返回下一个匹配项的完整路径
        matchedFiles.append(it.next());
    }

    return matchedFiles;
}

OpsLogExport::OpsLogExport(const string &target)
    : target_dir(target)
{
}

void OpsLogExport::run()
{
    // 注意：需要优先创建目录结构
    createDirStruct();

    exportAppLogs();
    exportSystemLogs();
    exportKernelLogs();
    exportDDELogs();
    exportHWLogs();
    exportOSVersionLogs();
    exportAptLogs();
    exportUosSteLogs();
    exportUosSteTwoLogs();
}

bool OpsLogExport::path_exists(const string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool OpsLogExport::create_directories(const string &path)
{
    return QDir(QString::fromStdString(path)).mkpath(".");
}

void OpsLogExport::copy_file_or_dir(const string &src, const string &dst_dir)
{
    if (!path_exists(src)) return;

    QString qSrc = QString::fromStdString(src);
    QString qDst = QString::fromStdString(dst_dir);

    QFileInfo srcInfo(qSrc);
    // 符号链接源一律不拷贝：避免引入指向特殊文件/系统文件的链接。新流程下整个收集、
    // 压缩、回传、清理均在 root 一次 D-Bus 调用内闭环完成，无前端直接读取临时目录的环节，
    // 故无需再对目录内残留的符号链接做统一清理。
    if (srcInfo.isSymLink()) return;
    if (srcInfo.isFile()) {
        // 单个文件：确保目标目录存在后用 QFile::copy
        QDir().mkpath(qDst);
        QString dstFile = qDst + "/" + srcInfo.fileName();
        QFile::remove(dstFile);  // QFile::copy 要求目标不存在
        QFile::copy(qSrc, dstFile);
    } else {
        // 目录：-rP 复制链接本身而非目标内容。导出目录最终整体压缩写入 fd，
        // zip 默认不跟随符号链接（仅存储链接路径），前端 unzip 也仅还原链接条目，
        // 不存在跟随链接读到非预期内容或阻塞的风险。
        runProcess({"cp", "-rP", qSrc, qDst});
    }
}

void OpsLogExport::execute_command(const QStringList &args, const string &output_file)
{
    runProcess(args, output_file.c_str());
}

void OpsLogExport::createDirStruct()
{
    // 创建目录结构
    vector<string> dirs = {
        target_dir + "/kernel",
        target_dir + "/system",
        target_dir + "/dde",
        target_dir + "/app",
        target_dir + "/app/deepin-defender",
        target_dir + "/app/deepin-cloud-print",
        target_dir + "/app/deepin-cloud-scan",
        target_dir + "/app/dde-printer",
        target_dir + "/app/deepin-graphics-driver-manager",
        target_dir + "/app/deepin-boot-maker",
        target_dir + "/app/deepin-scaner",
        target_dir + "/app/kms",
        target_dir + "/app/deepin-compressor",
        target_dir + "/app/dde-calendar",
        target_dir + "/app/deepin-manual",
        target_dir + "/app/deepin-reader",
        target_dir + "/app/deepin-font-manager",
        target_dir + "/app/deepin-deb-installer",
        target_dir + "/app/deepin-terminal",
        target_dir + "/app/deepin-voice-note",
        target_dir + "/app/deepin-devicemanager",
        target_dir + "/app/uos-service-support",
        target_dir + "/app/uos-remote-assistance",
        target_dir + "/app/deepin-system-monitor",
        target_dir + "/app/deepin-editor",
        target_dir + "/app/deepin-calculator",
        target_dir + "/app/deepin-mail",
        target_dir + "/app/deepin-screen-recorder",
        target_dir + "/app/deepin-draw",
        target_dir + "/app/deepin-music",
        target_dir + "/app/deepin-image-viewer",
        target_dir + "/app/deepin-album",
        target_dir + "/app/deepin-movie",
        target_dir + "/app/deepin-camera",
        target_dir + "/app/chineseime",
        target_dir + "/app/deepin-installer",
        target_dir + "/app/deepin-recovery",
        target_dir + "/app/oem-custom",
        target_dir + "/app/uos-activator",
        target_dir + "/app/uos-activator/log",
        target_dir + "/app/fcitx",
        target_dir + "/app/deepin-diskmanager",
        target_dir + "/app/downloader",
        target_dir + "/app/kwin",
        target_dir + "/app/kbox",
        target_dir + "/app/deepin-log-viewer",
        target_dir + "/dde/dde-desktop",
        target_dir + "/dde/dde-file-manager",
        target_dir + "/dde/dde-dock",
        target_dir + "/system/pulseaudio",
        target_dir + "/apt",
        target_dir + "/uos-ste",
        target_dir + "/uosste_logs"
    };

    for (const auto& dir : dirs) {
        create_directories(dir);
    }
}

void OpsLogExport::exportAppLogs()
{
    // 云打印
    copy_file_or_dir("/var/log/cups/dcp_log", target_dir + "/app/deepin-cloud-print/");
    // 打印管理器
    copy_file_or_dir("/var/log/cups/error_log", target_dir + "/app/dde-printer/");
    // 显卡驱动管理器
    copy_file_or_dir("/var/log/deepin-graphics-driver-manager-server.log", target_dir + "/app/deepin-graphics-driver-manager/");
    // 启动盘制作工具
    copy_file_or_dir("/var/log/deepin/deepin-boot-maker-service.log", target_dir + "/app/deepin-boot-maker/");
    // 安装器
    copy_file_or_dir("/var/log/deepin-installer.log", target_dir + "/app/deepin-installer/");
    copy_file_or_dir("/var/log/deepin-installer-first-boot.log", target_dir + "/app/deepin-installer/");
    // 备份还原
    copy_file_or_dir("/recovery/deepin-recovery-gui.log", target_dir + "/app/deepin-recovery/");
    copy_file_or_dir("/recovery/deepin-recovery-gui-old.log", target_dir + "/app/deepin-recovery/");
    // ISO定制工具
    copy_file_or_dir("/var/local/oem-custom-tool/oem-custom-tool.log", target_dir + "/app/oem-custom/");
    copy_file_or_dir("/var/local/oem-custom-tool/oem-custom-tool-bk.log", target_dir + "/app/oem-custom/");
    copy_file_or_dir("/root/.cache/isocustomizer-agent/iso-customizer-agent/iso-customizer-agent.log", target_dir + "/app/oem-custom/");
    // 授权管理客户端(1020及之后版本日志)
    copy_file_or_dir("/var/log/uos/uos-license-agent", target_dir + "/app/uos-activator/log/");
    copy_file_or_dir("/var/log/uos/uos-activator-kms", target_dir + "/app/uos-activator/log/");
    // 输入法配置
    //    executCmd(("cp -rf /tmp/fcitx*.log " + target_dir + "/app/fcitx/").c_str());
    // 磁盘管理器
    copy_file_or_dir("/var/log/deepin/deepin-diskmanager-service/Log", target_dir + "/app/deepin-diskmanager/");
    // 窗口管理器
    runProcess({"lspci", "-v"}, (target_dir + "/app/kwin/lspci_VGA.log").c_str(), "VGA", 19);
    // 安卓容器
    copy_file_or_dir("/usr/share/log/log.txt", target_dir + "/app/kbox/");
    // 日志收集工具
    copy_file_or_dir("/var/log/deepin/deepin-log-viewer-service", target_dir + "/app/deepin-log-viewer/");
}

void OpsLogExport::exportSystemLogs()
{
    // 系统
    execute_command({"dmesg", "-T"}, target_dir + "/system/dmesg_system.log");
    execute_command({"journalctl"}, target_dir + "/system/journalctl.log");
    execute_command({"journalctl", "-b", "-0"}, target_dir + "/system/journalctl-0.log");
    execute_command({"journalctl", "-b", "-1"}, target_dir + "/system/journalctl-1.log");
    execute_command({"journalctl", "-xb"}, target_dir + "/system/journalctl-xb.log");
    copy_file_or_dir("/var/log/apt/history.log", target_dir + "/system/");
    copy_file_or_dir("/var/log/alternatives.log", target_dir + "/system/");
    copy_file_or_dir("/var/log/kern.log", target_dir + "/system/");
    copy_file_or_dir("/var/log/syslog", target_dir + "/system/");
    copy_file_or_dir("/var/log/dpkg.log", target_dir + "/system/");
    // xorg /var/log/目录下所有log文件
    {
        QStringList xorgFiles = expandPathWithWildcardIterator("/var/log/Xorg*");
        if (!xorgFiles.isEmpty()) {
            QStringList cpArgs;
            cpArgs << "cp" << "-rf" << xorgFiles << QString::fromStdString(target_dir + "/system/");
            runProcess(cpArgs);
        }
    }
}

void OpsLogExport::exportKernelLogs()
{
    // 内核
    copy_file_or_dir("/etc/product-info", target_dir + "/kernel/");
    execute_command({"uname", "-a"}, target_dir + "/kernel/uname-a.log");
    execute_command({"dmesg"}, target_dir + "/kernel/dmesg.log");
    copy_file_or_dir("/sys/firmware/acpi/tables/DSDT", target_dir + "/kernel/");
    copy_file_or_dir("/var/log/lightdm/lightdm.log", target_dir + "/kernel/");
    {
        QStringList xorgFiles = expandPathWithWildcardIterator("/var/log/Xorg.0.log*");
        if (!xorgFiles.isEmpty()) {
            QStringList cpArgs;
            cpArgs << "cp" << "-rf" << xorgFiles << QString::fromStdString(target_dir + "/kernel/");
            runProcess(cpArgs);
        }
    }
    runProcess({"lspci", "-vvv"}, (target_dir + "/kernel/lspci_VGA.log").c_str(), "VGA c", 12);
    runProcess({"dmesg"}, (target_dir + "/kernel/dmesg_network.log").c_str(), "iwlwifi", 0);
    execute_command({"journalctl", "--system"}, target_dir + "/kernel/journalctl_system.log");
    // ⽆法识别声卡问题⽇志
    execute_command({"lshw", "-c", "sound"}, target_dir + "/kernel/sound_info.log");
    // 龙芯内核
    copy_file_or_dir("/var/log/kern.log", target_dir + "/kernel/");
}

void OpsLogExport::exportDDELogs()
{
    // 文件管理器
    copy_file_or_dir("/var/log/deepin/dde-file-manager-daemon", target_dir + "/dde/dde-file-manager/");
    copy_file_or_dir("/var/log/messages", target_dir + "/dde/");
    copy_file_or_dir("/var/log/syslog", target_dir + "/dde/");
    execute_command({"coredumpctl", "list"}, target_dir + "/dde/coredumpctl.log");
    execute_command({"free", "-m"}, target_dir + "/dde/free-m.log");   // 查看内存情况，输出内容截图或保存
    execute_command({"udisksctl", "dump"}, target_dir + "/dde/udiskctl_dump.txt");
    execute_command({"df", "-h"}, target_dir + "/dde/df-h.txt");
    copy_file_or_dir("/var/log/journalLog", target_dir + "/dde/");
}

void OpsLogExport::exportHWLogs()
{
    // hw-info
    execute_command({"dmidecode", "-t", "0"}, target_dir + "/system_info.txt");
    execute_command({"dmidecode", "-t", "11"}, target_dir + "/system_info.txt");
    execute_command({"cat", "/etc/hw_version"}, target_dir + "/system_info.txt");
    execute_command({"echo"}, target_dir + "/system_info.txt");
    execute_command({"/usr/sbin/hwfirmware", "-v"}, target_dir + "/system_info.txt");
    execute_command({"echo"}, target_dir + "/system_info.txt");
    execute_command({"lscpu"}, target_dir + "/system_info.txt");
}

void OpsLogExport::exportOSVersionLogs()
{
    execute_command({"uname", "-a"}, target_dir + "/info-version.txt");
    execute_command({"echo"}, target_dir + "/info-version.txt");
    execute_command({"cat", "/etc/os-version"}, target_dir + "/info-version.txt");
    execute_command({"echo"}, target_dir + "/info-version.txt");
    execute_command({"echo", "-n", "开发者模式开启状态："}, target_dir + "/info-version.txt");
    execute_command({"cat", "/var/lib/deepin/developer-mode/enabled"}, target_dir + "/info-version.txt");
    execute_command({"echo", "\\n"}, target_dir + "/info-version.txt");
    execute_command({"uos-activator-cmd", "-q"}, target_dir + "/info-version.txt");
}

void OpsLogExport::exportAptLogs()
{
    // apt日志：完整导出/var/log/apt目录下的所有内容
    copy_file_or_dir("/var/log/apt", target_dir);
}

void OpsLogExport::exportUosSteLogs()
{
    // uos-ste日志：完整导出/var/log/uos-ste目录下的所有内容
    copy_file_or_dir("/var/log/uos-ste", target_dir);
}

void OpsLogExport::exportUosSteTwoLogs()
{
    // uosste_logs日志：完整导出/var/log/uosste_logs目录下的所有内容
    copy_file_or_dir("/var/log/uosste_logs", target_dir);
}
