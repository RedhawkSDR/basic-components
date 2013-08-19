#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components HardLimit.
# 
# REDHAWK Basic Components HardLimit is free software: you can redistribute it and/or modify it under the terms of 
# the GNU Lesser General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components HardLimit is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
# PURPOSE.  See the GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License along with this 
# program.  If not, see http://www.gnu.org/licenses/.
#
 
HardLimit.jar: $(HardLimit_jar_SOURCES)
	mkdir -p bin
	$(JAVAC) -cp $(OSSIE_HOME)/lib/CFInterfaces.jar:$(OSSIE_HOME)/lib/log4j-1.2.15.jar:$(OSSIE_HOME)/lib/ossie.jar:$(OSSIE_HOME)/lib/BULKIOInterfaces.jar:$(OSSIE_HOME)/lib/bulkio.jar -d bin $(HardLimit_jar_SOURCES)
	$(JAR) cf ./HardLimit.jar -C bin .

clean-local:
	rm -rf bin

distclean-local:
	rm -f config.*
	rm -rf autom4te.cache
	rm -f acinclude.m4
	rm -f aclocal.m4
	rm -f configure
	rm -f depcomp
	rm -f install-sh
	rm -f ltmain.sh
	rm -f Makefile.in
	rm -f missing
	rm -rf .deps

ossieName = HardLimit
bindir = $(prefix)/dom/components/HardLimit/java/
bin_PROGRAMS = HardLimit.jar
HardLimit_jar_SOURCES := $(shell find ./src -name "*.java")

xmldir = $(prefix)/dom/components/HardLimit/
dist_xml_DATA = ../HardLimit.scd.xml ../HardLimit.prf.xml ../HardLimit.spd.xml

domdir = $(prefix)/dom/components/HardLimit/java/
dist_dom_SCRIPTS = startJava.sh
