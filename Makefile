CXX=g++

SOURCES = memory.cpp main.cpp core.cpp intelhexclass.cpp
BINARY = prosthetic

CXXFLAGS = -Wall -Werror -std=c++11 -g -O0

all: $(BINARY)

OBJS = $(SOURCES:%.cpp=%.o)

$(BINARY): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^

clean:
	$(RM) $(OBJS) $(BINARY)
