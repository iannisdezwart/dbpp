SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

TEST_SRC_DIR = ./src/test
TEST_SRC_FILES = $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_OBJ_FILES = $(patsubst $(TEST_SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(TEST_SRC_FILES))

CXX_FLAGS = -std=c++20 -g -O3

all: bin/test bin/main

bin/main: dirs $(OBJ_FILES)
	$(CXX) $(LD_FLAGS) $(OBJ_FILES) -o $@

.PHONY: dirs
dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

.PHONY: test
bin/test: dirs $(TEST_OBJ_FILES)
	$(CXX) $(LD_FLAGS) $(TEST_OBJ_FILES) -o $@
	bin/test

$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)