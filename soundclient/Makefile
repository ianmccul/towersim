# $Id: Makefile 991 2012-07-16 06:58:31Z uqimccul $

SHELL = /bin/sh

TRUNKDIR = $(HOME)../
SRCDIR=.

prefix = $(HOME)
data_prefix = $(HOME)/.towersim

VPATH=../towersim:../change

CXXFLAGS:=$(CXXFLAGS) -pthread -I$(TRUNKDIR) -I../ -I/usr/include/qt4 -g3

cxxsrcglob = *.cpp $(patsubst %,%/*.cpp, $(subst :, ,$(VPATH))) \
	     $(patsubst %,%/*.cpp, $(subst :, ,$(GPATH)))

cxx_source := $(notdir $(wildcard $(cxxsrcglob)))
cxx_depend = $(cxx_source:%.cpp=%.d)

nodep-targets = clean mrproper dep uninstall dist distclean maintainer-clean dist-dir tar-dir tar

change_obj = touch.o change.o

ifeq (,$(filter $(nodep-targets),$(MAKECMDGOALS)))
-include $(cxx_depend)
endif

default : soundclient

dep : $(cxx_depend)

.PHONY: clean mrproper dep test all install uninstall dist-clean \
        maintaner-clean install-dirs dist-dir tar-dir tar install-models install-tools \
        tools models mp-tools default

# turn off some implicit rules that we don't want
%.o : %.cpp
%.o : %.cc
%.o : %.c
%.o : %.f

% :
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIB) $(LIBS_$@) -o $@

#% :
#	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIB) -o $@

%.o : %.cpp %.d
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o : %.f
	$(F77) $(FFLAGS) -c $< -o $@

%.d : %.cpp
	$(SRCDIR)/depend.sh -- $(MAKEDEPENDFLAGS) -- $< > $@

.SECONDARY: $(cxx_depend)

clean :
	rm -f *.o
	rm -f *.d

mrproper :
	rm -f *.o
	rm -f *.d
	rm -f -- $(patsubst %,%/*~, $(subst :, ,$(VPATH)))

depclean :
	rm -f *.d

soundclient : soundclient.o outputparser.o method.o change.o touch.o
LIBS_soundclient =  -lboost_date_time -lboost_system -lSDL2_mixer -lSDL2

install : soundclient
	test -d $(prefix)/bin || mkdir -p $(prefix)/bin
	install -m 0755 soundclient $(prefix)/bin
	test -d $(data_prefix)/bellsounds || mkdir -p $(data_prefix)/bellsounds
	find bellsounds/ -type f | while IFS= read  file; do \
	   install -m 0644 "$$file" $(data_prefix)/bellsounds; \
        done
	test -d $(data_prefix)/calls || mkdir -p $(data_prefix)/calls
	find calls/ -type f| while IFS= read file; do \
	   install -m 0644 "$$file" $(data_prefix)/calls; \
        done
