CXX = g++
CXXFLAGS = -std=c++20 -g -Iinclude -Wall -Wextra -I../spdlog/include -L../spdlog/build -lspdlog -pthread

# Sources
SRC_ALL := $(shell find src -name "*.cpp")
SRC := $(filter-out %/main.cpp, $(SRC_ALL))
TEST_SRC := $(shell find tests -name "*.cpp")
CATCH_CPP := include/catch2.cpp

# Executables
PROGRAM = program.out
TEST_EXE = test_runner.out


all: $(PROGRAM)
$(PROGRAM): src/main.cpp $(SRC)
	$(CXX) $(CXXFLAGS) src/main.cpp $(SRC) -o $@

test: $(TEST_EXE)
$(TEST_EXE): $(SRC) $(TEST_SRC) $(CATCH_CPP)
	$(CXX) $(CXXFLAGS) -Isrc $(SRC) $(TEST_SRC) $(CATCH_CPP) -o $@

run_tests: test
	./$(TEST_EXE)

clean:
	rm -f $(PROGRAM) $(TEST_EXE)
