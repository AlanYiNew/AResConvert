SRC_DIR := ./
OBJ_DIR := ./
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -L./dep/protobuf-3.21.5/
LIBS := -lprotoc\
    -lprotobuf\
	-lprotobuf-lite
CPPFLAGS :=
CXXFLAGS :=-std=c++17\
	-I./dep/protobuf-3.21.5/src\
	-I./thirdparty/include


arpc-gen: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f main *.o
