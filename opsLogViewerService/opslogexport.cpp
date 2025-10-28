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
    // 注意：需要优先创建目录结构
    createDirStruct();
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
        target_dir + "/system/pulseaudio"
    };

    for (const auto& dir : dirs) {
        create_directories(dir);
    }
}
