# Spec file for the package 'pvvgname'
# Copyright (C) 2016 - 2022 rene.reucher@batcom-it.net

Name:		pvvgname
Version:	1.10
Release:	1
License:	GPL-3.0
Summary:	Reads the name of the VG that a PV belongs to directly from the MDA stored on disk 
Group:		System/Administration
Source0:	%{name}-%{version}.tar.bz2
BuildRequires:	gcc make
BuildRoot:      %{_tmppath}/%{name}-%{version}

%description
Reads the name of the VG that a PV belongs to directly from the MDA stored on disk.

%prep
%setup -qcT
tar -xjf %{SOURCE0}

%build
pushd %{name}
make
popd

%install
pushd %{name}
make install PREFIX=$RPM_BUILD_ROOT/usr
popd

%files
%defattr(-,root,root)
%doc %{name}/README %{name}/COPYING %{name}/CHANGES
%{_sbindir}/pvvgname
%{_mandir}/man8/pvvgname.8.gz
