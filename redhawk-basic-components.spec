# Define default SDRROOT
%{!?_sdrroot:    %define _sdrroot    /var/redhawk/sdr}
%define _prefix    %{_sdrroot}

Name:		redhawk-basic-components
Version:	1.8.3
Release:	4%{?dist}
Summary:	A collection of starter components for REDHAWK
Prefix:		%{_sdrroot}

Group:		Applications/Engineering
License:	LGPLv3+
URL:		http://redhawksdr.org/	
Source0:	%{name}-%{version}.tar.gz

BuildRequires:	redhawk >= 1.8.3
BuildRequires:	autoconf automake libtool
BuildRequires:	apache-log4cxx-devel >= 0.10
BuildRequires:	boost-devel >= 1.41
BuildRequires:	python-devel >= 2.4
BuildRequires:	bulkioInterfaces
BuildRequires:	jdk >= 1.6
BuildRequires:	libomniORB4.1-devel
BuildRequires:	libomniORBpy3-devel
%if "%{?rhel}" == "6"
BuildRequires: libuuid-devel
%else
BuildRequires: e2fsprogs-devel
%endif
Requires:	redhawk >= 1.8.3
Requires:	apache-log4cxx >= 0.10
Requires:	python
Requires:	bulkioInterfaces
Requires:	java >= 1.6
Requires:	libomniORB4.1
Requires:	omniORBpy
%if "%{?rhel}" == "6"
Requires: libuuid
%else
Requires: e2fsprogs
%endif
Requires:	scipy
AutoReqProv:	no

%if "%{?rhel}" != "6"
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot
%endif


%description
A collection of starter components for REDHAWK


%prep
%setup -q


%build
# Setup environment for Java
export JAVA_HOME=/usr/java/default
export PATH=$JAVA_HOME/bin:$PATH

# Build the DSP library first
pushd dsp/Release
make %{?_smp_mflags}
popd

# Build components
for dir in agc/cpp AmFmPmBasebandDemod/cpp DataConverter/DataConverter DataReader/python \
           DataWriter/python fcalc/python freqfilter/python HardLimit/cpp HardLimit/java \
           HardLimit/python medianfilter/python SCAAudioTagger/python SigGen/cpp \
           SigGen/python SigGen/java TuneFilterDecimate/cpp whitenoise/cpp;
do
    pushd $dir
    ./reconf
    %configure
    make %{?_smp_mflags}
    popd
done


%install
rm -rf %{buildroot}

# Install the components
for dir in agc/cpp AmFmPmBasebandDemod/cpp DataConverter/DataConverter DataReader/python \
           DataWriter/python fcalc/python freqfilter/python HardLimit/cpp HardLimit/java \
           HardLimit/python medianfilter/python SCAAudioTagger/python SigGen/cpp \
           SigGen/python SigGen/java TuneFilterDecimate/cpp whitenoise/cpp;
do
    make -C $dir install DESTDIR=%{buildroot}
done

# Install the DSP library
mkdir -p %{buildroot}%{_sdrroot}/dom/components/dsp
install dsp/dsp.spd.xml %{buildroot}%{_sdrroot}/dom/components/dsp/dsp.spd.xml
mkdir -p %{buildroot}%{_sdrroot}/dom/components/dsp/Release
install dsp/Release/libdsp.so %{buildroot}%{_sdrroot}/dom/components/dsp/Release/libdsp.so


%clean
rm -rf %{buildroot}


%files
%defattr(-,redhawk,redhawk,-)
%{_sdrroot}/dom/components/agc
%{_sdrroot}/dom/components/BasebandDemod
%{_sdrroot}/dom/components/DataConverter
%{_sdrroot}/dom/components/DataReader
%{_sdrroot}/dom/components/DataWriter
%{_sdrroot}/dom/components/dsp
%{_sdrroot}/dom/components/fcalc
%{_sdrroot}/dom/components/freqfilter
%{_sdrroot}/dom/components/HardLimit
%{_sdrroot}/dom/components/medianfilter
%{_sdrroot}/dom/components/SCAAudioTagger
%{_sdrroot}/dom/components/SigGen
%{_sdrroot}/dom/components/TuneFilterDecimate
%{_sdrroot}/dom/components/whitenoise


%changelog
* Tue Mar 12 2013 1.0.0-1
- Initial release

