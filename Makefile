#Root libraries
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)
ROOTFLAGS			= $(shell root-config --cflags)

#Variables and options
OPT           := -O2
#Compliler of choice
CXX           := g++
CXXFLAGS      := -Wall $(ROOTFLAGS) -pthread $(OPT) -fPIC -I$(ROOTSYS)/include
LD            := g++
LDFLAGS       := $(OPT) -pthread
LIBS         	= $(ROOTGLIBS) -L/usr/X11R6/lib -lX11 -lXpm


#include all header files for linking here
HDRS := Diffuse.hh

#Objects files for each class
#This will automatically create an object file for each header file "Header.hh" called "Header.o" in the list "HDRS"
OBJS := $(HDRS:.hh=.o)
#Source files for each class, here we also create an array of the source files corresponding to the headers
#This helps us reduce the number of places to input variables and reduce the chance of typos causing issues
SRCS := $(HDRS:.hh=.cxx)

#Since we dont create a file called "all" or "clean" with our rules, we declare them as phony targets
#A good description for this is here : https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean

#Having a function called "all" ensures that all of our rules will be ran when we call "make"
#otherwise only the first rule will be called
#Call "make clean" to remove clean the directory and start compilation fresh.
all: Radonify

#Main Radon program
Radonify: Diffuse.o
	@echo "Compiling Radon program.."
	$(LD) $(LDFLAGS) -o $@ $< $(LIBS) -ltree

#This rule helps us build our individual machine-readable object files for header files listed in HDRS
%.o: %.cxx
	$(CXX) -c $(CXXFLAGS) $<


clean:
	@echo "Cleaning up..."
	rm -f *.o
	rm -f *~
	rm -f Radonify
