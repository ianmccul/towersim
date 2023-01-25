# $Id: Makefile.in 1094 2009-11-18 15:28:06Z ianmcc $

SHELL = /bin/sh

SRCDIR=.

CXXFLAGS:=$(CXXFLAGS) -std=c++11 -pthread -I.. -O2 -g -I/usr/include/eigen3

nodep-targets = clean mrproper dep uninstall dist distclean maintainer-clean dist-dir tar-dir tar

MAKEDEPEND = $(CXX) -MM -MT $@ $(CXXFLAGS) -o $*.Td $<

VPATH = ../ ../common/

# Makedepend with post-processing to add dummy rules for each dependency
MAKEDEPEND_INFO = $(MAKEDEPEND); \
        if [ -f $*.Td ] ; then cp $*.Td $*.d; \
          sed -e 's/\#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
              -e '/^$$/ d' -e 's/$$/ :/' < $*.Td >> $*.d; \
          rm -f $*.Td ; else \
          echo "warning: unable to obtain dependency information for $<"; fi


cxxsrcglob = *.cpp $(patsubst %,%/*.cpp, $(subst :, ,$(VPATH))) \
             $(patsubst %,%/*.cpp, $(subst :, ,$(GPATH)))

cxx_source := $(notdir $(wildcard $(cxxsrcglob)))
cxx_depend = $(cxx_source:%.cpp=%.d)

default : all

dep : $(cxx_depend)

.PHONY: clean mrproper dep test all install uninstall dist-clean \
        maintaner-clean install-dirs dist-dir tar-dir tar install-models install-tools \
        tools models mp-tools default

# turn off some implicit rules that we don't want
%.o : %.cpp
%.o : %.cc
%.o : %.c
%.o : %.f
%   : %.cpp
%   : %.o

% : %.cpp %.d
	@$(MAKEDEPEND_INFO)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(filter %.o %.cpp, $^) $(LIB) $(LIBS_$@) -o $@

%.o : %.cpp %.d
	@$(MAKEDEPEND_INFO)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o : %.f
	$(F77) $(FFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d :
	\

%.h : %.ui
	uic $< -o $@

# this is only relevant for Qt3?
#%.cpp : %.ui
#	uic -impl $*.h $< -o $@

.SECONDARY: $(cxx_depend)


LIBS_udp-daemon = -lboost_system -lboost_date_time

udp-daemon : sensortcpserver.o bells.o

executable = udp-daemon

all : $(executable)

clean :
	rm -f *.o
	rm -f *.d
	rm -f $(executable)

mrproper :
	rm -f *.o
	rm -f *.d
	rm -f -- $(patsubst %,%/*~, $(subst :, ,$(VPATH)))

depclean :
	rm -f *.d

ifeq (,$(filter $(nodep-targets),$(MAKECMDGOALS)))
-include $(cxx_depend)
endif
