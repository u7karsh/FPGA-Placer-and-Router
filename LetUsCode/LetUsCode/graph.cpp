#include "graph.h"
#include <math.h>


#define XLIM 200
#define YLIM 200

int **coordinates;
int grid[XLIM][YLIM];

//hashmaps..
std::map<std::string, int> portHashMap;
std::multimap<int, int> PortDeviceHashMap;


int DEVICE_COUNT = 20;
GraphNode* graph;

///**************Functional definitions..

GraphNode addNode(char* type, int* input, int output, int ID){
	GraphNode node;
	if (!strcmp(type, "DFF") || !strcmp(type, "LUT")){
		strcpy(node.type, type);
		node.ID = ID;
		if (!strcmp(type, "LUT")) node.inPortCount = 4;
		else node.inPortCount = 2;
		node.inPort = (int*)malloc(sizeof(int)*node.inPortCount);
		for (int i = 0; i<node.inPortCount; i++) node.inPort[i] = input[i];
		node.outPort = output;

		return node;
	}
	else{
		print("Error! neither DFF nor LUT");
		return node;
	}
}

int estimateNodes(char* fileName){
	int count = 0;
	std::ifstream fp(fileName);
	char* line;
	if (fp.is_open()){
		while (fp.good()){
			line = readNextLine(fp);
			//parse all white space..
			while (*line == ' ') line++;
			if (*line == '.'){
				line++;
				int wordCnt = 0;
				char words[WORD_LIMIT][20];
				wordify(line, &wordCnt, words);
				if (!strcmp(words[0], "gate"))
					count++;
			}
			else continue;
		}
		fp.close();
		return count;
	}
	else return -1;
}

void wordify(char* line, int* count, char words[WORD_LIMIT][20]){
	char* pch;
	*count = 0;
	pch = strtok(line, " ");
	while (pch != NULL && *count < WORD_LIMIT)
	{
		strcpy(words[(*count)++], pch);
		pch = strtok(NULL, " ");
	}
}

char* readNextLine(std::ifstream& fp){
	std::string line;
	if (fp.is_open() && fp.good())
	{
		getline(fp, line);
		char *cstr = new char[line.length() + 1];
		strcpy(cstr, line.c_str());
		return cstr;
	}
	else return NULL;
}

GraphNode* createGraph(char* fileName, int* cnt){
	//node list generation

	//node count estimation
	print("Initial node count guessing..");
	int NodeCnt = estimateNodes(fileName);
	*cnt = NodeCnt;
	printf("%d ", NodeCnt);
	print("Nodes found!");

	int deviceID = 0;
	int portID = 0;
	//list generation

	print("Generating graph..");
	//init
	GraphNode* nodes = (GraphNode*)(malloc(sizeof(GraphNode)*NodeCnt));

	//open file
	std::ifstream fp(fileName);
	char* line;

	if (fp.is_open()){
		while (fp.good()){
			line = readNextLine(fp);
			//parse all white space..
			while (*line == ' ') line++;
			if (*line == '.'){
				line++;
				int wordCnt = 0;
				char words[WORD_LIMIT][20];
				wordify(line, &wordCnt, words);
				if (!strcmp(words[0], "gate"))

					//type of gate
					if (!strcmp(words[1], "SB_LUT4")){
						char type[] = "LUT";
						int input[4];
						int output;
						//inout ports..
						for (int i = 0; i < 5; i++){
							int offset;
							if (i == 4)
								offset = 2;
							else
								offset = 3;

							//add port to hash map if not added
							if (portHashMap.find(words[i + 2] + offset) == portHashMap.end()){
								portHashMap[words[i + 2] + offset] = portID++;
							}

							//add this port id to device hash map
							PortDeviceHashMap.insert(std::make_pair(deviceID, portHashMap[words[i + 2] + offset]));
							if (i < 4)
								input[i] = portHashMap[words[i + 2] + offset];
							else
								output = portHashMap[words[i + 2] + offset];
						}
						nodes[deviceID] = addNode(type, input, output, deviceID);
						deviceID++;

					}
					else if (!strcmp(words[1], "SB_DFF")){
						/*char type[] = "DFF";
						int input[2];
						int output;
						//inout ports..
						for (int i = 0; i < 3; i++){
							int offset = 2;

							//add port to hash map if not added
							if (portHashMap.find(words[i + 2] + offset) == portHashMap.end()){
								portHashMap[words[i + 2] + offset] = portID++;
							}

							//add this port id to device hash map
							devicePortHashMap.insert(std::make_pair(portHashMap[words[i + 2] + offset], deviceID));
							if (i < 2)
								input[i] = portHashMap[words[i + 2] + offset];
							else
								output = portHashMap[words[i + 2] + offset];
						}
						nodes[deviceID] = addNode(type, input, output, deviceID);
						deviceID++;*/
						continue;
					}
					else{
						print(line);
						print("Warning: Missing LUT or DFF");
						continue;
					}
			}
			else continue;
		}
		fp.close();
		print("Graph generated!");
		*cnt = deviceID;
		return nodes;
	}
	else{
		print("File Error!");
		return NULL;
	}
}

void printGraph(GraphNode* node, int count){

	print("****printing graph******");
	for (int i = 0; i < count; i++){
		if (!strcmp(node[i].type, "LUT"))
			printf("%s[%d] %d %d %d %d  => %d\n", node[i].type, node[i].ID, node[i].inPort[0], node[i].inPort[1], node[i].inPort[2], node[i].inPort[3], node[i].outPort);
		else
			printf("%s[%d] %d %d  => %d\n", node[i].type, node[i].ID, node[i].inPort[0], node[i].inPort[1], node[i].outPort);
	}

	/*print("***Printing hash maps****");
	print(">>Port hash map");
	print(">>Device port hash map");*/
}

void initCoordinates(){
	//init the grid
	for (int i = 0; i < XLIM; i++){
		for (int j = 0; j < YLIM; j++){
			grid[i][j] = -1;
		}
	}

	coordinates = new int*[DEVICE_COUNT];
	for (int i = 0; i < DEVICE_COUNT; i++)
		coordinates[i] = new int[2];

	//init random coordinates
	for (int i = 0; i<DEVICE_COUNT; i++){
		int x, y;
		do{
			x = rand() % XLIM;
			y = rand() % YLIM;
		} while (grid[x][y] != -1);
		//set to true
		grid[x][y] = i;
		coordinates[i][0] = x;
		coordinates[i][1] = y;
	}
}

double acceptanceProbability(int energy, int newEnergy, double temperature){
	if (newEnergy < energy)
		return 1.0;
	return exp((energy - newEnergy) / temperature);
}

int getDeviceDistance(int id1, int id2){
	int deltax = abs(coordinates[id1][0] - coordinates[id2][0]);
	int deltay = abs(coordinates[id1][1] - coordinates[id2][1]);
	return deltax + deltay;
}

int getPlacementCost(int** temp){
	int minx = XLIM, miny = YLIM, maxx = 0, maxy = 0;
	int cost = 0;
	for (int i = 0; i<DEVICE_COUNT; i++){
		if (coordinates[i][0] < minx)
			minx = coordinates[i][0];
		if (coordinates[i][1] < miny)
			miny = coordinates[i][1];

		if (coordinates[i][0] > maxx)
			maxx = coordinates[i][0];
		if (coordinates[i][0] > maxy)
			maxy = coordinates[i][1];
		if (!strcmp(graph[i].type, "LUT")){
			for (int j = 0; j < 4; j++){
				int tempPort = graph[i].inPort[j];
				//find device of tempPort
				std::pair <std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
				ret = PortDeviceHashMap.equal_range(tempPort);
				for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
					if (it->second < DEVICE_COUNT)
						cost = cost + getDeviceDistance(it->second, i);
			}
		}
	}
	return cost;
	//return abs(maxy - miny) + abs(maxx - minx); //semi parameter
}

void copyContents(int** best, int** coordinates){
	for (int i = 0; i<DEVICE_COUNT; i++){
		best[i][0] = coordinates[i][0];
		best[i][1] = coordinates[i][1];
	}
}

void swap(int pos1x, int pos1y, int pos2x, int pos2y){
	int id1 = grid[pos1x][pos1y];
	int id2 = grid[pos2x][pos2y];
	grid[pos1x][pos1y] = -1;
	grid[pos2x][pos2y] = -1;
	if (id1<0 && id2<0)
		return;
	else{

		if (id1 != -1){
			coordinates[id1][0] = pos2x;
			coordinates[id1][1] = pos2y;
			grid[pos2x][pos2y] = id1;
		}
		if (id2 != -1){
			coordinates[id2][0] = pos1x;
			coordinates[id2][1] = pos1y;
			grid[pos1x][pos1y] = id2;
		}
	}
}

void SA(){
	//init temperature
	double temp = 10000;
	// Cooling rate
	double coolingRate = 0.001;
	//iterations per degree temperature
	int iterations = 1000;

	// Initialize intial solution
	initCoordinates();

	// Set as current
	int** best;
	best = new int*[DEVICE_COUNT];
	for (int i = 0; i < DEVICE_COUNT; i++)
		best[i] = new int[2];

	copyContents(best, coordinates);

	int bestCost = getPlacementCost(best);
	printf("Initial Solution Cost: %d\n", bestCost);
	// Loop until system has cooled
	int iterationCount = (1 + ((log(1 / temp)) / log(1 - coolingRate))*iterations);
	int counter = 0;
	int fraction = 0;
	while (temp > 1) {
		for (int iter = 0; iter < iterations; iter++){
			counter = (counter + 1) % (iterationCount / 10);
			if (counter == 0){
				++fraction;
				printf("%d%%...\n", fraction * 10);
			}
			int** temporary;
			temporary = new int*[DEVICE_COUNT];
			for (int i = 0; i < DEVICE_COUNT; i++)
				temporary[i] = new int[2];
			copyContents(temporary, coordinates);

			// Get 2 random positions in the grid
			int Pos1x, Pos1y, Pos2x, Pos2y;
			while (1){
				Pos1x = (int)(rand() % XLIM);
				Pos1y = (int)(rand() % YLIM);
				Pos2x = (int)(rand() % XLIM);
				Pos2y = (int)(rand() % YLIM);
				if (Pos1x != Pos2x || Pos1y != Pos2y){
					if (grid[Pos1x][Pos1y] > 0 || grid[Pos2x][Pos2y] > 0)
						break;
				}
			}
			// Swap random places
			swap(Pos1x, Pos1y, Pos2x, Pos2y);

			// Get energy of solutions
			int E1 = getPlacementCost(temporary);
			int E2 = getPlacementCost(coordinates);

			// Decide if we should accept the neighbour
			if (!(acceptanceProbability(E1, E2, temp) > (rand() * 1.0) / RAND_MAX)) {
				copyContents(coordinates, temporary); //undo the move
			}

			// Keep track of the best solution found
			if (E1 < bestCost) {
				copyContents(best, temporary);
				bestCost = E1;
			}
		}//iterations ended
		// Cool system
		temp = temp*(1 - coolingRate);
	}
	printf("Final solution Cost: %d\n", bestCost);
}


int main(){
	char fileName[] = "sb_up3down5.blif";

	//Node graph generate
	graph = createGraph(fileName, &DEVICE_COUNT);
	printf("%d LUT's found\n", DEVICE_COUNT);
	printGraph(graph, DEVICE_COUNT);
	//apply placement by simulated annealing
	print("");
	print("*******SIMULATED ANNEALING PLACER********");
	SA();

	while (1); //for debugging purpose only!
	return 0;
}