# Run tests in check section
# disable for bootstrapping

%bcond_with check

%global with_debug 1
%if 0%{?with_debug}
%global debug_package   %{nil}
%endif


###deepin-log-viewer-5.7.0.3.orig.tar.xz

Name:          deepin-log-viewer
Version:        5.8.0.3
Release:        1
Summary:        Log Viewer is a useful tool for viewing system logs.
License:        GPLv3+
URL:            https://uos-packages.deepin.com/uos/pool/main/d/deepin-log-viewer/
Source0:        %{name}_%{version}.orig.tar.xz


BuildRequires: dtkwidget-devel
BuildRequires: dtkgui-devel
BuildRequires: dtkcore-devel
BuildRequires: deepin-gettext-tools
BuildRequires: qt5-linguist
BuildRequires: qt5-qtmultimedia-devel
BuildRequires: qt5-qtx11extras-devel
BuildRequires: systemd-devel
BuildRequires: cmake
##BuildRequires: pkg-config
##BuildRequires: qt5-default
BuildRequires:  qt5
BuildRequires:  qt5-qtbase-private-devel
BuildRequires:  qt5-qtbase-devel
BuildRequires:  qt5-qtsvg-devel
BuildRequires: libicu-devel





%description
Log Viewer is a useful tool for viewing system logs.


%prep
%autosetup

%build
export PATH=$PATH:%{_qt5_bindir}
cmake_version=$(cmake --version | head -1 | awk '{print $3}')
sed -i "s|VERSION 3.13.4|VERSION $cmake_version|g" CMakeLists.txt
%{__cmake} .
make
#export PATH=$PATH:/usr/lib64/qt5/bin
#mkdir build && cd build
#%{_libdir}/qt5/bin/qmake ..
#%{__make}

%install
pushd %{_builddir}/%{name}-%{version}/build
%make_install INSTALL_ROOT=%{buildroot}
popd


%files
%{_bindir}/deepin-log-viewer
%{_bindir}/logViewerAuth
%{_datadir}/applications/deepin-log-viewer.desktop
%{_datadir}/deepin-log-viewer/translations/deepin-log-viewer.qm
%{_datadir}/deepin-log-viewer/translations/deepin-log-viewer_zh_CN.qm
%{_datadir}/icons/hicolor/scalable/apps/deepin-log-viewer.svg
%{_datadir}/polkit-1/actions/com.deepin.pkexec.logViewerAuth.policy
%doc README.md





%changelog
* Tue May 28 2019 Robin Lee <cheeselee@fedoraproject.org> - 5.8.0.3-1
- Update to 5.8.0.3

* Tue May 28 2019 Robin Lee <cheeselee@fedoraproject.org> - 3.17.0-2
- Fix a security issue

* Tue Feb 26 2019 mosquito <sensor.wen@gmail.com> - 3.17.0-1
- Update to 3.17.0

* Tue Feb 19 2019 Kalev Lember <klember@redhat.com> - 3.16.0-2
- Rebuilt against fixed atk (#1626575)

* Thu Jan 31 2019 mosquito <sensor.wen@gmail.com> - 3.16.0-1
- Update to 3.16.0

* Thu Jan 31 2019 Robin Lee <cheeselee@fedoraproject.org> - 3.12.0-3
- Create deepin-sound-player user

* Thu Jan 31 2019 Fedora Release Engineering <releng@fedoraproject.org> - 3.12.0-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_30_Mass_Rebuild

* Wed Dec 12 2018 mosquito <sensor.wen@gmail.com> - 3.12.0-1
- Update to 3.12.0

* Thu Nov 29 2018 mosquito <sensor.wen@gmail.com> - 3.10.0-1
- Update to 3.10.0

* Wed Nov 21 2018 mosquito <sensor.wen@gmail.com> - 3.9.0-1
- Update to 3.9.0

* Fri Nov  9 2018 mosquito <sensor.wen@gmail.com> - 3.5.0-1
- Update to 3.5.0

* Sat Aug 25 2018 mosquito <sensor.wen@gmail.com> - 3.1.26-1
- Update to 3.1.26
- build error with gobject-introspection 1.58 by gir-generator
  https://github.com/linuxdeepin/developer-center/issues/604

* Tue Jul 31 2018 Florian Weimer <fweimer@redhat.com> - 3.1.20-3
- Rebuild with fixed binutils

* Thu Jul 12 2018 Fedora Release Engineering <releng@fedoraproject.org> - 3.1.20-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_29_Mass_Rebuild

* Fri Feb 16 2018 mosquito <sensor.wen@gmail.com> - 3.1.20-1
- Update to 3.1.20

* Wed Feb 07 2018 Fedora Release Engineering <releng@fedoraproject.org> - 3.1.18.1-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_28_Mass_Rebuild

* Thu Jan 11 2018 Igor Gnatenko <ignatenkobrain@fedoraproject.org> - 3.1.18.1-2
- Remove obsolete scriptlets

* Thu Dec 21 2017 mosquito <sensor.wen@gmail.com> - 3.1.18.1-1
- Update to 3.1.18.1

* Wed Nov 15 2017 mosquito <sensor.wen@gmail.com> - 3.1.17-1
- Update to 3.1.17

* Fri Oct 27 2017 mosquito <sensor.wen@gmail.com> - 3.1.15-1
- Update to 3.1.15

* Mon Oct 16 2017 mosquito <sensor.wen@gmail.com> - 3.1.14-2
- Fix out of memory on armv7hl

* Sat Oct 14 2017 mosquito <sensor.wen@gmail.com> - 3.1.14-1
- Update to 3.1.14

* Sat Aug 26 2017 mosquito <sensor.wen@gmail.com> - 3.1.13-1
- Update to 3.1.13

* Tue Aug  8 2017 mosquito <sensor.wen@gmail.com> - 3.1.11-2
- Rename deepin-api-devel to golang-deepin-api-devel

* Tue Aug  1 2017 mosquito <sensor.wen@gmail.com> - 3.1.11-1
- Update to 3.1.11

* Fri Jul 14 2017 mosquito <sensor.wen@gmail.com> - 3.1.10-1.git79125e7
- Update to 3.1.10

* Fri May 19 2017 mosquito <sensor.wen@gmail.com> - 3.1.7-1.git4c8e030
- Update to 3.1.7

* Sun Feb 26 2017 mosquito <sensor.wen@gmail.com> - 3.1.2-1.gitf93dbd7
- Update to 3.1.2

* Tue Jan 17 2017 mosquito <sensor.wen@gmail.com> - 3.0.16.1-1.gitcfdb295
- Update to 3.0.16.1

* Mon Jan 16 2017 Jaroslav <cz.guardian@gmail.com> Stepanek 3.0.16-1
- Update to version 3.0.16

* Sun Dec 18 2016 Jaroslav <cz.guardian@gmail.com> Stepanek 3.0.15-1
- Update to version 3.0.15

* Wed Dec 07 2016 Jaroslav <cz.guardian@gmail.com> Stepanek 3.0.14-2
- Changed compilation procedure

* Wed Sep 28 2016 Jaroslav <cz.guardian@gmail.com> Stepanek 3.0.14-1
- Initial package build
