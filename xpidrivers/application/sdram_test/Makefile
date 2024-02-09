SOURCES = mem_model.cpp \
          test_mem_module.cpp \
          ../api/dma_interface.cpp \
          netviz_mem_module.cpp  \
          SDRAMTests.cpp \
          SdramTest.cpp 


OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = $(OBJECTS:.o=)
EXECUTABLES = SdramTest
SCRIPTITEMS = $(OBJECTS:.o=.script)
APP_CXXFLAGS = -O3 -Wno-write-strings
APP_ARCH	 ?= x86
LAST_BUILD = last_build.$(APP_ARCH)
CUR_BUILD = $(wildcard last_build.*)
ARM_CROSS_COMPILE ?= 
ARM_CXXFLAGS ?= 

all: cleanlastarch  $(EXECUTABLES)
	touch last_build.$(APP_ARCH)

.PHONY : cleanlastarch
cleanlastarch: 
		@echo "Checking if last built version was $(APP_ARCH)"
		@echo "LAST_BUILD: '$(LAST_BUILD)'"
		@echo "CUR_BUILD : '$(CUR_BUILD)'"
    ifneq ($(CUR_BUILD),$(LAST_BUILD))
			@echo "Last build was for different Architecture, cleaning"
			@$(MAKE) clean
    else
			@echo "Last build was for same Architecture"
    endif

clean:
	rm -rf *.o *.out *.depends last_build.* SdramTest

.PHONY: ARM
ARM: ARM_CROSS_COMPILE := $(CROSS_COMPILE) 
ARM:
	$(MAKE) APP_ARCH=arm CXX=$(CROSS_COMPILE)cxx ARM_CXXFLAGS=-DARM

install: $(EXECUTABLES)
	@mkdir -p $(INSTALLDIR)
	@cp -pf $(EXECUTABLES) $(INSTALLDIR)


$(EXECUTABLES): CXXFLAGS=$(ARM_CXXFLAGS) $(APP_CXXFLAGS)

dma_interface:  CXXFLAGS += -laio -ludev 
SdramTest: $(OBJECTS)
	g++ -Wall $(CXXFLAGS) $(OBJECTS) -o SdramTest -laio -ludev -lstdc++

