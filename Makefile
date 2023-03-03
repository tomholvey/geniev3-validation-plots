CPPFLAGS=-I $(BOOST_INC) \
         -I $(NUSIMDATA_INC) \
         -I $(ROOT_INC) \
	 -I $(GENIE_INC) \
         -I $(HEP_CONCURRENCY_INC) \

CXX=g++
CXXFLAGS=-std=c++17 -Wall -Werror -pedantic -g -I.

LDFLAGS=$(shell root-config --libs) \
	-lEG \

CXXROOTONLY=$(shell root-config --cxx)

CXXFLAGSROOTONLY=$(shell root-config --cflags)

LDFLAGSROOTONLY=$(shell root-config --libs)

all: bin/plot_kinematics bin/plot_kinematics_reweights

bin/plot_kinematics: src/plot_kinematics.cpp src/NuisTree.cpp src/filter.cpp src/distributions.cpp
	@echo Building $@
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ 

bin/plot_kinematics_reweights: src/plot_kinematics_reweights.cpp src/NuisTree.cpp src/filter.cpp src/distributions.cpp
	@echo Building $@
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ 
