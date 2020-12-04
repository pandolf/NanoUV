CC = g++
CFLAGS = -Wall -c -g -fpermissive 

#ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
#ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I.. -I$(ROOTSYS)/include  -I../../CommonTools/ -I../../CommonTools/interface/ -I../interface/  #-I$(ROOFIT_INCLUDE)/ C-I$(CMSSW_BASE)/src -I$(CMSSW_BASE)/src/NanoUVCommonTools  

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -lHtml -lMathCore -lGenVector -lMinuit -lTreePlayer -lRooFitCore -lRooFit #-L$(ROOFIT_LIBDIR)/ 


measFileToTree: measFileToTree.cpp 
	$(CC) -Wall $(INCLUDES) -o measFileToTree measFileToTree.cpp $(ROOTFLAG) $(EXTRALIBS)

asciiFileToTree: asciiFileToTree.cpp 
	$(CC) -Wall $(INCLUDES) -o asciiFileToTree asciiFileToTree.cpp $(ROOTFLAG) $(EXTRALIBS)

drawPulseShape: drawPulseShape.cpp 
	$(CC) -Wall $(INCLUDES) -o drawPulseShape drawPulseShape.cpp $(ROOTFLAG) $(EXTRALIBS)


drawFe55: drawFe55.cpp NanoUVCommon.o
	$(CC) -Wall $(INCLUDES) -o drawFe55 drawFe55.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawLED: drawLED.cpp NanoUVCommon.o
	$(CC) -Wall $(INCLUDES) -o drawLED drawLED.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawTransparencyPMT: drawTransparencyPMT.cpp NanoUVCommon.o
	$(CC) -Wall $(INCLUDES) -o drawTransparencyPMT drawTransparencyPMT.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)

drawAPDvsL: drawAPDvsL.cpp NanoUVCommon.o
	$(CC) -Wall $(INCLUDES) -o drawAPDvsL drawAPDvsL.cpp NanoUVCommon.o $(ROOTFLAG) $(EXTRALIBS)


drawBaseline_vs_L: drawBaseline_vs_L.cpp NanoUVCommon.o HyperionData.o
	$(CC) -Wall $(INCLUDES) -o drawBaseline_vs_L drawBaseline_vs_L.cpp NanoUVCommon.o HyperionData.o $(ROOTFLAG) $(EXTRALIBS)

drawIapd_vs_CNThv: drawIapd_vs_CNThv.cpp NanoUVCommon.o HyperionData.o
	$(CC) -Wall $(INCLUDES) -o drawIapd_vs_CNThv drawIapd_vs_CNThv.cpp NanoUVCommon.o HyperionData.o $(ROOTFLAG) $(EXTRALIBS)

drawIapd_vs_CNThv_withLEDUV: drawIapd_vs_CNThv_withLEDUV.cpp NanoUVCommon.o HyperionData.o
	$(CC) -Wall $(INCLUDES) -o drawIapd_vs_CNThv_withLEDUV drawIapd_vs_CNThv_withLEDUV.cpp NanoUVCommon.o HyperionData.o $(ROOTFLAG) $(EXTRALIBS)

drawFieldEmissionSDD: drawFieldEmissionSDD.cpp NanoUVCommon.o HyperionData.o SDD.o
	$(CC) -Wall $(INCLUDES) -o drawFieldEmissionSDD drawFieldEmissionSDD.cpp NanoUVCommon.o HyperionData.o SDD.o $(ROOTFLAG) $(EXTRALIBS)

drawLEDvsFrequency: drawLEDvsFrequency.cpp NanoUVCommon.o HyperionData.o SDD.o
	$(CC) -Wall $(INCLUDES) -o drawLEDvsFrequency drawLEDvsFrequency.cpp NanoUVCommon.o HyperionData.o SDD.o $(ROOTFLAG) $(EXTRALIBS)



NanoUVCommon.o: ../../CommonTools/src/NanoUVCommon.cc 
	$(CC) -Wall $(INCLUDES) -c ../../CommonTools/src/NanoUVCommon.cc $(ROOTFLAG) $(EXTRALIBS)

HyperionData.o: ../src/HyperionData.cc 
	$(CC) -Wall $(INCLUDES) -c ../src/HyperionData.cc $(ROOTFLAG) $(EXTRALIBS)

SDD.o: ../src/SDD.cc 
	$(CC) -Wall $(INCLUDES) -c ../src/SDD.cc $(ROOTFLAG) $(EXTRALIBS)



clean:
	rm *.o APDGraph
