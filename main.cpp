#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <time.h>
#include <stdlib.h>
using namespace std;

double findMax(vector<vector<double> >& prevUtil, vector<vector<int> >& direction, int i, int j);

void valueIteration(double Rs, vector<vector<int> >& direction);

void valueIterationWithUtility(double Rs, vector<vector<int> >& direction, vector<vector<double> >& utilities);

void monteCarlo(int numIterations);

bool takeStep (vector<vector<int> >& monteCarloDirection, int &agentX, int &agentY, double &score);

void q3valueIteration(double gamma, vector<vector<int> >& direction);

void q3valueIterationWithUtility(double gamma, vector<vector<int> >& direction, vector<vector<double> >& utilities);

double q3findMax(vector<vector<double> >& prevUtil, vector<vector<int> >& direction, int i, int j);

int main() {
	srand(time(NULL));
	//START PART 1
	
	//a matrix to store the direction of each state
	vector<vector<int> > lowerDirection(3, vector<int>(4,0)); // 0 = LEFT, 1 = UP, 2 = RIGHT, 3 = DOWN
	vector<vector<int> > upperDirection(3, vector<int>(4,0));
	vector<vector<int> > midpointDirection(3, vector<int>(4,0));

	int thresholdsFound = 0; //count of found thresholds
	vector<double> thresholds; //vector to store found thresholds
	double lowerBound = -100, upperBound = 0; //lower and upper bound for binary search algorithm
	
	//find the thresholds
	while (thresholdsFound < 8) {
		if (thresholdsFound != 0)
			lowerBound = thresholds[thresholdsFound - 1] + 0.0001;
		upperBound = 0;
		//clear all direction vectors
		
		for (int i = 0; i < 3; i++) {
			lowerDirection[i].clear();
			upperDirection[i].clear();
			midpointDirection[i].clear();
		}

		while (abs(lowerBound - upperBound) > 0.0001) {
			valueIteration(lowerBound, lowerDirection);
			valueIteration(upperBound, upperDirection);
			
			double midpoint = (lowerBound + upperBound) / 2;
			valueIteration(midpoint, midpointDirection);
			//check to see if policy is the same
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 4; j++) {
					if (!((i == 0 && j == 3) || (i == 1 && j == 3) || (i == 1 && j == 1))) {
						if (lowerDirection[i][j] != midpointDirection[i][j])
							upperBound = midpoint;
					}
				}
			}
			if (upperBound != midpoint) {
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 4; j++) {
						if (!((i == 0 && j == 3) || (i == 1 && j == 3) || (i == 1 && j == 1))) {
							if (upperDirection[i][j] != midpointDirection[i][j])
								lowerBound = midpoint;
						}
					}
				}
			}
		}
		thresholds.push_back(lowerBound);
		thresholdsFound++;
	}
	
	
		

	//first output file
	ofstream outfile;
	outfile.open("generated/P1-output.txt");
	
	//set precision
	outfile.setf(ios::fixed, ios::floatfield);
	outfile.precision(4);
	
	vector<vector<int> > direction(3, vector<int>(4,0));
	vector<vector<int> > prevDirection(3, vector<int>(4,0));

	valueIteration(0, direction);
	outfile << "0" << "\n";
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == 1 && j == 1)
				outfile << "X ";
			else if (i == 0 && j == 3)
				outfile << "1 ";
			else if (i == 1 && j == 3)
				outfile << "-1 ";
			else {
				if (direction[i][j] == 0)
					outfile << "< ";
				else if (direction[i][j] == 1)
					outfile << "^ ";
				else if (direction[i][j] == 2)
					outfile << "> ";
				else if (direction[i][j] == 3)
					outfile << "v ";
			}
		}
		outfile << "\n";
	}
	
	for (int k = thresholds.size() - 1; k >= 0; k--) {
		prevDirection = direction;
		valueIteration(thresholds.at(k), direction);
		outfile << thresholds[k] << "\n";
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == 1 && j == 1)
					outfile << "X";
				else if (i == 0 && j == 3)
					outfile << "1";
				else if (i == 1 && j == 3)
					outfile << "-1";
				else {
					if (direction[i][j] == 0)
						outfile << "<";
					else if (direction[i][j] == 1)
						outfile << "^";
					else if (direction[i][j] == 2)
						outfile << ">";
					else if (direction[i][j] == 3)
						outfile << "v";
				}
				if (direction[i][j] != prevDirection[i][j])
					outfile << "* ";
				else
					outfile << " ";
			}
			outfile << "\n";
		}
	}
	
	outfile.close();
	
	//END PART 1
	//////////////////////////////////////////////////////////////////////////////////////////////
	//START PART 2
	outfile.open("generated/P2-output.txt");
		outfile.precision(3);
	vector<vector<int> > monteCarloDirection(3, vector<int>(4,0));
	vector<vector<double> > monteCarloUtility(3, vector<double>(4,0));
	
	//call value iteration to generate direction map
	valueIterationWithUtility(-0.04, monteCarloDirection, monteCarloUtility);
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == 1 && j == 1)
				outfile << "0 ";
			else
				outfile << monteCarloUtility[i][j] << " ";
		}
		outfile << "\n";
	}
	
	monteCarlo(10);
	monteCarlo(100);
	monteCarlo(1000);
	
	outfile.close();
	
	//END PART 2
	//////////////////////////////////////////////////////////////////////////////////////////////
	//START PART 3
	//a matrix to store the direction of each state
	vector<vector<int> > q3lowerDirection(3, vector<int>(3,0)); // 0 = LEFT, 1 = UP, 2 = RIGHT, 3 = DOWN
	vector<vector<int> > q3upperDirection(3, vector<int>(3,0));
	vector<vector<int> > q3midpointDirection(3, vector<int>(3,0));
	
	int q3thresholdsFound = 0; //count of found thresholds
	vector<double> q3thresholds; //vector to store found thresholds
	double q3lowerBound = 0.01, q3upperBound = 0.99; //lower and upper bound for binary search algorithm
	
	//find the thresholds
	while (q3thresholdsFound < 5) {
		if (q3thresholdsFound != 0)
			q3lowerBound = q3thresholds[q3thresholdsFound - 1] + 0.000001;
		q3upperBound = 0.99;
		//clear all direction vectors
		
		for (int i = 0; i < 3; i++) {
			q3lowerDirection[i].clear();
			q3upperDirection[i].clear();
			q3midpointDirection[i].clear();
		}

		while (abs(q3lowerBound - q3upperBound) > 0.0000001) {
			q3valueIteration(q3lowerBound, q3lowerDirection);
			q3valueIteration(q3upperBound, q3upperDirection);
			
			double q3midpoint = (q3lowerBound + q3upperBound) / 2;
			q3valueIteration(q3midpoint, q3midpointDirection);
			//check to see if policy is the same
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (!(i == 0 && j == 2)) {
						if (q3lowerDirection[i][j] != q3midpointDirection[i][j])
							q3upperBound = q3midpoint;
					}
				}
			}
			if (q3upperBound != q3midpoint) {
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						if (!(i == 0 && j == 2)) {
							if (q3upperDirection[i][j] != q3midpointDirection[i][j])
								q3lowerBound = q3midpoint;
						}
					}
				}
			}
		}
		q3thresholds.push_back(q3lowerBound);
		q3thresholdsFound++;
	}

	//p3 output file
	
	outfile.open("generated/P3-output.txt");
	
	//set precision
	outfile.setf(ios::fixed, ios::floatfield);
	outfile.precision(5);
	
	vector<vector<int> > q3direction(3, vector<int>(3,0));
	vector<vector<int> > q3prevDirection(3, vector<int>(3,0));
	vector<vector<double> > q3utilities(3, vector<double>(3,0));
	
	q3valueIteration(0, q3direction);
	outfile << "0" << "\n";
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == 0 && j == 2)
				outfile << "10 ";
			else {
				if (q3direction[i][j] == 0)
					outfile << "< ";
				else if (q3direction[i][j] == 1)
					outfile << "^ ";
				else if (q3direction[i][j] == 2)
					outfile << "> ";
				else if (q3direction[i][j] == 3)
					outfile << "v ";
			}
		}
		outfile << "\n";
	}
	outfile.precision(3);
		
	q3valueIterationWithUtility(0, q3direction, q3utilities);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			outfile << q3utilities[i][j] << " ";
		outfile << "\n";
	}
	
	
	for (int k = 0; k < q3thresholds.size(); k++) {
		outfile.precision(5);
		q3prevDirection = q3direction;
		q3valueIteration(q3thresholds[k], q3direction);
		outfile << q3thresholds[k] << "\n";
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (i == 0 && j == 2)
					outfile << "10";
				else {
					if (q3direction[i][j] == 0)
						outfile << "<";
					else if (q3direction[i][j] == 1)
						outfile << "^";
					else if (q3direction[i][j] == 2)
						outfile << ">";
					else if (q3direction[i][j] == 3)
						outfile << "v";
				}
				if (q3direction[i][j] != q3prevDirection[i][j])
					outfile << "* ";
				else
					outfile << " ";
			}
			outfile << "\n";
		}
		outfile.precision(3);
		
		q3valueIterationWithUtility(q3thresholds[k], q3direction, q3utilities);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				outfile << q3utilities[i][j] << " ";
			outfile << "\n";
		}
		
	}
	
	outfile.close();
	
	
	
	return 0;
}

double findMax(vector<vector<double> >& prevUtil, vector<vector<int> >& direction, int i, int j) {
	double maxUtil;
	double leftUtil, upUtil, rightUtil, downUtil;
	if (i == 0 && j == 0) {
		leftUtil = 0.9 * prevUtil[0][0] + 0.1 * prevUtil[1][0];
		upUtil = 0.9 * prevUtil[0][0] + 0.1 * prevUtil[0][1];
		rightUtil = 0.8 * prevUtil[0][1] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[1][0];
		downUtil = 0.8 * prevUtil[1][0] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[0][1];
	}
	else if (i == 0 && j == 1) {
		leftUtil = 0.8 * prevUtil[0][0] + 0.2 * prevUtil[0][1];
		upUtil = 0.8 * prevUtil[0][1] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[0][2];
		rightUtil = 0.8 * prevUtil[0][2] + 0.2 * prevUtil[0][1];
		downUtil = 0.8 * prevUtil[0][1] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[0][2];
	}
	else if (i == 0 && j == 2) {
		leftUtil = 0.8 * prevUtil[0][1] + 0.1 * prevUtil[0][2] + 0.1 * prevUtil[1][2];
		upUtil = 0.8 * prevUtil[0][2] + 0.1 * prevUtil[0][1] + 0.1 * prevUtil[0][3];
		rightUtil = 0.8 * prevUtil[0][3] + 0.1 * prevUtil[0][2] + 0.1 * prevUtil[1][2];
		downUtil = 0.8 * prevUtil[1][2] + 0.1 * prevUtil[0][1] + 0.1 * prevUtil[0][3];
	}
	else if (i == 1 && j == 0) {
		leftUtil = 0.8 * prevUtil[1][0] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[2][0];
		upUtil = 0.8 * prevUtil[0][0] + 0.2 * prevUtil[1][0];
		rightUtil = 0.8 * prevUtil[1][0] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[2][0];
		downUtil = 0.8 * prevUtil[2][0] + 0.2 * prevUtil[1][0];
	}
	else if (i == 1 && j == 2) {
		leftUtil = 0.8 * prevUtil[1][2] + 0.1 * prevUtil[0][2] + 0.1 * prevUtil[2][2];
		upUtil = 0.8 * prevUtil[0][2] + 0.1 * prevUtil[1][2] + 0.1 * prevUtil[1][3];
		rightUtil = 0.8 * prevUtil[1][3] + 0.1 * prevUtil[0][2] + 0.1 * prevUtil[2][2];
		downUtil = 0.8 * prevUtil[2][2] + 0.1 * prevUtil[1][2] + 0.1 * prevUtil[1][3];
	}
	else if (i == 2 && j == 0) {
		leftUtil = 0.9 * prevUtil[2][0] + 0.1 * prevUtil[1][0];
		upUtil = 0.8 * prevUtil[1][0] + 0.1 * prevUtil[2][0] + 0.1 * prevUtil[2][1];
		rightUtil = 0.8 * prevUtil[2][1] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[1][0];
		downUtil = 0.9 * prevUtil[2][0] + 0.1 * prevUtil[2][1];
	}
	else if (i == 2 && j == 1) {
		leftUtil = 0.8 * prevUtil[2][0] + 0.2 * prevUtil[2][1];
		upUtil = 0.8 * prevUtil[2][1] + 0.1 * prevUtil[2][0] + 0.1 * prevUtil[2][2];
		rightUtil = 0.8 * prevUtil[2][2] + 0.2 * prevUtil[2][1];
		downUtil = 0.8 * prevUtil[2][1] + 0.1 * prevUtil[2][0] + 0.1 * prevUtil[2][2];
	}
	else if (i == 2 && j == 2) {
		leftUtil = 0.8 * prevUtil[2][1] + 0.1 * prevUtil[1][2] + 0.1 * prevUtil[2][2];
		upUtil = 0.8 * prevUtil[1][2] + 0.1 * prevUtil[2][1] + 0.1 * prevUtil[2][3];
		rightUtil = 0.8 * prevUtil[2][3] + 0.1 * prevUtil[1][2] + 0.1 * prevUtil[2][2];
		downUtil = 0.8 * prevUtil[2][2] + 0.1 * prevUtil[2][1] + 0.1 * prevUtil[2][3];
	}
	else if (i == 2 && j == 3) {
		leftUtil = 0.8 * prevUtil[2][2] + 0.1 * prevUtil[1][3] + 0.1 * prevUtil[2][3];
		upUtil = 0.8 * prevUtil[1][3] + 0.1 * prevUtil[2][2] + 0.1 * prevUtil[2][3];
		rightUtil = 0.9 * prevUtil[2][3] + 0.1 * prevUtil[1][3];
		downUtil = 0.9 * prevUtil[2][3] + 0.1 * prevUtil[2][2];
	}
	
	maxUtil = leftUtil;
	direction[i][j] = 0;
	if (upUtil > maxUtil) {
		maxUtil = upUtil;
		direction[i][j] = 1;
	}
	if (rightUtil > maxUtil) {
		maxUtil = rightUtil;
		direction[i][j] = 2;
	}
	if (downUtil > maxUtil) {
		maxUtil = downUtil;
		direction[i][j] = 3;
	}
	
	return maxUtil;
}

void valueIteration(double Rs, vector<vector<int> >& direction) {
	vector<vector<double> > utils(3, vector<double>(4,0));	
	vector<vector<double> > prevUtils(3, vector<double>(4,0));
	vector<vector<double> > tempUtils(3, vector<double>(4,0));
	//set the terminal states
	utils[0][3] = 1;
	utils[1][3] = -1;
	prevUtils[0][3] = 1;
	prevUtils[1][3] = -1;
	
	//value to test when the utilities have converged
	double convergence = 0.0000001;
	
	bool converged = false;
	while (!converged) {
		converged = true;
		//save all utilities
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++)
				tempUtils[i][j] = utils[i][j];
		}
		//iterate through all states except for the terminal states and non-traversable state
		//update utility and check for convergence
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				if (!((i == 0 && j == 3) || (i == 1 && j == 3) || (i == 1 && j == 1))) {
					utils[i][j] = Rs + findMax(prevUtils, direction, i, j);
					if (abs(utils[i][j] - prevUtils[i][j]) > convergence)
						converged = false;
				}
			}
		}
		//update prevUtils
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++)
				prevUtils[i][j] = tempUtils[i][j];
		}
	}
}

double q3findMax(vector<vector<double> >& prevUtil, vector<vector<int> >& direction, int i, int j) {
	double maxUtil;
	double leftUtil, upUtil, rightUtil, downUtil;
	if (i == 0 && j == 0) {
		leftUtil = 0.9 * prevUtil[0][0] + 0.1 * prevUtil[1][0];
		upUtil = 0.9 * prevUtil[0][0] + 0.1 * prevUtil[0][1];
		rightUtil = 0.8 * prevUtil[0][1] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[1][0];
		downUtil = 0.8 * prevUtil[1][0] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[0][1];
	}
	else if (i == 0 && j == 1) {
		leftUtil = 0.8 * prevUtil[0][0] + 0.1 * prevUtil[0][1] + 0.1 * prevUtil[1][1];
		upUtil = 0.8 * prevUtil[0][1] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[0][2];
		rightUtil = 0.8 * prevUtil[0][2] + 0.1 * prevUtil[0][1] + 0.1 * prevUtil[1][1];
		downUtil = 0.8 * prevUtil[1][1] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[0][2];
	}
	else if (i == 0 && j == 2) {
		leftUtil = 0.8 * prevUtil[0][1] + 0.1 * prevUtil[0][2] + 0.1 * prevUtil[1][2];
		upUtil = 0.9 * prevUtil[0][2] + 0.1 * prevUtil[0][1];
		rightUtil = 0.9 * prevUtil[0][2] + 0.1 * prevUtil[1][2];
		downUtil = 0.8 * prevUtil[1][2] + 0.1 * prevUtil[0][2] + 0.1 * prevUtil[0][1];
	}
	else if (i == 1 && j == 0) {
		leftUtil = 0.8 * prevUtil[1][0] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[2][0];
		upUtil = 0.8 * prevUtil[0][0] + 0.1 * prevUtil[1][1] + 0.1 * prevUtil[1][0];
		rightUtil = 0.8 * prevUtil[1][1] + 0.1 * prevUtil[0][0] + 0.1 * prevUtil[2][0];
		downUtil = 0.8 * prevUtil[2][0] + 0.1 * prevUtil[1][1] + 0.1 * prevUtil[1][0];
	}
	else if (i == 1 && j == 1) {
		leftUtil = 0.8 * prevUtil[1][0] + 0.1 * prevUtil[0][1] + 0.1 * prevUtil[2][1];
		upUtil = 0.8 * prevUtil[0][1] + 0.1 * prevUtil[1][0] + 0.1 * prevUtil[1][2];
		rightUtil = 0.8 * prevUtil[1][2] + 0.1 * prevUtil[0][1] + 0.1 * prevUtil[2][1];
		downUtil = 0.8 * prevUtil[2][1] + 0.1 * prevUtil[1][0] + 0.1 * prevUtil[1][2];
	}
	else if (i == 1 && j == 2) {
		leftUtil = 0.8 * prevUtil[1][1] + 0.1 * prevUtil[0][2] + 0.1 * prevUtil[2][2];
		upUtil = 0.8 * prevUtil[0][2] + 0.1 * prevUtil[1][1] + 0.1 * prevUtil[1][2];
		rightUtil = 0.8 * prevUtil[1][2] + 0.1 * prevUtil[0][2] + 0.1 * prevUtil[2][2];
		downUtil = 0.8 * prevUtil[2][2] + 0.1 * prevUtil[1][1] + 0.1 * prevUtil[1][2];
	}
	else if (i == 2 && j == 0) {
		leftUtil = 0.9 * prevUtil[2][0] + 0.1 * prevUtil[1][0];
		upUtil = 0.8 * prevUtil[1][0] + 0.1 * prevUtil[2][0] + 0.1 * prevUtil[2][1];
		rightUtil = 0.8 * prevUtil[2][1] + 0.1 * prevUtil[1][0] + 0.1 * prevUtil[2][0];
		downUtil = 0.9 * prevUtil[2][0] + 0.1 * prevUtil[2][1];
	}
	else if (i == 2 && j == 1) {
		leftUtil = 0.8 * prevUtil[2][0] + 0.1 * prevUtil[2][1] + 0.1 * prevUtil[1][1];
		upUtil = 0.8 * prevUtil[1][1] + 0.1 * prevUtil[2][0] + 0.1 * prevUtil[2][2];
		rightUtil = 0.8 * prevUtil[2][2] + 0.1 * prevUtil[2][1] + 0.1 * prevUtil[1][1];
		downUtil = 0.8 * prevUtil[2][1] + 0.1 * prevUtil[2][0] + 0.1 * prevUtil[2][2];
	}
	else if (i == 2 && j == 2) {
		leftUtil = 0.8 * prevUtil[2][1] + 0.1 * prevUtil[2][2] + 0.1 * prevUtil[1][2];
		upUtil = 0.8 * prevUtil[1][2] + 0.1 * prevUtil[2][2] + 0.1 * prevUtil[2][1];
		rightUtil = 0.9 * prevUtil[2][2] + 0.1 * prevUtil[1][2];
		downUtil = 0.9 * prevUtil[2][2] + 0.1 * prevUtil[2][1];
	}
	
	maxUtil = leftUtil;
	direction[i][j] = 0;
	if (upUtil > maxUtil) {
		maxUtil = upUtil;
		direction[i][j] = 1;
	}
	if (rightUtil > maxUtil) {
		maxUtil = rightUtil;
		direction[i][j] = 2;
	}
	if (downUtil > maxUtil) {
		maxUtil = downUtil;
		direction[i][j] = 3;
	}
	
	return maxUtil;
}


void q3valueIteration(double gamma, vector<vector<int> >& direction) {
	vector<vector<double> > utils(3, vector<double>(3,0));	
	vector<vector<double> > prevUtils(3, vector<double>(3,0));
	vector<vector<double> > tempUtils(3, vector<double>(3,0));
	//set the terminal states
	utils[0][2] = 10;
	prevUtils[0][2] = 10;
	
	//value to test when the utilities have converged
	double convergence = 0.0000001;
	
	bool converged = false;
	while (!converged) {
		converged = true;
		//save all utilities
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				tempUtils[i][j] = utils[i][j];
		}
		//iterate through all states except for the terminal states and non-traversable state
		//update utility and check for convergence
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (i == 0 && j == 0) {
					utils[i][j] = 3 + gamma * q3findMax(prevUtils, direction, i, j);
					if (abs(utils[i][j] - prevUtils[i][j]) > convergence)
						converged = false;
				}
				else if (!(i == 0 && j == 2)) {
					utils[i][j] = -1 + gamma * q3findMax(prevUtils, direction, i, j);
					if (abs(utils[i][j] - prevUtils[i][j]) > convergence)
						converged = false;
				}
			}
		}
		//update prevUtils
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				prevUtils[i][j] = tempUtils[i][j];
		}
	}
}

void valueIterationWithUtility(double Rs, vector<vector<int> >& direction, vector<vector<double> >& utilities) {
	vector<vector<double> > utils(3, vector<double>(4,0));	
	vector<vector<double> > prevUtils(3, vector<double>(4,0));
	vector<vector<double> > tempUtils(3, vector<double>(4,0));
	//set the terminal states
	utils[0][3] = 1;
	utils[1][3] = -1;
	prevUtils[0][3] = 1;
	prevUtils[1][3] = -1;
	
	//value to test when the utilities have converged
	double convergence = 0.0000001;
	
	bool converged = false;
	while (!converged) {
		converged = true;
		//save all utilities
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++)
				tempUtils[i][j] = utils[i][j];
		}
		//iterate through all states except for the terminal states and non-traversable state
		//update utility and check for convergence
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				if (!((i == 0 && j == 3) || (i == 1 && j == 3) || (i == 1 && j == 1))) {
					utils[i][j] = Rs + findMax(prevUtils, direction, i, j);
					if (abs(utils[i][j] - prevUtils[i][j]) > convergence)
						converged = false;
				}
			}
		}
		//update prevUtils
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++)
				prevUtils[i][j] = tempUtils[i][j];
		}
	}
	for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++)
				utilities[i][j] = utils[i][j];
	}
}

void q3valueIterationWithUtility(double gamma, vector<vector<int> >& direction, vector<vector<double> >& utilities) {
	vector<vector<double> > utils(3, vector<double>(3,0));	
	vector<vector<double> > prevUtils(3, vector<double>(3,0));
	vector<vector<double> > tempUtils(3, vector<double>(3,0));
	//set the terminal states
	utils[0][2] = 10;
	prevUtils[0][2] = 10;
	
	//value to test when the utilities have converged
	double convergence = 0.0000001;
	
	bool converged = false;
	while (!converged) {
		converged = true;
		//save all utilities
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				tempUtils[i][j] = utils[i][j];
		}
		//iterate through all states except for the terminal states and non-traversable state
		//update utility and check for convergence
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (i == 0 && j == 0) {
					utils[i][j] = 3 + gamma * q3findMax(prevUtils, direction, i, j);
					if (abs(utils[i][j] - prevUtils[i][j]) > convergence)
						converged = false;
				}
				else if (!(i == 0 && j == 2)) {
					utils[i][j] = -1 + gamma * q3findMax(prevUtils, direction, i, j);
					if (abs(utils[i][j] - prevUtils[i][j]) > convergence)
						converged = false;
				}
			}
		}
		//update prevUtils
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				prevUtils[i][j] = tempUtils[i][j];
		}
	}
	for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				utilities[i][j] = utils[i][j];
	}
}


void monteCarlo(int numIterations) {
	vector<vector<int> > monteCarloDirection(3, vector<int>(4,0));
	vector<vector<double> > monteCarloUtility(3, vector<double>(4,0));
	
	//call value iteration to generate direction map
	valueIterationWithUtility(-0.04, monteCarloDirection, monteCarloUtility);
	
	ofstream outfile;
	string outstring = "generated/P2-data-" + to_string(numIterations) + ".txt";
	outfile.open(outstring);
	
	for (int i = 0; i < numIterations; i++) {
		bool terminated = false;
		double score = 0;
		int agentX = 2;
		int agentY = 3;
		while (!terminated)
			terminated = takeStep(monteCarloDirection, agentX, agentY, score);
		outfile << score << "\n";
	}
	outfile.close();
}


bool takeStep (vector<vector<int> >& monteCarloDirection, int &agentX, int &agentY, double &score) {

	int random = rand() % 10;
	int attemptedAction = monteCarloDirection[agentX][agentY];
	int actualAction;
	if (attemptedAction == 0) {
		if (random == 0)
			actualAction = 3;
		else if (random == 1)
			actualAction = 1;
		else
			actualAction = attemptedAction;
	}
	if (attemptedAction == 3) {
		if (random == 0)
			actualAction = 2;
		else if (random == 1)
			actualAction = 0;
		else
			actualAction = attemptedAction;
	}
	else {
		if (random == 0)
			actualAction = attemptedAction - 1;
		else if (random == 1)
			actualAction = attemptedAction + 1;
		else
			actualAction = attemptedAction;
	}
		
	if (agentX == 0 && agentY == 0) {
		if (actualAction == 0)
			score -= 0.04;
		else if (actualAction == 1)
			score -= 0.04;
		else if (actualAction == 2) {
			agentY = 1;
			score -= 0.04;
		}
		else if (actualAction == 3) {
			agentX = 1;
			score -= 0.04;
		}
		return false;
	}
	else if (agentX == 0 && agentY == 1) {
		if (actualAction == 0) {
			agentY = 0;
			score -= 0.04;
		}
		else if (actualAction == 1)
			score -= 0.04;
		else if (actualAction == 2) {
			agentY = 2;
			score -= 0.04;
		}
		else if (actualAction == 3) {
			score -= 0.04;
		}
		return false;
	}
	else if (agentX == 0 && agentY == 2) {
		if (actualAction == 0) {
			agentY = 1;
			score -= 0.04;
		}
		else if (actualAction == 1)
			score -= 0.04;
		else if (actualAction == 2) {
			agentY = 3;
			score += 1;
			return true;
		}
		else if (actualAction == 3) {
			agentX = 1;
			score -= 0.04;
		}
		return false;
	}
	else if (agentX == 1 && agentY == 0) {
		if (actualAction == 0)
			score -= 0.04;
		else if (actualAction == 1) {
			agentX = 0;
			score -= 0.04;
		}
		else if (actualAction == 2) {
			score -= 0.04;
		}
		else if (actualAction == 3) {
			agentX = 2;
			score -= 0.04;
		}
		return false;
	}
	else if (agentX == 1 && agentY == 2) {
		if (actualAction == 0)
			score -= 0.04;
		else if (actualAction == 1) {
			agentX = 0;
			score -= 0.04;
		}
		else if (actualAction == 2) {
			agentY = 3;
			score -= 1;
			return true;
		}
		else if (actualAction == 3) {
			agentX = 2;
			score -= 0.04;
		}
		return false;
	}
	else if (agentX == 2 && agentY == 0) {
		if (actualAction == 0) {
			score -= 0.04;
		}
		else if (actualAction == 1) {
			agentX = 1;
			score -= 0.04;
		}
		else if (actualAction == 2) {
			agentY = 1;
			score -= 0.04;
		}
		else if (actualAction == 3)
			score -= 0.04;
		return false;
	}
	else if (agentX == 2 && agentY == 1) {
		if (actualAction == 0) {
			agentY = 0;
			score -= 0.04;
		}
		else if (actualAction == 1)
			score -= 0.04;
		else if (actualAction == 2) {
			agentY = 2;
			score -= 0.04;
		}
		else if (actualAction == 3)
			score -= 0.04;
		return false;
	}
	else if (agentX == 2 && agentY == 2) {
		if (actualAction == 0) {
			agentY = 1;
			score -= 0.04;
		}
		else if (actualAction == 1) {
			agentX = 1;
			score -= 0.04;
		}
		else if (actualAction == 2) {
			agentY = 3;
			score -= 0.04;
		}
		else if (actualAction == 3) {
			score -= 0.04;
		}
		return false;
	}
	else if (agentX == 2 && agentY == 3) {
		if (actualAction == 0) {
			agentY = 2;
			score -= 0.04;
		}
		else if (actualAction == 1) {
			agentX = 1;
			score -= 1;
			return true;
		}
		else if (actualAction == 2) {
			score -= 0.04;
		}
		else if (actualAction == 3) {
			score -= 0.04;
		}
		return false;
	}	
}




















