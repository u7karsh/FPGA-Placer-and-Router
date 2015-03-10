#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <map>
#include <string.h>
#include <utility>


//some definitions
#define WORD_LIMIT 20

//macros..
#define print(n) printf("%s\n", n); 

//*********DATA TYPES**********

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

//prototypes..
char* readNextLine(std::ifstream&);
void wordify(char*, int*, char[WORD_LIMIT][20]);
GraphNode addNode(char* type, int* input, int output, int ID);
int estimateNodes(char* fileName);
GraphNode* createGraph(char* fileName, int*);
void printGraph(GraphNode* node, int count);
void SA();

