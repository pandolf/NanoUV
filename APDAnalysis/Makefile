CC = g++
CFLAGS = -Wall -c -g -fpermissive 

#ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
#ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I.. -I$(ROOTSYS)/include  -I../CommonTools/ -I../CommonTools/interface/ -I../interface/  #-I$(ROOFIT_INCLUDE)/ C-I$(CMSSW_BASE)/src -I$(CMSSW_BASE)/src/NanoUVCommonTools  

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -lHtml -lMathCore -lGenVector -lMinuit -lTreePlayer -lRooFitCore -lRooFit #-L$(ROOFIT_LIBDIR)/ 


APDGraph: APDGraph.cpp NanoUVCommon.o
	$(CC) -Wall $(INCLUDES) -o APDGraph APDGraph.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)



NanoUVCommon.o: ../CommonTools/src/NanoUVCommon.cc 
	$(CC) -Wall $(INCLUDES) -c ../CommonTools/src/NanoUVCommon.cc $(ROOTFLAG) $(EXTRALIBS)



clean:
	rm *.o APDGraph
