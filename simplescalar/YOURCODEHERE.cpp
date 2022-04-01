#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

#include "431project.h"

using namespace std;

/*
 * Enter your PSU IDs here to select the appropriate scanning order.
 */
#define PSU_ID_SUM (920177802+915750011) //PSU_ID_SUM mod 24 == 5

/*
 * Some global variables to track heuristic progress.
 * 
 * Feel free to create more global variables to track progress of your
 * heuristic.
 */
unsigned int currentlyExploringDim = 0;
bool currentDimDone = false;
bool isDSEComplete = false;

/*
 * Given a half-baked configuration containing cache properties, generate
 * latency parameters in configuration string. You will need information about
 * how different cache paramters affect access latency.
 * 
 * Returns a string similar to "1 1 1"
 */
std::string generateCacheLatencyParams(string halfBackedConfig) {

	string latencySettings;

	//
	//YOUR CODE BEGINS HERE
	//

	
	std::stringstream latency;
	
	int dl1Size = getdl1size(halfBackedConfig);
	int il1Size = getil1size(halfBackedConfig);
	int ul2Size = getl2size(halfBackedConfig);
	
	
	latency  << log2(dl1Size/1024) + extractConfigPararm(halfBackedConfig, 4) -1 << " " << log2(il1Size/1024) + extractConfigPararm(halfBackedConfig, 6) -1 << " " << log2(ul2Size/1024) + extractConfigPararm(halfBackedConfig, 9) - 5;
	latencySettings = latency.str();
	//
	//YOUR CODE ENDS HERE
	//

	return latencySettings;
}

/*
 * Returns 1 if configuration is valid, else 0
 */
int validateConfiguration(std::string configuration) {

	// FIXME - YOUR CODE HERE
	
	unsigned int il1block_size = 8*(1 << extractConfigPararm(configuration, 2));
	unsigned int dl1block_size = 8*(1 << extractConfigPararm(configuration, 2));
	unsigned int ul2block_size = 16 << extractConfigPararm(configuration, 8);

	unsigned int dl1Size = getdl1size(configuration);
	unsigned int il1Size = getil1size(configuration);
	unsigned int ul2Size = getl2size(configuration);

	int ifq = 8*(1<<extractConfigPararm(configuration, 0));
	
	
	if (il1block_size < ifq){ //constraint 1
		return 0;
		}
	else if (il1block_size != dl1block_size){ //???????????????????constraint 1 (second part)
		return 0;
		}
	else if (ul2Size < 2*(dl1Size + il1Size)) { //constraint 2. Check piazza
		return 0;
		}
	else if (ul2block_size < 2*dl1block_size) { //constraint 2   ul2 >= 2* (il1+dl1)
		return 0;
		}
	else if (ul2block_size > 128) { //constraint 2, ul2block_size <= 128B
		return 0;
		}
	else if ((il1Size < 2*1024) || (il1Size > 64*1024)) { //constraint 3
		return 0;
		}
	else if ((dl1Size < 2*1024) || (dl1Size > 64*1024)) { //constraint 3 
		return 0;
		}
	else if ((ul2Size < 32*1024) || (ul2Size>1024*1024)) { //constraint 4
		return 0;
		}
	else {
		return 1;
	}

	// The below is a necessary, but insufficient condition for validating a
	// configuration.
	return isNumDimConfiguration(configuration);
}

/*
 * Given the current best known configuration, the current configuration,
 * and the globally visible map of all previously investigated configurations,
 * suggest a previously unexplored design point. You will only be allowed to
 * investigate 1000 design points in a particular run, so choose wisely.
 *
 * In the current implementation, we start from the leftmost dimension and
 * explore all possible options for this dimension and then go to the next
 * dimension until the rightmost dimension.
 */
std::string generateNextConfigurationProposal(std::string currentconfiguration,
		std::string bestEXECconfiguration, std::string bestEDPconfiguration,
		int optimizeforEXEC, int optimizeforEDP) {

	//
	// Some interesting variables in 431project.h include:
	//
	// 1. GLOB_dimensioncardinality
	// 2. GLOB_baseline
	// 3. NUM_DIMS
	// 4. NUM_DIMS_DEPENDENT
	// 5. GLOB_seen_configurations

	std::string nextconfiguration = currentconfiguration;
	int traversal [NUM_DIMS-NUM_DIMS_DEPENDENT] = {12, 13, 14, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; //Traversing based on 5th exploration order
	int skipCounter = 0; //check how many times while loop has run without returning. After the last valid config the while loop gets stuck
	// Continue if proposed configuration is invalid or has been seen/checked before.
	while (!validateConfiguration(nextconfiguration) ||
		GLOB_seen_configurations[nextconfiguration]) {
		//////////cout << !validateConfiguration(nextconfiguration) << " || " << GLOB_seen_configurations[nextconfiguration] << endl;
		// Check if DSE has been completed before and return current
		// configuration.
		if(isDSEComplete) {
			//return currentconfiguration;
			currentlyExploringDim = 0;
			isDSEComplete = false;
			//currentDimDone = false;
		}

		std::stringstream ss;

		string bestConfig;
		if (optimizeforEXEC == 1)
			bestConfig = bestEXECconfiguration;

		if (optimizeforEDP == 1)
			bestConfig = bestEDPconfiguration;

		std::string temp = bestConfig; //Hold modified configuration, replaces variable ss
		// Fill in the dimensions already-scanned with the already-selected best
		// value.

		//////////cout << "best config: " << temp << endl;
		for (int dim = 0; dim < currentlyExploringDim; ++dim) {
			temp[traversal[dim]*2] = bestConfig[traversal[dim]*2];//extractConfigPararm(bestConfig, traversal[dim]);
			//ss << extractConfigPararm(bestConfig, traversal[dim]) << " ";
		}
		//////////cout << "config so far: " << temp << endl;
		// Handling for currently exploring dimension. This is a very dumb
		// implementation.
		int nextValue = extractConfigPararm(nextconfiguration,
				traversal[currentlyExploringDim]) + 1;
		//////////cout << "next val: " << nextValue << endl;
		if (nextValue >= GLOB_dimensioncardinality[traversal[currentlyExploringDim]]) {
			nextValue = GLOB_dimensioncardinality[traversal[currentlyExploringDim]] - 1;
			currentDimDone = true;
			temp[traversal[(currentlyExploringDim+1)%(NUM_DIMS-NUM_DIMS_DEPENDENT)]*2] = '0';//make new dim 0
			//////////cout << "FINISHED DIM" << endl;
		}

		//ss << nextValue << " ";
		char next = '0' + nextValue; //change nextValue to a char
		temp[traversal[currentlyExploringDim]*2] = next;
		//////////cout << "config after change ("<< nextValue << "|" <<currentlyExploringDim <<"): " << temp << endl;
		// Fill in remaining independent params with 0.
		//for (int dim = (currentlyExploringDim + 1);
		//		dim < (NUM_DIMS - NUM_DIMS_DEPENDENT); ++dim) {
		//	ss << "0 ";
		//	//temp[traversal[dim]*2] = '0';
		//}
		//////////cout<<"after 0: "<< temp<<endl; 

		//
		// Last NUM_DIMS_DEPENDENT3 configuration parameters are not independent.
		// They depend on one or more parameters already set. Determine the
		// remaining parameters based on already decided independent ones.
		//
		string configSoFar = temp;//ss.str();

		// Populate this object using corresponding parameters from config.
		//ss << generateCacheLatencyParams(configSoFar);
		//////////cout << "before replace: " << temp << endl;
		temp.replace(15*2,5, generateCacheLatencyParams(configSoFar));
		//////////cout << "after replace: " << temp << endl;

		// Configuration is ready now.
		nextconfiguration = temp;//ss.str();

		// Make sure we start exploring next dimension in next iteration.
		if (currentDimDone) {
			currentlyExploringDim++;
			currentDimDone = false;
		}

		// Signal that DSE is complete after this configuration.
		if (currentlyExploringDim == (NUM_DIMS - NUM_DIMS_DEPENDENT))
			isDSEComplete = true;
		//////////cout << "ctr: " << skipCounter << endl;
		if (++skipCounter>100) //check if stuck in while loop
			return currentconfiguration;
	}
	return nextconfiguration;
}

