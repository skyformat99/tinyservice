CC=gcc
CXX=g++
MAKE=make
TEMP:=gen-cpp
MODULES:=demo

CFLAGS=-rdynamic -std=c++11 -g -Wall -I.
CXXFLAGS=-rdynamic -std=c++11 -g -Wall -I.

LDFLAGS=-rdynamic -lthriftnb -lthrift -levent -lpthread -lrt -lstdc++

RM=-rm -rf

SRCS+=$(wildcard src/*.cpp)
SRCS+=$(filter-out %.skeleton.cpp,$(wildcard $(TEMP)/*.cpp))
SRCS+=$(wildcard $(addsuffix /*.cpp,$(MODULES)))
OBJS=$(SRCS:%.cpp=%.o)
DEPENDS=$(SRCS:%.cpp=%.d)


all:tinyservice

tinyservice:$(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS):%.o:%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

-include $(DEPENDS)

$(DEPENDS):%.d:%.cpp
	set -e; rm -f $@; \
	echo -n $(dir $<) > $@.$$$$; \
	$(CXX) -MM $(CXXFLAGS) $< >> $@.$$$$; \
	sed 's,\($*\)\.o[:]*,\1.o $@:,g' < $@.$$$$ > $@; \
	rm $@.$$$$

clean:
	$(RM) $(OBJS) $(DEPENDS) tinyservice

fake:
	echo $(OBJS)
