#include "placer.h"

int grid[XLIM][YLIM];

///**************Functional definitions*********
void initCoordinates(std::vector<std::vector<int>> &coordinates, int count){
	//init the grid
	for (int i = 0; i < XLIM; i++){
		for (int j = 0; j < YLIM; j++){
			grid[i][j] = -1;
		}
	}
	//init coordinate master record
	coordinates.resize(count);
	for (int i = 0; i < count; i++)
		coordinates[i].resize(2);

	//init random coordinates
	for (int i = 0; i<count; i++){
		int x, y, z;
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

int getDeviceDistance(std::vector<std::vector<int>> &coor, int id1, int id2){
	int deltax = abs(coor[id1][0] - coor[id2][0]);
	int deltay = abs(coor[id1][1] - coor[id2][1]);
	return deltax + deltay;
}

int getPlacementCost(std::vector<std::vector<int>> &coordinates, std::vector<PLD> &PLD_array, std::multimap<int, int> &PortPLDHashMap){
	int cost = 0;
	for (int i = 0; i< coordinates.size(); i++){
		//LUT COST

		//4 input wire cost
		for (int j = 0; j < 4; j++){
			int tempPort = PLD_array.at(i).lut.input[j];
			//find devices having tempPort
			std::pair <std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
			ret = PortPLDHashMap.equal_range(tempPort);
			for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
				if (it->second < coordinates.size())
					cost = cost + getDeviceDistance(coordinates, it->second, i);
		}
		//1 output wire cost
		int tempPort = PLD_array.at(i).lut.output;
		//find devices having tempPort
		std::pair <std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
		ret = PortPLDHashMap.equal_range(tempPort);
		for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
			if (it->second < coordinates.size())
				cost = cost + getDeviceDistance(coordinates, it->second, i);

		//DFF COST
		if (PLD_array.at(i).dff.enabled){
			//clock wire cost
			int tempPort = PLD_array.at(i).dff.clock;
			//find devices having tempPort
			std::pair <std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
			ret = PortPLDHashMap.equal_range(tempPort);
			for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
				if (it->second < coordinates.size())
					cost = cost + getDeviceDistance(coordinates, it->second, i);
			//output wire cost
			tempPort = PLD_array.at(i).dff.output;
			//find devices having tempPort
			ret = PortPLDHashMap.equal_range(tempPort);
			for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
				if (it->second < coordinates.size())
					cost = cost + getDeviceDistance(coordinates, it->second, i);
			//input wire is free of cost
			cost = cost + 0;
		}

	}
	return cost / 2; //removing twice calculated costs
}

void swap(std::vector<std::vector<int>> &coordinates, int pos1x, int pos1y, int pos2x, int pos2y){
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

void SA(std::vector<PLD> &PLD_array, std::map<std::string, int> &portHashMap, std::multimap<int, int> &PortPLDHashMap, double temp, double coolingRate, int iterations){
	//init temperature, cooling rate, iterations per degree
	std::vector<std::vector<int>> coordinates;
	// Initialize intial solution
	initCoordinates(coordinates, PLD_array.size());

	// Init best placement
	std::vector<std::vector<int>> best(coordinates);

	int bestCost = getPlacementCost(best, PLD_array, PortPLDHashMap);
	printf("Initial Solution Cost: %d\n", bestCost);
	// Loop until system has cooled
	int iterationCount = (1 + ((log(1 / temp)) / log(1 - coolingRate))*iterations);
	printf("%d steps to perform\n", iterationCount);
	int counter = 0;
	int fraction = 0;
	while (temp > 1) {
		for (int iter = 0; iter < iterations; iter++){
			counter = (counter + 1) % (iterationCount / 10);
			if (counter == 0){
				++fraction;
				printf("%d%%...\n", fraction * 10);
			}
			
			//get a temporary vector
			std::vector<std::vector<int>> temporary(coordinates);

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
			swap(coordinates, Pos1x, Pos1y, Pos2x, Pos2y);

			// Get energy of solutions
			int E1 = getPlacementCost(temporary, PLD_array, PortPLDHashMap);
			int E2 = getPlacementCost(coordinates, PLD_array, PortPLDHashMap);

			// Decide if we should accept the neighbour
			if (!(acceptanceProbability(E1, E2, temp) > (rand() * 1.0) / RAND_MAX)) {
				coordinates = temporary; //undo the move
			}

			// Keep track of the best solution found
			if (E1 < bestCost) {
				best = temporary;
				bestCost = E1;
			}
		}//iterations ended
		// Cool system
		temp = temp*(1 - coolingRate);
	}
	printf("Final solution Cost: %d\n", bestCost);
}