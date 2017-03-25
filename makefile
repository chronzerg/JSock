libFile=raiiSocket.a
testFile=test.out

flags=-g -std=c++11

libSource=$(shell find "src" -iname "*.cpp")
libObjects=$(libSource:.cpp=.o)
testSource=$(shell find "test" -iname "*.cpp")

.PHONY: all raiiSocket test

all: raiiSocket

raiiSocket: $(libFile)

test: $(testFile)

runTest: $(testFile)
	./$(testFile)

clean:
	@rm -f $(libFile)
	@rm -f $(testFile)
	@rm -f **/*.o

$(libFile): $(libObjects)
	@ar rs $@ $^

$(testFile): $(testSource) $(libFile)
	g++ $(flags) $^ -o $@

%.o: %.cpp
	g++ -c $(flags) $< -o $@
