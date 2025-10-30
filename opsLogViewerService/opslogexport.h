// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OPS_LOG_EXPORT_H
#define OPS_LOG_EXPORT_H

#include <string>

class OpsLogExport {
public:
    OpsLogExport(const std::string& target, const std::string& home);

    void run();

private:
    std::string target_dir;
    std::string home_dir;

    bool path_exists(const std::string& path);
    bool create_directories(const std::string& path);
    void copy_file_or_dir(const std::string& src, const std::string& dst_dir);
    void execute_command(const std::string& cmd, const std::string& output_file);

    void createDirStruct();
    void exportAppLogs();
    void exportSystemLogs();
    void exportKernelLogs();
    void exportDDELogs();
    void exportHWLogs();
    void exportOSVersionLogs();
    void exportDebVersionLogs();
};

#endif
