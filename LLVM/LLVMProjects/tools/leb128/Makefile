LLVM_CONFIG := llvm-config

CXXFLAGS := -std=c++11 $(shell $(LLVM_CONFIG) --cxxflags) -Wall 
CXXFLAGS += -g -O0
LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --libs --system-libs)
CXX := clang++

SOURCE_FILES := $(wildcard *.cpp)
PROGRAM := leb128

%.o: %.cpp $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(PROGRAM): $(SOURCE_FILES:%.cpp=%.o)
	$(CXX) -o $(PROGRAM) $^ $(CXXFLAGS) $(LDFLAGS)

clean:
	@rm -rf *.o $(PROGRAM)
