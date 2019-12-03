#include "logapplicationhelper.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLocale>

std::atomic<LogApplicationHelper *> LogApplicationHelper::m_instance;
std::mutex LogApplicationHelper::m_mutex;

LogApplicationHelper::LogApplicationHelper(QObject *parent)
    : QObject(parent)
{
    init();
}

void LogApplicationHelper::init()
{
    m_desktop_files.clear();
    m_log_files.clear();

    m_en_log_map.clear();
    m_en_trans_map.clear();
    m_trans_log_map.clear();

    // get current system language shortname
    m_current_system_language = QLocale::system().name();

    // get desktop & log files
    createDesktopFiles();
    createLogFiles();
}

void LogApplicationHelper::createDesktopFiles()
{
    QString path = "/usr/share/applications";
    QDir dir(path);
    if (!dir.exists())
        return;

    QStringList fileInfoList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    for (QString desktop : fileInfoList) {
        if (desktop.contains("deepin-") || desktop.contains("dde-")) {
            m_desktop_files.append(desktop);
        }
    }

    for (QString var : m_desktop_files) {
        QString filePath = path + "/" + var;

        QFile fi(filePath);
        if (!fi.open(QIODevice::ReadOnly))
            continue;

        bool isDeepin = false;

        while (!fi.atEnd()) {
            QString lineStr = fi.readLine();
            lineStr.replace("\n", "");

            if (!lineStr.contains("X-Deepin-Vendor", Qt::CaseInsensitive)) {
                continue;
            }

            QStringList _x_vendor_list = lineStr.split("=", QString::SkipEmptyParts);
            if (_x_vendor_list.count() != 2) {
                continue;
            }

            QString rval = _x_vendor_list[1];
            if (0 == rval.compare("deepin", Qt::CaseInsensitive)) {
                isDeepin = true;
                break;
            }
        }
        fi.close();

        parseField(filePath, var, isDeepin);
    }
}
#if 0

            if (!lineStr.contains("GenericName"))
                continue;

            QStringList gNameList = lineStr.split("=", QString::SkipEmptyParts);
            if (gNameList.count() != 2)
                continue;

            QString leftStr = gNameList[0];
            QString genericName = gNameList[1];

            if (leftStr.split("_").count() == 2) {
                if (leftStr.contains(m_current_system_language)) {
                    m_en_trans_map.insert(var.section(".", 0, 0), genericName);
                    break;
                }
            } else if (leftStr.contains(m_current_system_language.split("_")[0])) {
                m_en_trans_map.insert(var.section(".", 0, 0), genericName);
                break;
            }
        }
}
#endif

void LogApplicationHelper::createLogFiles()
{
    QString homePath = QDir::homePath();
    if (homePath.isEmpty()) {
        return;
    }
    QString path = homePath + "/.cache/deepin/";
    QDir appDir(path);
    if (!appDir.exists()) {
        return;
    }

    m_log_files = appDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    for (auto i = 0; i < m_desktop_files.count(); ++i) {
        QString _name = m_desktop_files[i].section(".", 0, 0);
        for (auto j = 0; j < m_log_files.count(); ++j) {
            if (_name == m_log_files[j]) {
                QString logPath = path + m_log_files[j];
                m_en_log_map.insert(_name, logPath);
                break;
            }
        }
    }
}

void LogApplicationHelper::parseField(QString path, QString name, bool isDeepin)
{
    QFile fi(path);
    if (!fi.open(QIODevice::ReadOnly)) {
        return;
    }

    // insert map at first, en-en, then repalce transName if has name,
    m_en_trans_map.insert(name.section(".", 0, 0), name.section(".", 0, 0));  // desktop name

    while (!fi.atEnd()) {
        QString lineStr = fi.readLine();
        lineStr.replace("\n", "");

        if (isDeepin) {
            // GenericName所对应的当前语言的值=》GenericName无任何语言标记的值=》Name对应语言的值
            // =》Name无任何语言标记的值 =》desktop的名称
            if (!lineStr.startsWith("GenericName") && !lineStr.startsWith("Name"))
                continue;

        } else {
            // Name对应语言的值 =》Name无任何语言标记的值 =》desktop的名称
            if (!lineStr.startsWith("Name"))
                continue;
        }

        QStringList gNameList = lineStr.split("=", QString::SkipEmptyParts);
        if (gNameList.count() != 2)
            continue;

        QString leftStr = gNameList[0];
        QString genericName = gNameList[1];

        // could not find GenericName[...], use GenericName=
        m_en_trans_map.insert(name.section(".", 0, 0), genericName);  // GenericName=xxxx

        if (leftStr.split("_").count() == 2) {
            if (leftStr.contains(m_current_system_language)) {
                m_en_trans_map.insert(name.section(".", 0, 0), genericName);
                break;
            }
        } else if (leftStr.contains(m_current_system_language.split("_")[0])) {
            m_en_trans_map.insert(name.section(".", 0, 0), genericName);
            break;
        }
    }
}

QString LogApplicationHelper::getLogFile(QString path)
{
    QString ret;
    QDir subdir(path);
    if (!subdir.exists())
        return ret;

    QStringList logFiles = subdir.entryList(QDir::NoDotAndDotDot | QDir::Files);

    for (int j = 0; j < logFiles.count(); j++) {
        QString fileName = logFiles.at(j);
        if (!fileName.endsWith(".log"))
            continue;
        ret = QString("%1/%2").arg(path).arg(fileName);
        break;
    }
    return ret;
}

QMap<QString, QString> LogApplicationHelper::getMap()
{
    QMap<QString, QString>::const_iterator iter = m_en_log_map.constBegin();
    while (iter != m_en_log_map.constEnd()) {
        QString displayName = m_en_trans_map.value(iter.key());
        QString logPath = getLogFile(iter.value());
        m_trans_log_map.insert(displayName, logPath);
        ++iter;
    }
    return m_trans_log_map;
}

QString LogApplicationHelper::transName(QString str)
{
    return m_en_trans_map.value(str);
}
