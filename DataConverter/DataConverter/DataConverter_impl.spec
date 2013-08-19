#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components DataConverter.
# 
# REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it under the terms of 
# the GNU Lesser General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
# PURPOSE.  See the GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License along with this 
# program.  If not, see http://www.gnu.org/licenses/.
#
 
# Invoke RPM building via 'rpmbuild -bb DataConverter_impl.spec' 

# Override default RPM directories to allow in-place creation of a binary RPM
%define _builddir %(pwd)
%define _rpmdir %(pwd)
%define _specdir %(pwd)

# By default, the RPM will install to the standard REDHAWK SDR root location (/var/redhawk/sdr)
# You can override this at install time using --prefix /new/sdr/root when invoking rpm (preferred)
# You can override this at build time and change the default using --define "_sdroot /new/sdr/root" when invoking rpmbuild (discouraged)
%{!?_sdrroot: %define _sdrroot /var/redhawk/sdr} 
%define _prefix %{_sdrroot}
Prefix: %{_prefix}

# Point install paths to locations within our target SDR root
%define _bindir %{_prefix}/dom/components/DataConverter/DataConverter_cpp_impl
%define _sysconfdir %{_prefix}/etc
%define _localstatedir %{_prefix}/var
%define _mandir %{_prefix}/man
%define _infodir  %{_prefix}/info

Summary: An implementation of DataConverter
Name: DataConverter-DataConverter_impl
Version: null
Release: 1
License: None
Group: None
Requires: redhawk >= 1.7.0
BuildRoot: %{_tmppath}/DataConverter-DataConverter_cpp_impl-root
BuildRequires: boost-devel >= 1.41
BuildRequires: libomniORB4.1-devel
BuildRequires: apache-log4cxx-devel >= 0.10
BuildRequires: redhawk >= 1.7.0
BuildRequires: autoconf automake libtool

# Specify a prefix so a user installing the RPM can change the SDRROOT with --prefix when installing
Prefix: %{_prefix}

%description

%prep
# No prep required - we'll take files as-is

%build
./reconf
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%dir %{_prefix}/dom/components/DataConverter
%{_prefix}/dom/components/DataConverter/DataConverter.spd.xml
%{_prefix}/dom/components/DataConverter/DataConverter.prf.xml
%{_prefix}/dom/components/DataConverter/DataConverter.scd.xml
%dir %{_prefix}/dom/components/DataConverter/DataConverter_cpp_impl
%{_prefix}/dom/components/DataConverter/DataConverter_cpp_impl/DataConverter_impl
