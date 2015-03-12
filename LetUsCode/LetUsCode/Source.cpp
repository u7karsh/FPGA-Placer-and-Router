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
	SA(graph, coordinates, portHashMap, PortPLDHashMap, 1000, 0.01, 1000);
	time_t end = time(NULL);

	print("\n\nPrinting final placed coordinates..\n\n");
	for (int i = 0; i < coordinates.size(); i++){
		printf("ID: %d (%d, %d, %d)\n", i, coordinates.at(i)[0], coordinates.at(i)[1], coordinates.at(i)[2]);
	}
	printf("\n\n Time elapsed: ");
	std::cout << end - begin << std::endl;
	while (1); //for debugging purpose only!
	return 0;
}