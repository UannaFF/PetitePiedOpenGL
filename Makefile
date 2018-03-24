
SRC_DIR := .
OBJ_DIR := build
SRC_FILES := $(shell find $(SRC_DIR) | grep cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lGL -lGLEW -lglfw
CPPFLAGS := -std=c++11


petit_pied: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^ 
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(shell dirname $@)
	g++ $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c -o $@ $<

clean:
	rm -rf build
    
