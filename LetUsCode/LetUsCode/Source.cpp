#include "placer.h"
#include <ctime>

int main(){
	//filename
	char fileName[] = "sb_up3down5.blif";
	//vars
	std::map<std::string, int> portHashMap;
	std::multimap<int, int> PortPLDHashMap; 
	std::vector<std::vector<int>> coordinates;

	//calls
	time_t begin = time(NULL);
	std::vector<PLD> graph = generatePLDs(fileName, portHashMap, PortPLDHashMap);
	printPLDs(graph);
	SA(graph, coordinates, portHashMap, PortPLDHashMap, 1000, 0.001, 100);
	time_t end = time(NULL);
	std::cout << end - begin << std::endl;
	while (1); //for debugging purpose only!
	return 0;
}