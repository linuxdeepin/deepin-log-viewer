// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OPS_LOG_EXPORT_H
#define OPS_LOG_EXPORT_H

#include <string>
#include <QStringList>

class OpsLogExport {
public:
    // callerGid：调用 DBus 的前端用户主组。导出目录属主保持 root，仅将 group 改为
    // callerGid 并设为 0750/0640——caller 以同组身份获得 r-x（可读不可写），既避免
    // 0755/0644 的世界可读泄露，又使 caller 无法在临时目录内植入符号链接（无 w 权限），
    // 从而消除 root 后续清理时跟随符号链接误删系统文件的风险。
    explicit OpsLogExport(const std::string& target, uint callerGid);

    void run();

private:
    std::string target_dir;
    uint m_callerGid;

    void createOpsLogDirStruct(const QString &outDir);

    bool path_exists(const std::string& path);
    void copy_file_or_dir(const std::string& src, const std::string& dst_dir);
    void execute_command(const QStringList &args, const std::string &output_file);
    void setDirectoryPermissionsSafe(const std::string& dir_path);

    // 验证目标目录在 /var/log 下（目录由 root 通过 QTemporaryDir 随机创建）
    bool ensureSafeTargetDir();

    void exportAppLogs();
    void exportSystemLogs();
    void exportKernelLogs();
    void exportDDELogs();
    void exportHWLogs();
    void exportOSVersionLogs();
    void exportAdditionalLogs();
    void exportAptLogs();
    void exportUosSteLogs();
    void exportUosSteTwoLogs();
};

#endif
