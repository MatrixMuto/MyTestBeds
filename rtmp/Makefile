CXX=clang++
CXXFLAGS=-std=c++11 -g -Ilibamf -Ilibamf/src

all:
	$(CXX) $(CXXFLAGS) main.cpp rtmp.cpp message.cpp -o test -Llibamf/src -lamf -lboost_system -lpthread

