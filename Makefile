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


plot_kinematics: plot_kinematics.cpp NuisTree.cpp filter.cpp distributions.cpp
	@echo Building $@
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
