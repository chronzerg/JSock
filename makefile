libFile=jsock.a

libs=-lncurses
flags=-g -std=c++11 $(libs)

libSource=$(shell find "src" -iname "*.cpp")
libObjects=$(libSource:.cpp=.o)
demoSources=$(shell find "demos" -iname "*.cpp")
demoExecs=$(addsuffix .out, \
	$(notdir $(basename $(demoSources))))

.PHONY: all jsock test

all: jsock

jsock: $(libFile)

demos: $(demoExecs)

clean:
	@echo "Cleaning"
	@rm -f $(libFile)
	@rm -f *.out
	@rm -f **/*.o

$(libFile): $(libObjects)
	@echo "Packaging library"
	@ar rcs $@ $^

%.out: demos/%.cpp $(libFile)
	@echo "Building $(basename $@)"
	@g++ $(flags) $^ -o $@

%.o: %.cpp
	@echo "Compiling $<"
	@g++ -c $(flags) $< -o $@

