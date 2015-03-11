#include <string>
#include <iostream>
#include <stdio.h>
#include <map>
#include <string.h>
#include <utility>
#include <math.h>
#include <assert.h>
#include <vector>
#include "parser.h"

//lut space
#define XLIM 100
#define YLIM 100
#define ZLIM 8

void initCoordinates(std::vector<std::vector<int>> &, int );
double acceptanceProbability(int , int , double );
int getDeviceDistance(std::vector<std::vector<int>> &, int , int );
int getPlacementCost(std::vector<std::vector<int>> &, std::vector<PLD> &, std::multimap<int, int> &);
void swap(std::vector<std::vector<int>> &, int , int , int , int );
void SA(std::vector<PLD> &, std::map<std::string, int> &, std::multimap<int, int> &, double, double, int);