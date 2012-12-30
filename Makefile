SOURCES=builder2.cpp crawl99.cpp crawlfnc.cpp fn2.cpp fn3.cpp fn4.cpp invent.cpp monsstat.cpp port.cpp
OBJECTS=$(SOURCES:%.cpp=%.o)
CXXFLAGS=-w -fpermissive -ggdb3 -I/usr/include/ncursesw

.PHONY: all clean

all: crawl scores

crawl: $(OBJECTS)
	g++ $^ -lncursesw -o $@

scores:
	(for i in `seq 1 15` ; do echo -n '0       empty                                                                   ' ; done) > scores

%.o: %.cpp
	g++ -c $(CXXFLAGS) $< -o $@

port.o: CXXFLAGS=-Wall -ggdb3 -I/usr/include/ncursesw

clean:
	rm -f *.o crawl
