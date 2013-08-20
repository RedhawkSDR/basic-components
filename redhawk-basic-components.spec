# Define default SDRROOT
%{!?_sdrroot:    %define _sdrroot    /var/redhawk/sdr}
%define _prefix    %{_sdrroot}

Name:		redhawk-basic-components
Version:	1.9.0
Release:	2.1%{?dist}
Summary:	A collection of starter components for REDHAWK
Prefix:		%{_sdrroot}

Group:		Applications/Engineering
License:	LGPLv3+
URL:		http://redhawksdr.org/	
Source0:	%{name}-%{version}.tar.gz
Vendor:     REDHAWK

BuildRequires:	redhawk-devel >= 1.9
BuildRequires:	bulkioInterfaces
BuildRequires:  fftw-devel
Requires:	redhawk >= 1.9
Requires:	bulkioInterfaces
Requires:	scipy
Requires:       fftw

AutoReqProv:	no

%if 0%{?rhel} < 6
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot
%endif

%description
A collection of starter components for REDHAWK


%prep
%setup -q


%build
# Setup environment for Java
. /usr/share/java-utils/java-functions && set_jvm
export PATH=$JAVA_HOME/bin:$PATH

# Build the DSP library first
pushd dsp/Release
make %{?_smp_mflags}
popd

#Next build the fftlib library

pushd fftlib/Release
make %{?_smp_mflags}
popd

# Build components
for dir in agc/cpp AmFmPmBasebandDemod/cpp DataConverter/DataConverter DataReader/python \
           DataWriter/python fastfilter/cpp fcalc/python freqfilter/python HardLimit/cpp HardLimit/java \
           HardLimit/python medianfilter/python psd/cpp SigGen/cpp \
           SigGen/python SigGen/java sinksocket/cpp sourcesocket/cpp \
           TuneFilterDecimate/cpp whitenoise/cpp;
do
    cd $dir
    ./reconf
    %configure
    make %{?_smp_mflags}
    cd -
done


%install
rm -rf %{buildroot}

# Install the components
for dir in agc/cpp AmFmPmBasebandDemod/cpp DataConverter/DataConverter DataReader/python \
           DataWriter/python fastfilter/cpp fcalc/python freqfilter/python HardLimit/cpp HardLimit/java \
           HardLimit/python medianfilter/python psd/cpp SigGen/cpp \
           SigGen/python SigGen/java sinksocket/cpp sourcesocket/cpp \
           TuneFilterDecimate/cpp whitenoise/cpp;
do
    make -C $dir install DESTDIR=%{buildroot}
done

# Install the DSP library
mkdir -p %{buildroot}%{_sdrroot}/dom/components/dsp
install dsp/dsp.spd.xml %{buildroot}%{_sdrroot}/dom/components/dsp/dsp.spd.xml
mkdir -p %{buildroot}%{_sdrroot}/dom/components/dsp/Release
install dsp/Release/libdsp.so %{buildroot}%{_sdrroot}/dom/components/dsp/Release/libdsp.so

# Install the FFTLIB library
mkdir -p %{buildroot}%{_sdrroot}/dom/components/fftlib
install fftlib/fftlib.spd.xml %{buildroot}%{_sdrroot}/dom/components/fftlib/fftlib.spd.xml
mkdir -p %{buildroot}%{_sdrroot}/dom/components/fftlib/Release
install fftlib/Release/libfftlib.so %{buildroot}%{_sdrroot}/dom/components/fftlib/Release/libfftlib.so


%clean
rm -rf %{buildroot}


%files
%defattr(-,redhawk,redhawk,-)
%{_sdrroot}/dom/components/agc
%{_sdrroot}/dom/components/AmFmPmBasebandDemod
%{_sdrroot}/dom/components/DataConverter
%{_sdrroot}/dom/components/DataReader
%{_sdrroot}/dom/components/DataWriter
%{_sdrroot}/dom/components/dsp
%{_sdrroot}/dom/components/fastfilter
%{_sdrroot}/dom/components/fcalc
%{_sdrroot}/dom/components/fftlib
%{_sdrroot}/dom/components/freqfilter
%{_sdrroot}/dom/components/HardLimit
%{_sdrroot}/dom/components/medianfilter
%{_sdrroot}/dom/components/psd
%{_sdrroot}/dom/components/SigGen
%{_sdrroot}/dom/components/sinksocket
%{_sdrroot}/dom/components/sourcesocket
%{_sdrroot}/dom/components/TuneFilterDecimate
%{_sdrroot}/dom/components/whitenoise


%changelog
* Tue Jul 9 2013 - 1.9.0-1
- Update dependencies for Redhawk 1.9
- Add new components

* Fri Jul 5 2013 - 1.8.5
- Use OpenJDK

* Fri Mar 15 2013 - 1.8.4-3
- Update Java dependency to 1.6

* Tue Mar 12 2013 - 1.8.3-4
- Initial release

