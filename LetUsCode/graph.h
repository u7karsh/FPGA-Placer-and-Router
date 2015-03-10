#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <map>
#define WORD_LIMIT 20

#define print(n) printf("%s\n", n); 

//ICE40 LUT
typedef struct LUT{
	int ID; //ID of this LUT.

	char initVal[16]; //contains LUT init vals
	int input[4]; //will contain port ID
	int output; //will contain port ID
}LUT;

typedef struct DFF{
	int ID; //ID of this DFF

	int clock; //port ID
	int data; //port ID
	int output; //port ID
}DFF;

typedef struct GraphNode{
	char type[5]; //info related to type like LUT or DFF

	int ID;

	int inPortCount; //number of input ports
	int* inPort;
	int outPort;

	//******For future purpose*********
	struct LUT lut; //LUT instance
	struct DFF dff; //dff instance
}GraphNode;

char* readNextLine(std::ifstream&);
char** wordify(char*, int*);
