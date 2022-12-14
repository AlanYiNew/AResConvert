PLATFORM := "LINUX"
SRC_DIR := ./
OBJ_DIR := ./BUILD
PROTOCOL_DIR := ./protocol
PROTO_FILES := $(wildcard $(SRC_DIR)/*.proto)
PROTO_SRCS := $(patsubst $(SRC_DIR)/%.proto, $(SRC_DIR)/%.pb.cc,$(PROTO_FILES))
PROTO_OBJ_FILES := $(patsubst $(SRC_DIR)/%.pb.cc,$(OBJ_DIR)/%.pb.o,$(PROTO_SRCS))
PROTOBUF_DIR = ./dep/protobuf-3.21.5/

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) 
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -O2 -g\
        -L./thirdparty/WebView2/x64/\
        -L./thirdparty/OpenXLSX/output/windows\

WINDOWS_LIBS := -lole32 -lshell32 -lshlwapi -luser32 -lws2_32 -lWebView2Loader.dll -lOpenXLSX
LINUX_LIBS := $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)

LIBS := -lprotoc\
    -lprotobuf\
	-lprotobuf-lite\
    -lpthread\

ifeq ($(PLATFORM),"LINUX")
 	LIBS += $(LINUX_LIBS)
 	CC := g++
    LDFLAGS += -L$(PROTOBUF_DIR)
else
	LIBS += $(WINDOWS_LIBS)
 	CC := x86_64-w64-mingw32-g++ 
    LDFLAGS += -L$(PROTOBUF_DIR)/windows
endif
    
CPPFLAGS := -MD -g -O2 
CXXFLAGS :=-std=c++17\
	-I./thirdparty/include\
	-I$(PROTOBUF_DIR)/src\
    -I./thirdparty/WebView2/include\
    -I./thirdparty/OpenXLSX/OpenXLSX

$(PROTO_SRCS): $(PROTO_FILES)
	./dep/protobuf-3.21.5/protoc --cpp_out=. -I. -I$(PROTOBUF_DIR) -I$(PROTOCOL_DIR) $(PROTO_FILES)

$(PROTO_OBJ_FILES):$(OBJ_DIR)/%.pb.o: $(SRC_DIR)/%.pb.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJ_FILES): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR):
	@mkdir -p $@

AResConvert: $(PROTO_OBJ_FILES) $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

#include $(OBJ_FILES:.o=.d)

.PHONY: all clean

all : $(OBJ_DIR) AResConvert 

.DEFAULT_GOAL := all

clean:
	rm -f AResconvert $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d
	rm -f *.pb.cc *.pb.h

