#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <getopt.h>

using namespace std;

struct vertexStruct{
	bool visited = false;
	double distance = numeric_limits<double>::infinity();
	int connectedID = -1;
	int xPos;
	int yPos;
	bool wild = false;
	bool board = false;
}vertexInfo;

vector<vertexStruct> vertexVec;
deque<int> currPath;
deque<int> bestPath;

bool hasKind = false;
bool hasWild = false;
bool hasBorder = false;

double upperBound = 0;
int numCages;

string getMode(int argc, char * argv[]);
double findDist(vertexStruct &lhs, vertexStruct &rhs);
void MST();
void FASTTSP();
void printRecurse(int input);
void upperBoundMaker(vector<vector<double>> &distVec);
void makeDistVec(vector<vector<double>> &distVec);
void OPTTSP(deque<int> &notVisited, deque<int> &currPath, deque<int> &bestPath, double currDist, vector<vector<double>> &distVec);
double makeMST(deque<int> &notVisited, int first, int last, vector<vector<double>> &distVec);

int main(int argc, char* argv[]){

	cout << setprecision(2);
	cout << fixed;

	std::ios_base::sync_with_stdio(false);

	string mode = getMode(argc, argv);

	int xPos;
	int yPos;

	cin >> numCages;
	vertexVec.reserve(numCages);

	if(mode == "MST"){
		for(int i = 0; i < numCages; i++){
			cin >> xPos >> yPos;
			vertexInfo.xPos = xPos;
			vertexInfo.yPos = yPos;

			if(xPos < 0 && yPos < 0){
				hasWild = true;
				vertexInfo.wild = true;
			}
			else if((xPos < 0 && yPos == 0) || (xPos == 0 && yPos < 0) || (xPos == 0 && yPos == 0)){
				hasBorder = true;
				vertexInfo.board = true;
			}
			else{
				hasKind = true;
			}
			vertexVec.push_back(vertexInfo);
			vertexInfo.board = false;
			vertexInfo.wild = false;
		}
		if(!hasBorder){
			if(hasWild && hasKind){
				cerr << "Cannot construct MST";
				exit(1);
			}
		}
		MST();
	}
	else if(mode == "FASTTSP"){
		for(int i = 0; i < numCages; i++){
			cin >> xPos >> yPos;
			vertexInfo.xPos = xPos;
			vertexInfo.yPos = yPos;
			vertexVec.push_back(vertexInfo);
		}
		FASTTSP();
	}
	else if(mode == "OPTTSP"){
		vector<vector<double>> distVec(numCages, vector<double>(numCages, numeric_limits<double>::infinity()));
		double currDist = 0;
		for(int i = 0; i < numCages; i++){
			cin >> xPos >> yPos;
			vertexInfo.xPos = xPos;
			vertexInfo.yPos = yPos;
			vertexVec.push_back(vertexInfo);
		}
		makeDistVec(distVec);
		upperBoundMaker(distVec);
		deque<int> notVisited;
		notVisited = bestPath;
		notVisited.pop_front();
		currPath.push_back(0);
		OPTTSP(notVisited, currPath, bestPath, currDist, distVec);
		cout << upperBound << endl;
		cout << "0";
		for(int i = 1; i < numCages; i++){
			cout << " " << bestPath[i];
		}
		cout << endl;
	}
	return 0;
}

string getMode(int argc, char * argv[]) {
    // These are used with getopt_long()
	string mode;
    opterr = true; // Give us help with errors
    int choice;
    int option_index = 0;
    option long_options[] = {
    	{ "help", no_argument, nullptr, 'h' },
    	{ "mode", required_argument, nullptr, 'm' },
    	{ nullptr, 0,                 nullptr, '\0' }
    };
    while ((choice = getopt_long(argc, argv, "hm:", long_options, &option_index)) != -1) {
    	switch(choice) {
    		case 'h':
    		cout << "help message" << endl;
    		exit(0);

    		case 'm':
    		mode = optarg;
    		break;

    		default:
    		cerr << "Error: invalid option " << endl;
    		exit(1);
        } // switch
    } // while

    return mode;
}

double findDist(vertexStruct &lhs, vertexStruct &rhs){
	double lhsX = (double)lhs.xPos;
	double lhsY = (double)lhs.yPos;
	double rhsX = (double)rhs.xPos;
	double rhsY = (double)rhs.yPos;
	return (lhsX - rhsX) * (lhsX - rhsX) + (lhsY - rhsY) * (lhsY - rhsY);
}

void MST(){

	vertexVec[0].distance = 0; //START AT FIRST CAGE, SO SET DISTANCE TO 0
	vertexVec[0].visited = true;

	int numVisited = 0;
	int nextIndex = -1;
	while(numVisited != numCages){
		double minDistance = numeric_limits<double>::infinity();
		if(numVisited == 0){
			nextIndex = 0;
		}
		else{
			for(int i = 0; i < numCages; i++){
				if(vertexVec[i].visited == false){
					if(vertexVec[i].distance < minDistance){
						minDistance = vertexVec[i].distance;
						nextIndex = i;
					}
				}
			}
		}
		vertexVec[nextIndex].visited = true;
		numVisited++;
		for(int i = 0; i < numCages; i++){
			double newDist = numeric_limits<double>::infinity();
			if(vertexVec[i].visited == false){
				if(vertexVec[i].wild == vertexVec[nextIndex].wild || vertexVec[i].board == true || vertexVec[nextIndex].board == true){
					newDist = findDist(vertexVec[nextIndex], vertexVec[i]);
				}
				if(newDist < vertexVec[i].distance){
					vertexVec[i].distance = newDist;
					vertexVec[i].connectedID = nextIndex;
				}
			}
		}
	}

	double totalWeight = 0;
	for(int i  = 0; i < numCages; i++){
		totalWeight += (double)sqrt(vertexVec[i].distance);
	}

	cout << totalWeight << endl;

	for(int i = 1; i < numCages; i++){
		if(i < vertexVec[i].connectedID){
			cout << i << " " << vertexVec[i].connectedID << endl;
		}
		else{
			cout << vertexVec[i].connectedID << " " << i << endl;
		}
	}
}

void FASTTSP(){

	//CONNECT ID IS CHILD ID

	int numVisited = 2;
	int firstAdd = -1;
	int reachFrom = -1;
	vector<int> visitedVec;
	visitedVec.reserve(numCages);

	double minDistance = numeric_limits<double>::infinity();
	for(int i = 1; i < numCages; i++){
		double tempDist = findDist(vertexVec[0], vertexVec[i]);
		if(tempDist < minDistance){
			minDistance = tempDist;
			firstAdd = i;
		}
	}

	vertexVec[0].connectedID = firstAdd;
	vertexVec[0].visited = true;
	vertexVec[0].distance = findDist(vertexVec[0], vertexVec[firstAdd]);

	vertexVec[firstAdd].visited = true;
	vertexVec[firstAdd].connectedID = 0;
	vertexVec[firstAdd].distance = findDist(vertexVec[0], vertexVec[firstAdd]);

	visitedVec.push_back(0);
	visitedVec.push_back(firstAdd);

	for(int i = 0; i < numCages; i++){
		if(vertexVec[i].visited == false){
			minDistance = numeric_limits<double>::infinity();
			for(int j = 0; j < numVisited; j++){
				double temp1 = findDist(vertexVec[visitedVec[j]], vertexVec[i]);
				double temp2 = findDist(vertexVec[vertexVec[visitedVec[j]].connectedID], vertexVec[i]);
				double temp3 = vertexVec[visitedVec[j]].distance;
				double tempDist = sqrt(temp1) + sqrt(temp2) - sqrt(temp3);
				tempDist = tempDist * tempDist;
				if(tempDist < minDistance){
					minDistance = tempDist;
					reachFrom = visitedVec[j];
				}
			}
			vertexVec[i].visited = true;
			vertexVec[i].connectedID = vertexVec[reachFrom].connectedID;
			vertexVec[i].distance = findDist(vertexVec[vertexVec[i].connectedID], vertexVec[i]);     //findDist(vertexVec[reachFrom], vertexVec[i]);
			visitedVec.push_back(i);
			numVisited++;

			vertexVec[reachFrom].connectedID = i;
			vertexVec[reachFrom].distance = findDist(vertexVec[i], vertexVec[reachFrom]);
		}
	}

	double totalWeight = 0;
	for(int i = 0; i < numCages; i++){
		totalWeight += (double)sqrt(vertexVec[i].distance);
	}
	cout << totalWeight << endl;

	int nextPrint = 0;
	cout << "0";
	for(int i = 1; i < numCages; i++){
		nextPrint = vertexVec[nextPrint].connectedID;
		cout << " " << nextPrint;
	}
	cout << endl;
}

double makeMST(deque<int> &notVisited, int first, int last, vector<vector<double>> &distVec){
	double lowerBoundMade = 0;

	deque<vertexStruct> locVerVec;
	locVerVec.resize((unsigned)numCages);
	for(int i = 0; i < numCages; i++){
		locVerVec[i].xPos = vertexVec[i].xPos;
		locVerVec[i].yPos = vertexVec[i].yPos;
	}

	locVerVec[notVisited[0]].distance = 0; //START AT FIRST CAGE, SO SET DISTANCE TO 0
	locVerVec[notVisited[0]].visited = true;

	int numLeft = (int)notVisited.size();
	int numVisited = 0;
	int nextIndex = -1;
	while(numVisited != numLeft){
		double minDistance = numeric_limits<double>::infinity();
		if(numVisited == 0){
			nextIndex = notVisited[0];
		}
		else{
			for(int i = 0; i < numLeft; i++){
				if(locVerVec[notVisited[i]].visited == false){
					if(locVerVec[notVisited[i]].distance < minDistance){
						minDistance = locVerVec[notVisited[i]].distance;
						nextIndex = notVisited[i];
					}
				}
			}
		}
		locVerVec[nextIndex].visited = true;
		numVisited++;
		for(int i = 0; i < numLeft; i++){
			double newDist = numeric_limits<double>::infinity();
			if(locVerVec[notVisited[i]].visited == false){
				newDist = distVec[nextIndex][notVisited[i]];
				if(newDist < locVerVec[notVisited[i]].distance){
					locVerVec[notVisited[i]].distance = newDist;
					locVerVec[notVisited[i]].connectedID = nextIndex;
				}
			}
		}
	}

	for(int i  = 0; i < numLeft; i++){
		lowerBoundMade += locVerVec[notVisited[i]].distance;
	}

	double minToFirst = numeric_limits<double>::infinity();
	double minToLast = numeric_limits<double>::infinity();

	for(int i = 0; i < numLeft; i++){
		if(distVec[notVisited[i]][first] < minToFirst){
			minToFirst = distVec[notVisited[i]][first];
		}
		if(distVec[notVisited[i]][last] < minToLast){
			minToLast = distVec[notVisited[i]][last];
		}
	}

	lowerBoundMade += minToFirst;
	lowerBoundMade += minToLast;
	return lowerBoundMade;
}

bool promising(deque<int> &notVisited, deque<int> &currPath, double currDist, vector<vector<double>> &distVec){
	double tempDist = currDist + makeMST(notVisited, currPath.front(), currPath.back(), distVec);
	if(tempDist < upperBound){
		return true;
	}
	else{
		return false;
	}
}

void OPTTSP(deque<int> &notVisited, deque<int> &currPath, deque<int> &bestPath, double currDist, vector<vector<double>> &distVec){
	if(notVisited.empty()){
		currDist += distVec[currPath.front()][currPath.back()];
		if(currDist < upperBound){
			upperBound = currDist;
			bestPath = currPath;
		}
		return;
	}
	if(notVisited.size() > 4){
		if(!promising(notVisited, currPath, currDist, distVec)){
			return;
		}
	}
	for(unsigned k = 0; k != notVisited.size(); k++){
		double temp4 = distVec[currPath.back()][notVisited.front()];
		currPath.push_back(notVisited.front());
		notVisited.pop_front();
		OPTTSP(notVisited, currPath, bestPath, currDist + temp4, distVec);
		notVisited.push_back(currPath.back());
		currPath.pop_back();
	}
}

void makeDistVec(vector<vector<double>> &distVec){
	for(int i = 0; i < numCages; i++){
		for(int j = 0; j < numCages; j++){
			if(distVec[i][j] == numeric_limits<double>::infinity()){
				double temp = sqrt(findDist(vertexVec[i], vertexVec[j]));
				distVec[i][j] = temp;
				distVec[j][i] = temp;
			}
		}
	}
}

void upperBoundMaker(vector<vector<double>> &distVec){
	int numVisited = 2;
	int firstAdd = -1;
	int reachFrom = -1;
	vector<int> visitedVec;
	visitedVec.reserve(numCages);

	double minDistance = numeric_limits<double>::infinity();
	for(int i = 1; i < numCages; i++){
		double tempDist = distVec[0][i];
		if(tempDist < minDistance){
			minDistance = tempDist;
			firstAdd = i;
		}
	}

	vertexVec[0].connectedID = firstAdd;
	vertexVec[0].visited = true;
	vertexVec[0].distance = distVec[0][firstAdd];

	vertexVec[firstAdd].visited = true;
	vertexVec[firstAdd].connectedID = 0;
	vertexVec[firstAdd].distance = distVec[0][firstAdd];

	visitedVec.push_back(0);
	visitedVec.push_back(firstAdd);

	for(int i = 0; i < numCages; i++){
		if(vertexVec[i].visited == false){
			minDistance = numeric_limits<double>::infinity();
			for(int j = 0; j < numVisited; j++){
				double temp1 = distVec[visitedVec[j]][i];
				double temp2 = distVec[vertexVec[visitedVec[j]].connectedID][i];
				double temp3 = vertexVec[visitedVec[j]].distance;
				double tempDist = temp1 + temp2 - temp3;
				tempDist = tempDist * tempDist;
				if(tempDist < minDistance){
					minDistance = tempDist;
					reachFrom = visitedVec[j];
				}
			}
			vertexVec[i].visited = true;
			vertexVec[i].connectedID = vertexVec[reachFrom].connectedID;
			vertexVec[i].distance = distVec[vertexVec[i].connectedID][i];     //findDist(vertexVec[reachFrom], vertexVec[i]);
			visitedVec.push_back(i);
			numVisited++;

			vertexVec[reachFrom].connectedID = i;
			vertexVec[reachFrom].distance = distVec[i][reachFrom];
		}
	}
	for(int i = 0; i < numCages; i++){
		upperBound += vertexVec[i].distance;
	}
	int nextPrint = 0;
	for(int i = 0; i < numCages; i++){
		bestPath.push_back(nextPrint);
		nextPrint = vertexVec[nextPrint].connectedID;
	}
}