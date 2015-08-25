**News:** Oscpack 1.1.0 is now available from the [Downloads page](https://code.google.com/p/oscpack/downloads/list). Read the [CHANGES](https://code.google.com/p/oscpack/source/browse/trunk/CHANGES) file for more info.

---

Oscpack is simply a set of C++ classes for packing and unpacking OSC packets. Oscpack includes a minimal set of UDP networking classes for Windows and POSIX. The networking classes are sufficient for writing many OSC applications and servers, but you are encouraged to use another networking framework if it better suits your needs. Oscpack is not an OSC application framework. It doesn't include infrastructure for constructing or routing OSC namespaces, just classes for easily constructing, sending, receiving and parsing OSC packets. The library should also be easy to use for other transport methods (e.g. serial).

The key goals of the oscpack library are:

  * Be a simple and complete implementation of OSC
  * Be portable to a wide variety of platforms
  * Allow easy development of robust OSC applications (for example it should be impossible to crash a server by sending it malformed packets, and difficult to create malformed packets.)

For a taste of oscpack take a look at the following examples: [SimpleSend.cpp](https://code.google.com/p/oscpack/source/browse/trunk/examples/SimpleSend.cpp) and [SimpleReceive.cpp](https://code.google.com/p/oscpack/source/browse/trunk/examples/SimpleReceive.cpp).