#include "parser.h"

//lut space
#define XLIM 30
#define YLIM 32
#define INDEX 8

//prototypes..
void initCoordinates(std::vector<std::vector<int>> &, int );
double acceptanceProbability(int , int , double );
int getDeviceDistance(std::vector<std::vector<int>> &, int , int );
int getPlacementCost(std::vector<std::vector<int>> &, std::vector<PLD> &, std::multimap<int, int> &);
void swap(std::vector<std::vector<int>> &, int[] , int[]);
void SA(std::vector<PLD> &, std::vector<std::vector<int>> &, std::map<std::string, int> &, std::multimap<int, int> &, double, double, int);
