PLATFORM := "LINUX"
SRC_DIR := ./
OBJ_DIR := ./
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

$(OBJ_FILES): %.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


UnitTest:  $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: all clean

all : UnitTest 

.DEFAULT_GOAL := all

clean:
	rm -f UnitTest UnitText.exe $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d

-include $(OBJ_FILES:.o=.d)
