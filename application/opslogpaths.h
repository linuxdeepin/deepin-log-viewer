// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OPSLOGPATHS_H
#define OPSLOGPATHS_H

// 运维日志导出的目录结构常量。
//
// 前端（application/utils.cpp 的 exportUserPermissionOpsLogs 系列）与
// root 服务（logViewerService/opslogexport.cpp 的 createOpsLogDirStruct）必须
// 使用同一份目录结构，避免前后端导出路径不一致，故提取为公共头。
//
// 注意：application 目标以 -std=c++11 编译，故使用 static constexpr（非 C++17
// 的 inline constexpr 变量），命名空间作用域下内部链接，多 TU 包含无 ODR 问题。

static constexpr char kKernelPath[] { "/kernel" };
static constexpr char kSystemPath[] { "/system" };
static constexpr char kDDEPath[] { "/dde" };
static constexpr char kAppPath[] { "/app" };
static constexpr char kJournalPath[] { "/journal" };
static constexpr char kAptPath[] { "/apt" };
static constexpr char kUosStePath[] { "/uos-ste" };
static constexpr char kUossteLogsPath[] { "/uosste_logs" };
static constexpr char kHisiPath[] { "/hisi" };
static constexpr char kDefenderPath[] { "/app/deepin-defender" };
static constexpr char kCloudPrintPath[] { "/app/deepin-cloud-print" };
static constexpr char kCloudScanPath[] { "/app/deepin-cloud-scan" };
static constexpr char kPrinterPath[] { "/app/dde-printer" };
static constexpr char kGraphicsDriverManagerPath[] { "/app/deepin-graphics-driver-manager" };
static constexpr char kBootMakerPath[] { "/app/deepin-boot-maker" };
static constexpr char kScanerPath[] { "/app/deepin-scaner" };
static constexpr char kKMSPath[] { "/app/kms" };
static constexpr char kCompressorPath[] { "/app/deepin-compressor" };
static constexpr char kCalendarPath[] { "/app/dde-calendar" };
static constexpr char kManualPath[] { "/app/deepin-manual" };
static constexpr char kReaderPath[] { "/app/deepin-reader" };
static constexpr char kFontManagerPath[] { "/app/deepin-font-manager" };
static constexpr char kDebInstallerPath[] { "/app/deepin-deb-installer" };
static constexpr char kTerminalPath[] { "/app/deepin-terminal" };
static constexpr char kVoiceNotPath[] { "/app/deepin-voice-note" };
static constexpr char kDevicemanagerPath[] { "/app/deepin-devicemanager" };
static constexpr char kServiceSupportPath[] { "/app/uos-service-support" };
static constexpr char kRemoteAssistancePath[] { "/app/uos-remote-assistance" };
static constexpr char kSystemMonitorPath[] { "/app/deepin-system-monitor" };
static constexpr char kEditorPath[] { "/app/deepin-editor" };
static constexpr char kCalculatorPath[] { "/app/deepin-calculator" };
static constexpr char kMailPath[] { "/app/deepin-mail" };
static constexpr char kScreenRecorderPath[] { "/app/deepin-screen-recorder" };
static constexpr char kDrawPath[] { "/app/deepin-draw" };
static constexpr char kMusicPath[] { "/app/deepin-music" };
static constexpr char kImageViewerPath[] { "/app/deepin-image-viewer" };
static constexpr char kAlbumPath[] { "/app/deepin-album" };
static constexpr char kMoviePath[] { "/app/deepin-movie" };
static constexpr char kCameraPath[] { "/app/deepin-camera" };
static constexpr char kChineseImePath[] { "/app/chineseime" };
static constexpr char kDeepinInstallerPath[] { "/app/deepin-installer" };
static constexpr char kDeepinRecoveryPath[] { "/app/deepin-recovery" };
static constexpr char kOemCustomPath[] { "/app/oem-custom" };
static constexpr char kUosActivatorPath[] { "/app/uos-activator" };
static constexpr char kUosActivatorLogPath[] { "/app/uos-activator/log" };
static constexpr char kFcitxPath[] { "/app/fcitx" };
static constexpr char kDiskManagerPath[] { "/app/deepin-diskmanager" };
static constexpr char kDownloaderPath[] { "/app/downloader" };
static constexpr char kKwinPath[] { "/app/kwin" };
static constexpr char kKboxPath[] { "/app/kbox" };
static constexpr char kDeepinLogViewerPath[] { "/app/deepin-log-viewer" };
static constexpr char kDdeDesktopPath[] { "/dde/dde-desktop" };
static constexpr char kDdeFileManagerPath[] { "/dde/dde-file-manager" };
static constexpr char kDdeDockPath[] { "/dde/dde-dock" };
static constexpr char kSystemPulseaudioPath[] { "/system/pulseaudio" };

#endif // OPSLOGPATHS_H
