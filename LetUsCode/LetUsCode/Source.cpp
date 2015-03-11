#include "placer.h"

int main(){
	char fileName[] = "sb_up3down5.blif";
	std::map<std::string, int> portHashMap;
	std::multimap<int, int> PortPLDHashMap;
	std::vector<PLD> graph = generatePLDs(fileName, portHashMap, PortPLDHashMap);
	printPLDs(graph);
	SA(graph, portHashMap, PortPLDHashMap, 1000, 0.1, 10);

	while (1); //for debugging purpose only!
	return 0;
}