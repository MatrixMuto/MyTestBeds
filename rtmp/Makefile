CXX=clang++ 
CXXFLAGS= -g --std=c++11 -Ilibamf -Ilibamf/src

all: main.o rtmp.o message.o
	$(CXX) $(CXXFLAGS)  $^ -o test -Llibamf/src -lamf -lboost_regex -lboost_system -lpthread

clean:
	rm *.o test

