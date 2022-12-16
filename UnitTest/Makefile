PLATFORM := "LINUX"
SRC_DIR := ./
OBJ_DIR := ./
LIB_SRC_DIR := ..
LIB_SRC_FILES := $(LIB_SRC_DIR)/AMeta.cpp $(LIB_SRC_DIR)/md5.cpp
LIB_OBJ := $(patsubst $(LIB_SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,$(LIB_SRC_FILES))
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)  
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

ifeq ($(PLATFORM),"LINUX")
 	CC := g++
else
 	CC := x86_64-w64-mingw32-g++ 
endif
    
CPPFLAGS := -MD -g 
CXXFLAGS :=-std=c++17\
	-I../protocol\
    -I../

LDFLAGS := -L../dep/protobuf-3.21.5/
LIBS := -lprotobuf -lprotoc

$(OBJ_FILES): %.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(LIB_OBJ): %.o: $(LIB_SRC_DIR)/%.cpp
	@echo $@	
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

UnitTest:  $(OBJ_FILES) $(LIB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: all clean

all : UnitTest 

.DEFAULT_GOAL := all

clean:
	rm -f UnitTest UnitText.exe $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d

-include $(OBJ_FILES:.o=.d)
