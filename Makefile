SRC_DIR      := src
INC_DIR      := include
OBJ_DIR   	:= obj
BIN_DIR  	:= bin
RES_DIR      := res

TARGET := $(BIN_DIR)/gl-test
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXX := clang++
CPPFLAGS := -g -I$(INC_DIR) -MMD -MP
CXXFLAGS := -std=c++17 -Wall -Wextra 
LDFLAGS := 
LDLIBS := -framework OpenGL -lglew -lglfw


all: $(TARGET)
.PHONY: all

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	@echo "Linking the final executable"
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ 

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
.PHONY: clean

-include $(OBJECTS:.o=.d)
