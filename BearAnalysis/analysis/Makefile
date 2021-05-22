CC = g++
CFLAGS = -Wall -c -g -fpermissive 

#ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
#ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I.. -I$(ROOTSYS)/include  -I../../CommonTools/ -I../../CommonTools/interface/ -I../interface/  #-I$(ROOFIT_INCLUDE)/ C-I$(CMSSW_BASE)/src -I$(CMSSW_BASE)/src/NanoUVCommonTools  

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -lHtml -lMathCore -lGenVector -lMinuit -lTreePlayer -lRooFitCore -lRooFit #-L$(ROOFIT_LIBDIR)/ 


drawGain: drawGain.cpp NanoUVCommon.o
	$(CC) -Wall $(INCLUDES) -o drawGain drawGain.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawCurrent: drawCurrent.cpp NanoUVCommon.o
	$(CC) -Wall $(INCLUDES) -o drawCurrent drawCurrent.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawAPDGunScan: drawAPDGunScan.cpp NanoUVCommon.o GunScan.o
	$(CC) -Wall $(INCLUDES) -o drawAPDGunScan drawAPDGunScan.cpp NanoUVCommon.o GunScan.o $(ROOTFLAG) $(EXTRALIBS)

drawGainVsEnergy: drawGainVsEnergy.cpp NanoUVCommon.o 
	$(CC) -Wall $(INCLUDES) -o drawGainVsEnergy drawGainVsEnergy.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawBaseline: drawBaseline.cpp NanoUVCommon.o GunScan.o
	$(CC) -Wall $(INCLUDES) -o drawBaseline drawBaseline.cpp NanoUVCommon.o GunScan.o $(ROOTFLAG) $(EXTRALIBS)

checkFitSyst: checkFitSyst.cpp NanoUVCommon.o GunScan.o
	$(CC) -Wall $(INCLUDES) -o checkFitSyst checkFitSyst.cpp NanoUVCommon.o GunScan.o $(ROOTFLAG) $(EXTRALIBS)

checkSystFuncForm: checkSystFuncForm.cpp NanoUVCommon.o GunScan.o
	$(CC) -Wall $(INCLUDES) -o checkSystFuncForm checkSystFuncForm.cpp NanoUVCommon.o GunScan.o $(ROOTFLAG) $(EXTRALIBS)

checkCurrentSyst: checkCurrentSyst.cpp NanoUVCommon.o GunScan.o
	$(CC) -Wall $(INCLUDES) -o checkCurrentSyst checkCurrentSyst.cpp NanoUVCommon.o GunScan.o $(ROOTFLAG) $(EXTRALIBS)

drawAllSystCurrent: drawAllSystCurrent.cpp NanoUVCommon.o 
	$(CC) -Wall $(INCLUDES) -o drawAllSystCurrent drawAllSystCurrent.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawAllSystFuncForm: drawAllSystFuncForm.cpp NanoUVCommon.o 
	$(CC) -Wall $(INCLUDES) -o drawAllSystFuncForm drawAllSystFuncForm.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)



GunScan.o: ../../ElectronGunAnalysis/src/GunScan.cc NanoUVCommon.o
	$(CC) -Wall $(INCLUDES) -c ../../ElectronGunAnalysis/src/GunScan.cc NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)

NanoUVCommon.o: ../../CommonTools/src/NanoUVCommon.cc 
	$(CC) -Wall $(INCLUDES) -c ../../CommonTools/src/NanoUVCommon.cc $(ROOTFLAG) $(EXTRALIBS)



clean:
	rm *.o drawGain
