## deepin-log-viewer
Log Viewer is a fast and lightweight application for viewing system log.

日志收集工具是一款提供系统日志查看的软件工具。

#### Core
The GUI is based on [DTK](https://github.com/linuxdeepin/dtkwidget), Qt (Supported >= 5.12).

Build-Depends:
debhelper (>= 11), pkg-config, qt5-qmake, qt5-default, libdtkwidget-dev, libdtkgui-dev, qtbase5-private-dev, qttools5-dev-tools, libqt5svg5-dev, libsystemd-dev, libicu-dev

Execute Depends:
${shlibs:Depends}, ${misc:Depends}

#### Thridparty lib (source code)
- [docx](https://github.com/lpxxn/docx)
- [QtXlsx](http://qtxlsx.debao.me)

#### Installation
* sudo apt-get install deepin-log-viewer

#### Build
- mkdir build
- cd build
- qmake ..
- make

#### Usage
Caution: deepin-log-viewer is depend on polkit, So if build from source code, 
you have to add policy to usr/share/polkit-1/actions/. 
add com.deepin.pkexec.logViewerAuth.policy to /usr/bin.
If run in Qt Creator, you should set [Project][Run Settings] [Run] [Run configuration] to src from 2 projects authenticateProxy and src.

- sudo cp application/com.deepin.pkexec.logViewerAuth.policy /usr/share/polkit-1/actions/
- sudo cp build-deepin_log_viewer-unknown-Debug/logViewerAuth/logViewerAuth /usr/bin

- ./deepin-log-viewer

## [Wiki](https://wiki.deepin.org/)
## License

Deepin Log Viewer is licensed under [GPLv3](LICENSE) or any later version.


