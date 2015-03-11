#include"parser.h"

int main(){
	char fileName[] = "sb_up3down5.blif";
	//Node graph generate
	std::vector<PLD> graph = generatePLDs(fileName);
	printPLDs(graph);

	while (1); //for debugging purpose only!
	return 0;
}