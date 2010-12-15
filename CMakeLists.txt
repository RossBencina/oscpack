cmake_minimum_required(VERSION 2.6)
PROJECT(TestOSC)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})


IF(WIN32)
 set(SystemTypePath ip/win32)
ELSE(WIN32)
 set(SystemTypePath ip/posix)
ENDIF(WIN32)

ADD_LIBRARY(OSC 

ip/IpEndpointName.cpp 
${SystemTypePath}/NetworkingUtils.cpp
${SystemTypePath}/UdpSocket.cpp

osc/OscPrintReceivedElements.cpp
osc/OscReceivedElements.cpp 
osc/OscTypes.cpp 
osc/OscOutboundPacketStream.cpp

)

IF(WIN32)
 ADD_EXECUTABLE(OscDump examples/OscDump.cpp)
 TARGET_LINK_LIBRARIES(OscDump OSC Ws2_32 winmm)

 ADD_EXECUTABLE(SimpleReceive examples/SimpleReceive.cpp)
 TARGET_LINK_LIBRARIES(SimpleReceive OSC Ws2_32 winmm)

 ADD_EXECUTABLE(SimpleSend examples/SimpleSend.cpp)
 TARGET_LINK_LIBRARIES(SimpleSend OSC Ws2_32 winmm)
ELSE(WIN32)
 ADD_EXECUTABLE(OscDump examples/OscDump.cpp)
 TARGET_LINK_LIBRARIES(OscDump OSC)

 ADD_EXECUTABLE(SimpleReceive examples/SimpleReceive.cpp)
 TARGET_LINK_LIBRARIES(SimpleReceive OSC)

 ADD_EXECUTABLE(SimpleSend examples/SimpleSend.cpp)
 TARGET_LINK_LIBRARIES(SimpleSend OSC)
ENDIF(WIN32)
