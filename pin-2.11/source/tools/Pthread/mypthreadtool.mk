SHELL = /bin/sh
.PHONY: all depend clean
.SUFFIXES: .cc .o

default: all

TARGET = ia32e
TOOLS_DIR = ..
EXTENGINELIB = true
include $(TOOLS_DIR)/makefile.gnu.config

LIBS = -L/usr/local/lib -lsnappy
INCS = -I/usr/local/include
#LIBS = -L/gpfs/home/juz138/work/benchmark/snappy/lib -lsnappy
#/home/mli55/mcsim/google-snappy
#INCS = -I/gpfs/home/juz138/work/benchmark/snappy/include

ifeq ($(TAG),dbg)
  DBG = -Wall
  OPT = -ggdb -g -O0
else
  DBG = -DNDEBUG
  OPT = -O3 -g
endif

CXXFLAGS_COMMON = -Wno-unknown-pragmas $(DBG) $(OPT) 
#CXX = g++ -m32
#CC  = gcc -m32
CXX = g++
CC  = gcc
PINFLAGS = 

SRCS_COMMON  = PthreadUtil.cc \
	PthreadAttr.cc \
	PthreadOnce.cc \
	PthreadKey.cc \
	PthreadMutexAttr.cc \
	PthreadMutex.cc \
	PthreadBarrier.cc \
	PthreadCondAttr.cc \
	PthreadCond.cc \
	PthreadCleanup.cc \
	PthreadCancel.cc \
	PthreadJoin.cc \
	PthreadMalloc.cc

ifneq ($(EXTENGINELIB),)
  SRCS = $(SRCS_COMMON) EEPthreadSim.cc EEPthreadScheduler.cc PTS.cc
  CXXFLAGS = $(CXXFLAGS_COMMON) -DEXTENGINE
else
  SRCS = $(SRCS_COMMON) PthreadSim.cc PthreadScheduler.cc
  CXXFLAGS = $(CXXFLAGS_COMMON)
endif

OBJS = $(patsubst %.cc,obj_$(TAG)/%.o,$(SRCS))
MYPIN_CXXFLAGS = $(subst -I../,-I$(TOOLS_DIR)/,$(PIN_CXXFLAGS))
MYPIN_LDFLAGS  = $(subst -L../,-L$(TOOLS_DIR)/,$(PIN_LDFLAGS))

all: obj_$(TAG)/mypthreadtool obj_$(TAG)/libmypthread.a
	cp -f obj_$(TAG)/mypthreadtool mypthreadtool
	cp -f obj_$(TAG)/libmypthread.a libmypthread.a

obj_$(TAG)/mypthreadtool : $(OBJS) obj_$(TAG)/mypthreadtool.o 
	$(CXX) $(OBJS) $@.o $(MYPIN_LDFLAGS) -o $@ $(PIN_LIBS) $(LIBS) -pthread
#	$(CXX) $(OBJS) $@.o $(MYPIN_LDFLAGS) -o $@ $(PIN_LIBS) $(LIBS) $(EXTENGINELIB) -pthread

obj_$(TAG)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $(MYPIN_CXXFLAGS) $(INCS) -o $@ $<

obj_$(TAG)/libmypthread.a : mypthread.cc
	$(CC) -c -o obj_$(TAG)/mypthread.o mypthread.cc
	ar ru obj_$(TAG)/libmypthread.a obj_$(TAG)/mypthread.o
	ranlib obj_$(TAG)/libmypthread.a

clean:
	-rm -f *.o mypthreadtool pin.log libmypthread.a

