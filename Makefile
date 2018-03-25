
SRC_DIR := .
OBJ_DIR := build
SRC_FILES := $(shell find $(SRC_DIR) | grep cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lGL -lGLEW -lglfw
CPPFLAGS := -std=c++11 -g


petit_pied: $(OBJ_FILES)
	g++ $(CPPFLAGS) -o $@ $^ $(LDFLAGS)
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(shell dirname $@)
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

clean:
	rm -rf build petit_pied
    
