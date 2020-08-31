# $Id: Makefile.in 1094 2009-11-18 15:28:06Z ianmcc $

SHELL = /bin/sh

TRUNKDIR = ../
SRCDIR=.

CXXFLAGS:=$(CXXFLAGS) -I$(TRUNKDIR)

CFLAGS:=$(CXXFLAGS) -I$(TRUNKDIR)

cxxsrcglob = *.cpp $(patsubst %,%/*.cpp, $(subst :, ,$(VPATH))) \
	     $(patsubst %,%/*.cpp, $(subst :, ,$(GPATH)))

cxx_source := $(notdir $(wildcard $(cxxsrcglob)))
cxx_depend = $(cxx_source:%.cpp=%.d)

csrcglob = *.c $(patsubst %,%/*.c, $(subst :, ,$(VPATH))) \
	     $(patsubst %,%/*.c, $(subst :, ,$(GPATH)))

c_source := $(notdir $(wildcard $(csrcglob)))
c_depend = $(c_source:%.c=%.d)


nodep-targets = clean mrproper dep uninstall dist distclean maintainer-clean dist-dir tar-dir tar

obj = touch.o change.o

ifeq (,$(filter $(nodep-targets),$(MAKECMDGOALS)))
-include $(cxx_depend)
-include $(c_depend)
endif

dep : $(cxx_depend) $(c_depend)

.PHONY: clean mrproper dep test all install uninstall dist-clean \
        maintaner-clean install-dirs dist-dir tar-dir tar install-models install-tools \
        tools models mp-tools default

# turn off some implicit rules that we don't want
%.o : %.cpp
%.o : %.cc
%.o : %.c
%.o : %.f

% : %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIB) $(LIBS_$@) -o $@

% : %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIB) -o $@

%.o : %.cpp %.d
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o : %.c %.d
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.f
	$(F77) $(FFLAGS) -c $< -o $@

%.d : %.cpp
	$(SRCDIR)/depend.sh -- $(MAKEDEPENDFLAGS) -- $< > $@

%.d : %.c
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

testmethod : $(obj) method.o parsemethod.o
testloadmethod : $(obj) method.o

learnmethod : $(obj) method.o tty_cbreak_raw.o
