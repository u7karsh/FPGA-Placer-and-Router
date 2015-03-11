#include "parser.h"

//hashmaps..
std::map<std::string, int> portHashMap;
std::multimap<int, int> PortPLDHashMap;


///**************Functional definitions**************
void wordify(char* line, std::vector<std::string> &words){
	char* pch;
	int count = 0;
	pch = strtok(line, " ");
	while (pch != NULL)
	{
		words.push_back(pch);
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

std::vector<PLD> generatePLDs(char* fileName){
	//init PLD and port ID
	int PLDID = 0;
	int portID = 0;

	//list generation
	print("Generating graph..");
	//init
	std::vector<PLD> PLD_array;

	PLD PLD_temp;

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
				std::vector<std::string> words;
				wordify(line, words);
				if (!words.at(0).compare("gate")){
					//type of gate
					if (!words.at(1).compare("SB_LUT4")){ //got an LUT		
						//Register 4 Input ports
						for (int i = 0; i < 4; i++){
							//register port to an address in hash map if not added
							if (portHashMap.find(words.at(i + 2).c_str() + 3) == portHashMap.end()){
								portHashMap[words.at(i + 2).c_str() + 3] = portID++;
							}
							//register this port id to PLD hash map
							PortPLDHashMap.insert(std::make_pair(PLDID, portHashMap[words.at(i + 2).c_str() + 3]));

							//add the port address to PLD
							PLD_temp.lut.input[i] = portHashMap[words.at(i + 2).c_str() + 3];
						}

						//Register 1 Output port
						//register port to an address in hash map if not added
						if (portHashMap.find(words.at(6).c_str() + 2) == portHashMap.end()){
							portHashMap[words.at(6).c_str() + 2] = portID++;
						}
						//register this port id to PLD hash map
						PortPLDHashMap.insert(std::make_pair(portHashMap[words.at(6).c_str() + 2], PLDID));

						//add the port address to PLD
						PLD_temp.lut.output = portHashMap[words.at(6).c_str() + 2];

						//element to be touched in future
						PLD_temp.dff.enabled = false; //disable
						//increment PLD ID
						PLDID++;
						//push back PLD temp to PLD vector
						PLD_array.push_back(PLD_temp);

					}
					else if (!words.at(1).compare("SB_DFF")){ //got a DFF
						//check for a registerd LUT associated with this FF
						int pld_id = -1;
						std::pair <std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
						ret = PortPLDHashMap.equal_range(portHashMap[words.at(3).c_str() + 2]);
						for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it){
							if (PLD_array.at(it->second).lut.output == portHashMap[words.at(3).c_str() + 2]){
								pld_id = it->second; //id of pld associated with it.
								break;
							}
						}

						assert(pld_id != -1);
						//register clock to an address in hash map if not added
						if (portHashMap.find(words.at(2).c_str() + 2) == portHashMap.end()){
							portHashMap[words.at(2).c_str() + 2] = portID++;
						}
						//register Data to an address in hash map if not added
						if (portHashMap.find(words.at(3).c_str() + 2) == portHashMap.end()){
							portHashMap[words.at(3).c_str() + 2] = portID++;
						}
						//register output to an address in hash map if not added
						if (portHashMap.find(words.at(4).c_str() + 2) == portHashMap.end()){
							portHashMap[words.at(4).c_str() + 2] = portID++;
						}

						//add to data structure
						PLD_array.at(pld_id).dff.clock = portHashMap[words.at(2).c_str() + 2]; //clock
						PLD_array.at(pld_id).dff.data = portHashMap[words.at(3).c_str() + 2]; //data
						PLD_array.at(pld_id).dff.output = portHashMap[words.at(4).c_str() + 2]; //output

						PLD_array.at(pld_id).dff.enabled = true; //enable it
					}
					else{
						print(line);
						print("Warning: Missing LUT or DFF");
						continue;
					}
				}
				else if (!words.at(0).compare("param") && !words.at(1).compare("LUT_INIT")){
					strcpy(PLD_array.at(PLDID - 1).lut.initVal, words.at(2).c_str());
				}
				else continue;
			}
		}
		fp.close();
		printf("Graph generated with %d PLD's!\n", PLDID);
	}
	else{
		//file read error
		assert(true);
	}
	return PLD_array;
}

void printPLDs(std::vector<PLD> &node){
	print("****printing graph******\n\n");
	for (int i = 0; i < node.size(); i++){
		if (node.at(i).dff.enabled)
			printf("%d LUT[%s]: %d %d %d %d => %d || DFF %d %d => %d\n", i, node.at(i).lut.initVal, node.at(i).lut.input[0], node.at(i).lut.input[1], node.at(i).lut.input[2], node.at(i).lut.input[3], node.at(i).lut.output, node.at(i).dff.clock, node.at(i).dff.data, node.at(i).dff.output);
		else
			printf("%d LUT[%s]: %d %d %d %d => %d\n", i, node.at(i).lut.initVal, node.at(i).lut.input[0], node.at(i).lut.input[1], node.at(i).lut.input[2], node.at(i).lut.input[3], node.at(i).lut.output);
	}
}