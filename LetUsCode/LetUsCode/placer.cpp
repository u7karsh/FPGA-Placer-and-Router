#include "placer.h"

int grid[XLIM][YLIM][INDEX];

///**************Functional definitions*********
void initCoordinates(std::vector<std::vector<int>> &coordinates, int count){
	//init the grid
	for (int i = 0; i < XLIM; i++){
		for (int j = 0; j < YLIM; j++){
			for (int k = 0; k < INDEX;k++)
				grid[i][j][k] = -1;
		}
	}
	//init coordinate master record
	coordinates.resize(count);
	for (int i = 0; i < count; i++)
		coordinates[i].resize(3);

	//init random coordinates
	for (int i = 0; i<count; i++){
		int x, y, z;
		do{
			x = rand() % XLIM;
			y = rand() % YLIM;
			z = rand() % INDEX;
		} while (grid[x][y][z] != -1);
		//set to true
		grid[x][y][z] = i;
		coordinates[i][0] = x;
		coordinates[i][1] = y;
		coordinates[i][2] = z;
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
	int deltaz = 0; //no cost in same PLD block
	return deltax + deltay + deltaz;
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

void swap(std::vector<std::vector<int>> &coordinates, int pos1[], int pos2[]){
	int id1 = grid[pos1[0]][pos1[1]][pos1[2]];
	int id2 = grid[pos2[0]][pos2[1]][pos2[2]];
	grid[pos1[0]][pos1[1]][pos1[2]] = -1;
	grid[pos2[0]][pos2[1]][pos2[2]] = -1;
	if (id1<0 && id2<0)
		return;
	else{
		if (id1 != -1){
			coordinates[id1][0] = pos2[0];
			coordinates[id1][1] = pos2[1];
			coordinates[id1][2] = pos2[2];
			grid[pos2[0]][pos2[1]][pos2[2]] = id1;
		}
		if (id2 != -1){
			coordinates[id2][0] = pos1[0];
			coordinates[id2][1] = pos1[1];
			coordinates[id2][2] = pos1[2];
			grid[pos1[0]][pos1[1]][pos1[2]] = id2;
		}
	}
}

void SA(std::vector<PLD> &PLD_array, std::vector<std::vector<int>> &coordinates, std::map<std::string, int> &portHashMap, std::multimap<int, int> &PortPLDHashMap, double temp, double coolingRate, int iterations){
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
			int Pos1[3], Pos2[3];
			while (1){
				//generate 1st random point
				Pos1[0] = (int)(rand() % XLIM);
				Pos1[1] = (int)(rand() % YLIM);
				Pos1[2] = (int)(rand() % INDEX);
				//generate 2nd random point
				Pos2[0] = (int)(rand() % XLIM);
				Pos2[1] = (int)(rand() % YLIM);
				Pos2[2] = (int)(rand() % INDEX);
				if (Pos1[0] != Pos2[0] || Pos1[1] != Pos2[1] || Pos1[2] != Pos2[2]){ //both coordinates to be swapped are not same!
					if (grid[Pos1[0]][Pos1[1]][Pos1[2]] > 0 || grid[Pos2[0]][Pos2[1]][Pos2[2]] > 0) //atleast one of the coordinate picked must have a device registered
						break;
				}
			}
			// Swap random places
			swap(coordinates, Pos1, Pos2);

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