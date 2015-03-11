#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <map>
#include <string.h>
#include <utility>
#include <math.h>
#include <assert.h>
#include <vector>


//some definitions
//lut space..
#define XLIM 100
#define YLIM 100

//*********DATA TYPES**********

//ICE40 LUT
typedef struct LUT{
	char initVal[17]; //contains LUT init vals. one extra to add newline in future..
	int input[4]; //will contain port ID
	int output; //will contain port ID
}LUT;

typedef struct DFF{
	int clock; //port ID
	int data; //port ID
	int output; //port ID
	bool enabled; //if dff enabled
}DFF;

typedef struct PLD{
	struct LUT lut; //LUT instance
	struct DFF dff; //dff instance
}PLD;

//prototypes..
int estimateLUT(char*);
void wordify(char* line, std::vector<std::string> &);
char* readNextLine(std::ifstream&);
PLD* generatePLDs(char*, int*);
void printPLDs(PLD*, int);

//macros..
#define print(n) printf("%s\n", n); 