TEST=OscUnitTests
SEND=OscSendTests
RECEIVE=OscReceiveTest
DUMP=OscDump

# should be either OSC_HOST_BIG_ENDIAN or OSC_HOST_LITTLE_ENDIAN
# Apple: OSC_HOST_BIG_ENDIAN
# Win32: OSC_HOST_LITTLE_ENDIAN
# i386 LinuX: OSC_HOST_LITTLE_ENDIAN

ENDIANESS=OSC_HOST_LITTLE_ENDIAN

SENDSOURCES = ./osc/OscSendTests.cpp ./osc/OscOutboundPacketStream.cpp ./osc/OscTypes.cpp ./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp ./ip/IpEndpointName.cpp
SENDOBJECTS = $(SENDSOURCES:.cpp=.o)

RECEIVESOURCES = ./osc/OscReceiveTest.cpp ./osc/OscTypes.cpp ./osc/OscReceivedElements.cpp ./osc/OscPrintReceivedElements.cpp ./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp
RECEIVEOBJECTS = $(RECEIVESOURCES:.cpp=.o)

DUMPSOURCES = ./osc/OscDump.cpp ./osc/OscTypes.cpp ./osc/OscReceivedElements.cpp ./osc/OscPrintReceivedElements.cpp ./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp
DUMPOBJECTS = $(DUMPSOURCES:.cpp=.o)

TESTSOURCES = ./osc/OscUnitTests.cpp ./osc/OscOutboundPacketStream.cpp ./osc/OscTypes.cpp ./osc/OscReceivedElements.cpp ./osc/OscPrintReceivedElements.cpp
TESTOBJECTS = $(TESTSOURCES:.cpp=.o)

INCLUDES = -I./ip -I./osc
COPTS  = -Wall -O3
CDEBUG = -Wall -g 
CXXFLAGS = $(COPTS) $(INCLUDES) -D$(ENDIANESS)
LIBS = -lpthread

all:	test send receive dump

test : $(TESTOBJECTS)
	@if [ ! -d bin ] ; then mkdir bin ; fi
	$(CXX) -o bin/$(TEST) $+ $(LIBS) 
send : $(SENDOBJECTS)
	@if [ ! -d bin ] ; then mkdir bin ; fi
	$(CXX) -o bin/$(SEND) $+ $(LIBS) 
receive : $(RECEIVEOBJECTS)
	@if [ ! -d bin ] ; then mkdir bin ; fi
	$(CXX) -o bin/$(RECEIVE) $+ $(LIBS) 
dump : $(DUMPOBJECTS)
	@if [ ! -d bin ] ; then mkdir bin ; fi
	$(CXX) -o bin/$(DUMP) $+ $(LIBS) 

clean:
	rm -rf bin $(TESTOBJECTS) $(SENDOBJECTS) $(RECEIVEOBJECTS) $(DUMPOBJECTS)

