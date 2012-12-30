SOURCES=builder2.cpp crawl99.cpp crawlfnc.cpp fn2.cpp fn3.cpp fn4.cpp invent.cpp monsstat.cpp port.cpp
OBJECTS=$(SOURCES:%.cpp=%.o)
CXXFLAGS=-w -fpermissive -ggdb3 -I/usr/include/ncursesw

.PHONY: clean

crawl: $(OBJECTS)
	g++ $^ -lncursesw -o $@

%.o: %.cpp
	g++ -c $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o crawl
