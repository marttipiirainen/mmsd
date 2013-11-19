Name:       mmsd
Summary:    MMS daemon
Version:    0.1
Release:    1
Group:      Communications/Connectivity Adaptation
License:    GPLv2
URL:        http://git.kernel.org/pub/scm/network/ofono/
Source0:    %{name}-%{version}.tar.gz
Requires:   dbus
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(ofono)
BuildRequires:  libtool
BuildRequires:  automake
BuildRequires:  autoconf

%description
MMS daemon

%prep
%setup -q -n %{name}-%{version}
./bootstrap

%build
autoreconf --force --install
%configure
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%files
%defattr(-,root,root,-)
%doc COPYING ChangeLog AUTHORS README
%{_libexecdir}/*

