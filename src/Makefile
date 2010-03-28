################ cegcc settings

BASE ?= /opt/mingw32ce
export BASE

RC = $(BASE)/bin/arm-mingw32ce-windres
RCFLAGS = -r -l 0x409 -Iinclude

CXX = $(BASE)/bin/arm-mingw32ce-g++
STRIP = $(BASE)/bin/arm-mingw32ce-strip

DLLTOOL = $(BASE)/bin/arm-mingw32ce-dlltool
DLLTOOLFLAGS =

CXXFLAGS = -Wall -O -g -march=armv4 -fno-exceptions -fno-rtti
LDFLAGS = -Wl,--major-subsystem-version=2,--minor-subsystem-version=10

LIBS = -lcoredll

VPATH := .

SRCS	 := $(notdir $(wildcard $(addsuffix /*.cpp, $(VPATH))))
OBJS     := $(SRCS:.cpp=.o)

OUTPUT_NAME := CEBatMon.exe

.PHONY: clean

all: $(OUTPUT_NAME)
	$(STRIP) --strip-unneeded $(OUTPUT_NAME)

$(OUTPUT_NAME): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LDFLAGS) $(LIBS)
	
%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OUTPUT_NAME)
	rm -f *.o
