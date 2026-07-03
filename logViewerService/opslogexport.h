// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OPS_LOG_EXPORT_H
#define OPS_LOG_EXPORT_H

#include <string>
#include <QStringList>

class OpsLogExport {
public:
    explicit OpsLogExport(const std::string& target);

    void run();

private:
    std::string target_dir;

    void createOpsLogDirStruct(const QString &outDir);

    bool path_exists(const std::string& path);
    void copy_file_or_dir(const std::string& src, const std::string& dst_dir);
    void execute_command(const QStringList &args, const std::string &output_file);

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
