#include "Test.h"
#include <cstdio>


int printLib() {
    for (int i = 0; i < sizeof(g_libBuff)/sizeof(char); ++i) {
       printf("%d ", g_libBuff[i]); 
    }
    return 0;
};
