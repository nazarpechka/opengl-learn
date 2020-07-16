CXX := clang++
CXXFLAGS := -g -std=c++17 -Wall -Wextra -Werror
LDFLAGS := -g
LDLIBS := -framework OpenGL -lglew -lglfw

SRC_DIR := ./src
BUILD_DIR := ./build
BIN_DIR := ./bin
TARGET := gl-test

SRC_EXT = cpp

INCLUDE := -Iinclude/
SOURCES := $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
OBJECTS := $(patsubst $(SRC_DIR)/%.$(SRC_EXT), $(BUILD_DIR)/%.o, $(SOURCES))


all: build $(BIN_DIR)/$(TARGET)
.PHONY: all

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@echo "Linking the final executable"
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

build:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
.PHONY: build

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
.PHONY: clean
