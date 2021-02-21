LIBS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
CXX := g++ #allows us to change compiler
BIN=./bin/
OBJ_DIR := build/
SRC_FILES := $(wildcard *.cpp)
OBJ_FILES :=  $(patsubst %.cpp,$(OBJ_DIR)%.o,$(SRC_FILES))
LDFLAGS :=
CPPFLAGS :=
CXXFLAGS := -g

build/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build/%.o: %.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

all: culturesim

culturesim: $(OBJ_FILES)
	@echo "** Linking object files in build directory to create game..."
	$(CXX) $(LDFLAGS) -o $(BIN)$@ $^ $(LIBS)

clean:
	@echo "** Removing object files and executable..."
	rm -f $(BIN)culturesim build/*.o

cleanobj:
	@echo "** Removing object files..."
	rm -f build/*.o

cleangame:
	@echo "** Removing executable.."
	rm -f $(BIN)culturesim