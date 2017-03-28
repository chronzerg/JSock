libFile=jsock.a
testFile=test.out

libs=-lncurses
flags=-g -std=c++11 $(libs)

libSource=$(shell find "src" -iname "*.cpp")
libObjects=$(libSource:.cpp=.o)
testSource=$(shell find "test" -iname "*.cpp")
testObjects=$(testSource:.cpp=.o)

.PHONY: all jsock test

all: jsock

jsock: $(libFile)

test: $(testFile)

runTest: $(testFile)
	@echo "Starting test"
	@./$(testFile)

clean:
	@echo "Cleaning"
	@rm -f $(libFile)
	@rm -f $(testFile)
	@rm -f **/*.o

$(libFile): $(libObjects)
	@echo "Packaging library"
	@ar rcs $@ $^

$(testFile): $(testObjects) $(libFile)
	@echo "Linking test"
	@g++ $(flags) $^ -o $@

%.o: %.cpp
	@echo "Compiling $<"
	@g++ -c $(flags) $< -o $@
