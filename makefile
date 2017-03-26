libFile=raiiSocket.a
testFile=test.out

flags=-std=c++11

libSource=$(shell find "src" -iname "*.cpp")
libObjects=$(libSource:.cpp=.o)
testSource=$(shell find "test" -iname "*.cpp")

.PHONY: all raiiSocket test

all: raiiSocket

raiiSocket: $(libFile)

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

$(testFile): $(testSource) $(libFile)
	@echo "Linking test"
	@g++ $(flags) $^ -o $@

%.o: %.cpp
	@echo "Compiling $<"
	@g++ -c $(flags) $< -o $@
