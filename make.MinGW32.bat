del bin\OscUnitTests.exe
del bin\OscDump.exe
del bin\OscSendTests.exe
del bin\OscReceiveTest.exe
mkdir bin

g++ osc\OscUnitTests.cpp osc\OscTypes.cpp osc\OscReceivedElements.cpp osc\OscPrintReceivedElements.cpp osc\OscOutboundPacketStream.cpp -Wall -I..\ip -lws2_32 -o bin\OscUnitTests.exe

g++ osc\OscDump.cpp osc\OscTypes.cpp osc\OscReceivedElements.cpp osc\OscPrintReceivedElements.cpp ip\win32\NetworkingUtils.cpp ip\win32\UdpSocket.cpp -Wall -Iip -lws2_32 -lwinmm -o bin\OscDump.exe

g++ osc\OscSendTests.cpp osc\OscTypes.cpp osc\OscOutboundPacketStream.cpp ip\win32\NetworkingUtils.cpp ip\win32\UdpSocket.cpp ip\IpEndpointName.cpp -Wall -Iip -lws2_32 -lwinmm -o bin\OscSendTests.exe

g++ osc\OscReceiveTest.cpp osc\OscTypes.cpp osc\OscReceivedElements.cpp ip\win32\NetworkingUtils.cpp ip\win32\UdpSocket.cpp -Wall -Iip -lws2_32 -lwinmm -o bin\OscReceiveTest.exe

.\bin\OscUnitTests.exe