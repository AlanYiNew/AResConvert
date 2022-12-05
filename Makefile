SRC_DIR := ./
OBJ_DIR := ./
PROTOBUF_DIR := ./dep/protobuf-3.21.5/src 
PROTO_FILES := $(wildcard $(SRC_DIR)/*.proto)
PROTO_SRCS := $(patsubst $(SRC_DIR)/%.proto, $(SRC_DIR)/%.pb.cc,$(PROTO_FILES))
PROTO_OBJ_FILES := $(patsubst $(SRC_DIR)/%.pb.cc,$(OBJ_DIR)/%.pb.o,$(PROTO_SRCS))

CC := x86_64-w64-mingw32-g++ 
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) 
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -L./dep/protobuf-3.21.5/\
        -L./thirdparty/WebView2/x64/\
        -L./thirdparty/OpenXLSX/output/
LIBS := -lprotoc\
    -lprotobuf\
	-lprotobuf-lite\
    -lWebView2Loader.dll\
    -lOpenXLSX\
    -lole32 -lshell32 -lshlwapi -luser32
CPPFLAGS := -MD 
CXXFLAGS :=-std=c++17\
	-I./dep/protobuf-3.21.5/src\
	-I./thirdparty/include\
    -I./thirdparty/WebView2/include\
    -I./thirdparty/OpenXLSX/OpenXLSX\
	-I$(PROTOBUF_DIR)

$(PROTO_SRCS): $(PROTO_FILES)
	./dep/protobuf-3.21.5/protoc --cpp_out=. -I. -I$(PROTOBUF_DIR) $(PROTO_FILES)

$(PROTO_OBJ_FILES):%.pb.o: $(SRC_DIR)/%.pb.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJ_FILES): %.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


AResConvert:  $(PROTO_OBJ_FILES) $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: all clean

all : AResConvert 

.DEFAULT_GOAL := all

clean:
	rm -f AResconvert $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d

-include $(OBJ_FILES:.o=.d)
