%define pkgrelease  1
%if 0%{?openeuler}
%define specrelease %{pkgrelease}
%else
## allow specrelease to have configurable %%{?dist} tag in other distribution
%define specrelease %{pkgrelease}%{?dist}
%endif

Name:           deepin-log-viewer
Version:        5.8.0.3
Release:        %{specrelease}
Summary:        Log Viewer is a useful tool for viewing system logs
License:        GPLv3+
URL:            https://github.com/linuxdeepin/%{name}
Source0:        %{name}-%{version}.tar.gz

BuildRequires: gcc-c++
BuildRequires: cmake3
BuildRequires: dtkwidget-devel
BuildRequires: systemd-devel
BuildRequires: libicu-devel
BuildRequires: qt5-rpm-macros
BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qttools-devel

%description
Log Viewer is a useful tool for viewing system logs.

%prep
%autosetup
# %setup -q
####sed -i 's|lrelease|lrelease-qt5|' translations/translate_generation.sh

# %patch0 -p1

%build
export PATH=%{_qt5_bindir}:$PATH
sed -i "s|^cmake_minimum_required.*|cmake_minimum_required(VERSION 3.0)|" $(find . -name "CMakeLists.txt")
mkdir build && pushd build 
%cmake -DCMAKE_BUILD_TYPE=Release ../  -DAPP_VERSION=%{version} -DVERSION=%{version} 
%make_build  
popd

%install
%make_install -C build INSTALL_ROOT="%buildroot"



%files
%doc README.md
%license LICENSE
%{_bindir}/%{name}
%{_bindir}/logViewerAuth
%{_bindir}/logViewerTruncate
%{_datadir}/applications/%{name}.desktop
%{_datadir}/%{name}/translations/%{name}*
%{_datadir}/icons/hicolor/scalable/apps/%{name}.svg
%{_datadir}/polkit-1/actions/*.policy



%changelog
* Tue May 28 2019 Robin Lee <cheeselee@fedoraproject.org> - 5.8.0.3-1
- Update to 5.8.0.3
