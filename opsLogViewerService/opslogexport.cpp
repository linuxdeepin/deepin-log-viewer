// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
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

using namespace std;

OpsLogExport::OpsLogExport(const string &target, const string &home)
    : target_dir(target)
    , home_dir(home)
{
}

void OpsLogExport::run()
{
}

bool OpsLogExport::path_exists(const string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool OpsLogExport::create_directories(const string &path)
{
    string cmd = "mkdir -p " + path;
    return system(cmd.c_str()) == 0;
}

void OpsLogExport::copy_file_or_dir(const string &src, const string &dst_dir)
{
    if (!path_exists(src)) return;
    string cmd = "cp -rf " + src + " " + dst_dir + " 2>/dev/null";
    system(cmd.c_str());
}

void OpsLogExport::execute_command(const string &cmd, const string &output_file)
{
    string full_cmd = cmd + " >> " + output_file;
    system(full_cmd.c_str());
}
