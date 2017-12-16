
#include <stdio.h> 

int parseRLE(char* input,char* output ,int padding, int sizeX); //Every line = padding , translation , padding 

void parseXY(char* input, int*x,int*y);

void loadFile(char* file, char** output1, char** output2, int* sizeX, int* mySizeY);

void parseNext(FILE* f, char* buffer ,int * size);
