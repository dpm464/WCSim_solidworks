#!/bin/sh


g++ -c -O2 -o WCSimRootGeom.o ../src/WCSimRootGeom.cc -fPIC `root-config --cflags` -I../include
g++ -c -O2 -o WCSimWCTrigger.o ../src/WCSimWCTrigger.cc -fPIC `root-config --cflags` -I../include I/opt/geant4.10.00.p03-install/include/Geant4/G4UImessenger.hh
#g++ -c -O2 -o libWCSimRoot.o
g++ -c -O2 -o WCSimRootEvent.o ../src/WCSimRootEvent.cc -fPIC `root-config --cflags` -I../include -I/opt/geant4.10.00.p03-install/include/Geant4
g++ -c -O2 -o timingrefC.o timingrefC.cpp -fPIC `root-config --cflags` 


g++ -o  timingrefC timingrefC.o   WCSimRootEvent.o  WCSimWCTrigger.o  WCSimRootGeom.o  `root-config --glibs` `root-config --cflags` -I../include
rm *.o


