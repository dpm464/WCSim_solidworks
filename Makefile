#**********************************************************************
# Author:		Matej Pavin
# Email			mpavin@triumf.ca
# Date:			02/16/2018
# Experiment:	Emphatic (T1396)
#
# Compile. Or do not compile. There is no try. - Yoda
#********************************************************************** 
INCDIR= $(shell pwd)/include
SRCDIR= $(shell pwd)/src
OBJDIR= $(shell pwd)/obj
BINDIR= $(shell pwd)/bin

VPATH = $(SRCDIR)

CXX=g++
CFLAGS=-c -O3 -Xclang -fopenmp -g -Wall `root-config --cflags` -I${INCDIR}
#CFLAGS=-c -O3 -Xclang -fopenmp -g -Wall `root-config --cflags` -I${INCDIR} -I/usr/local/gfortran/lib/gcc/x86_64-apple-darwin15/6.1.0/include
LDFLAGS=`root-config --glibs` -lHistPainter -lMinuit2  -lGenVector
#LDFLAGS=`root-config --glibs` -lHistPainter -lMinuit -lomp
#LDFLAGS=`root-config --glibs` -lHistPainter -lMinuit -lgomp


TARGET=timingrefC.cpp

EXECUTABLE=$(TARGET:%.cpp=%)


FILES= $(wildcard $(SRCDIR)/*.cpp)
SOURCES=$(FILES)

OBJECTS = $(FILES:$(SRCDIR)/%.cpp=${OBJDIR}/%.o)


OBJ=$(TARGET:%.cpp=${OBJDIR}/%.o) $(OBJECTS)

all: MESSAGE $(TARGET) $(SOURCES) $(EXECUTABLE)

MESSAGE:
	@echo '**********************************************************************'
	@echo '* Author:     Matej Pavin                                            *'
	@echo '* Email       mpavin@triumf.ca                                       *'
	@echo '* Date:       02/16/2018                                             *'
	@echo '* Experiment: Emphatic (T1396)                                       *'
	@echo '* Compile. Or do not compile. There is no try. - Yoda                *'
	@echo '**********************************************************************'	
	
$(EXECUTABLE): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)
	
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

print-%  : ; @echo $* = $($*)

clean:
	- $(RM) $(EXECUTABLE) $(OBJDIR)/*
