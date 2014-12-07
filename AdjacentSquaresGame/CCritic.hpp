#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <sstream>
#include "annet.hpp"

using namespace std;

class CCritic
{
public:
	struct Position {
		bool isDown;
		int row;
		int col;	
		
		void toString();	
	};
	struct Values {
		bool isDown;
		vector<double> vals;
		
		void toString();
	};

private:
	fstream t1in, t2in;	//trace input
	fstream v1in, v2in; //values input

	vector<double> down_weights;
	vector<double> up_weights;
	vector<vector<double> > p1_scores;
	vector<vector<double> > p2_scores;
	vector<double> p1_targetVals;
	vector<double> p2_targetVals;
	
	vector<Position> p1MoveTrace;
	vector<Position> p2MoveTrace;
	vector<Values> p1ValueTrace; 
	vector<Values> p2ValueTrace; 
	
	annet move_ann;
	annet pickup_ann;
	
public:
	CCritic();
	void endCritic();
	
	int getTrace(vector<Position> &trace, fstream &fin, int player);
	int getValues(vector<Values> &trace, fstream &fin);	
	
	int getScores(vector<vector<double> > &svec, string filename);
	int getWeights(vector<double> &w, string filename);
	
	void recreate_ann(vector<double> &weights, annet &ann);
	
	void execute_backProp();
	void outputWeights(annet &ann, string filename);
	
	string toString();
};

CCritic::CCritic()
{
	t1in.open("trace1.txt", fstream::in);
	t2in.open("trace2.txt", fstream::in);
	v1in.open("values1.txt", fstream::in);
	v2in.open("values2.txt", fstream::in);	
	
	getTrace(p1MoveTrace, t1in, 1);
	getTrace(p2MoveTrace, t2in, 2);
	
	getValues(p1ValueTrace, v1in);
	getValues(p2ValueTrace, v2in);
	
	getScores(p1_scores, "scores1.txt");
	getScores(p2_scores, "scores2.txt");
	
	getWeights(down_weights, "placementWeights.txt");
	getWeights(up_weights, "pickUpWeights.txt");
	
	move_ann.setUp(3,2,1);
	pickup_ann.setUp(3,2,1);
	
	//reconstruct the game's neural networks
	recreate_ann(down_weights, move_ann);
	recreate_ann(up_weights, pickup_ann);
}

void CCritic::endCritic()
{
	t1in.close();
	t2in.close();
	v1in.close();
	v2in.close();
}

int CCritic::getTrace(vector<Position> &trace, fstream &fin, int player) 
{
	Position tempPos;
	string lineBuffer;
	string action, rowStr, colStr;	
	int startPos, length;
	
	vector<double> *vals;
	
	if (player == 1) {
		vals = &p1_targetVals;
	}
	else if (player == 2) {
		vals = &p2_targetVals;
	}
	
	while (getline(fin, lineBuffer)) {
		if (lineBuffer[0] == 'W') {
			vals->push_back(1);
			break;
		}
		else if (lineBuffer[0] == 'L')  {
			vals->push_back(0);
			break;
		}
		else if (lineBuffer[0] == 'D') {
			vals->push_back(0.5);
			break;
		}
		
		/*
		startPos = 0;
		length = lineBuffer.find(":");
		action = lineBuffer.substr(startPos, length);
		
		startPos = length + 1;
		length = lineBuffer.find(",") - startPos;
		rowStr = lineBuffer.substr(startPos, length);
		
		startPos = startPos + length + 1;
		colStr = lineBuffer.substr(startPos);
				
		if (action == "d") {
			tempPos.isDown = true;
		}
		else if (action == "u") {
			tempPos.isDown = false;
		}
		
		tempPos.row = strtod(rowStr.c_str(), NULL);
		tempPos.col = strtod(colStr.c_str(), NULL);
		
		trace.push_back(tempPos);*/
	}
	
	return 0;
}

int CCritic::getValues(vector<Values> &trace, fstream &fin)
{
	Values tempValues;
	string lineBuffer, buffer, line;
	string action;
	int startPos, length;
	double dbuf;
	
	while (getline(fin, lineBuffer)) {
		startPos = 0;
		length = lineBuffer.find(":");
		action = lineBuffer.substr(startPos, length);
		line = lineBuffer.substr(length+1);
		
		if (action == "d") {
			tempValues.isDown = true;
		}
		else if (action == "u") {
			tempValues.isDown = false;
		}
		
		istringstream iss(line);
		while (getline(iss, buffer, ',')) {
			dbuf = strtod(buffer.c_str(), NULL);
			tempValues.vals.push_back(dbuf);
		}
		trace.push_back(tempValues);
		
		tempValues.vals.clear();
	}

	return 0;
}

int CCritic::getScores(vector<vector<double> > &svec, string filename)
{
	string lineBuffer, buffer, line;
	string action;
	int startPos, length;
	double dbuf;
	vector <double> tempVec;
	
	fstream fin(filename.c_str(), fstream::in);
	
	while (getline(fin, lineBuffer)) {
		startPos = 0;
		length = lineBuffer.find(":");
		action = lineBuffer.substr(startPos, length);
		line = lineBuffer.substr(length+1);
		
		istringstream iss(line);
		while (getline(iss, buffer, ',')) {
			dbuf = strtod(buffer.c_str(), NULL);
			tempVec.push_back(dbuf);
		}
		svec.push_back(tempVec);
		
		tempVec.clear();
	}

	fin.close();
	return 0;
}

int CCritic::getWeights(vector<double> &w, string filename)
{
	string buffer;
	double dbuf;
	fstream fin(filename.c_str(), fstream::in);
	
	w.clear();
	while (getline(fin, buffer)) {
		dbuf = strtod(buffer.c_str(), NULL);
		w.push_back(dbuf);
	}
	
	fin.close();
	return 0;
}

void CCritic::recreate_ann(vector<double> &weights, annet &ann)
{
	vector<vector<weight> > inWeights;
	vector<vector<weight> > outWeights;
	vector<weight> wVec;
	weight w;
	
	for (int i=0; i < weights.size()-2; ++i) {
		w.value = weights[i];
		wVec.push_back(w);
	}
	inWeights.push_back(wVec);
	wVec.clear();	
	
	for (int i=weights.size()-2; i < weights.size(); ++i) {
		w.value = weights[i];
		wVec.push_back(w);
	}
	outWeights.push_back(wVec);
	
	ann.setWeights(inWeights, outWeights);	//create the neural network	
}

void CCritic::execute_backProp()
{
	vector<double> inputVals;
	vector<double> hiddenVals;
	vector<double> outputVals;
	double avgErr;
	
	if (p1_scores.size() != p1ValueTrace.size()) {
		cout << p1_scores.size() << " - " << p1ValueTrace.size() << endl;
		return;
	}
	if (p2_scores.size() != p2ValueTrace.size()) {
		cout << p2_scores.size() << " - " << p2ValueTrace.size() << endl;
		return;
	}
	
	move_ann.setLearnRate(.05);
	pickup_ann.setLearnRate(.05);
	
	//ADJUST WEIGHTS FOR PLAYER ONE TRACE
	for (int i=0; i < p1ValueTrace.size(); ++i) {
		for (int j=0; j < p1ValueTrace[i].vals.size(); ++j) {
			inputVals.push_back(p1ValueTrace[i].vals[0]);
		}
		for (int j=0; j < p1_scores[i].size()-1; ++j) {
			hiddenVals.push_back(p1_scores[i][j]);
		}
		outputVals.push_back(p1_scores[i][p1_scores[i].size()-1]);
		
		if (p1ValueTrace[i].isDown) {
			move_ann.setLayerValues(inputVals, hiddenVals, outputVals);
			move_ann.calculateError(p1_targetVals, avgErr); //calculate error
			move_ann.adjustWeights(false); //adjust weights
		}
		else {
			pickup_ann.setLayerValues(inputVals, hiddenVals, outputVals);
			pickup_ann.calculateError(p1_targetVals, avgErr); //calculate error
			pickup_ann.adjustWeights(false); //adjust weights
		}
		
		inputVals.clear();
		hiddenVals.clear();
		outputVals.clear();
	}
	
	//ADJUST WEIGHTS FOR PLAYER TWO TRACE
	for (int i=0; i < p2ValueTrace.size(); ++i) {
		for (int j=0; j < p2ValueTrace[i].vals.size(); ++j) {
			inputVals.push_back(p2ValueTrace[i].vals[0]);
		}
		for (int j=0; j < p2_scores[i].size()-1; ++j) {
			hiddenVals.push_back(p2_scores[i][j]);
		}
		outputVals.push_back(p2_scores[i][p2_scores[i].size()-1]);
		
		if (p2ValueTrace[i].isDown) {
			move_ann.setLayerValues(inputVals, hiddenVals, outputVals);
			move_ann.calculateError(p2_targetVals, avgErr); //calculate error
			move_ann.adjustWeights(false); //adjust weights
		}
		else {
			pickup_ann.setLayerValues(inputVals, hiddenVals, outputVals);
			pickup_ann.calculateError(p2_targetVals, avgErr); //calculate error
			pickup_ann.adjustWeights(false); //adjust weights
		}
		
		inputVals.clear();
		hiddenVals.clear();
		outputVals.clear();
	}

	outputWeights(move_ann, "placementWeights.txt");
	outputWeights(pickup_ann, "pickUpWeights.txt");
}

void CCritic::outputWeights(annet &ann, string filename)
{
	fstream fout(filename.c_str(), fstream::out);
	vector<double> in_weights;
	vector<double> out_weights;
	
	ann.getWeights(in_weights, out_weights);

	for (int i=0; i < in_weights.size(); ++i) {
		fout << in_weights[i] << endl;
	}
	for (int i=0; i < out_weights.size(); ++i) {
		fout << out_weights[i] << endl;
	}
	
	fout.close();
}

string CCritic::toString()
{	
	cout << "tv1: " << endl;
	for (int i=0; i < p1ValueTrace.size(); ++i) {
		p1ValueTrace[i].toString();
	}
	
	cout << "tv2: " << endl;
	for (int i=0; i < p2ValueTrace.size(); ++i) {
		p2ValueTrace[i].toString();
	}
	
	return "";
}

void CCritic::Position::toString()
{
	if (isDown) {
		cout << "Down: ";
	}
	else {
		cout << "Up: ";
	}
	
	cout << row << ", " << col << endl;
}

void CCritic::Values::toString()
{
	if (isDown) {
		cout << "Down: ";
	}
	else {
		cout << "Up: ";
	}
	
	for (int i=0; i < vals.size(); ++i) {
		cout << vals[i] << " "; 
	}
	
	cout << endl;
		
}
