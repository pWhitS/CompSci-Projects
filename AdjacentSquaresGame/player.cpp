#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include "CGame.hpp"

using namespace std;

int getInput(vector<int> &input)
{
	string buffer;
	int ibuf;
	
	while (getline(cin, buffer)) {
		ibuf = strtol(buffer.c_str(), NULL, 10);
		input.push_back(ibuf);
	}
	
	return input[0]; //return the player
}

void parsePositions(const vector<int> &pos, vector<int> &p1, vector<int> &p2)
{
	int val;
	
	for (int i=1; i < 25; ++i) {
		if (i < 13) {
			p1.push_back(pos[i]);
		}
		else {
			p2.push_back(pos[i]);
		}
	}
}

void outputMove(string filename)
{
	fstream fin(filename.c_str(), fstream::in);
	string lineBuffer;
	vector<string> moves;
	
	while (getline(fin, lineBuffer)) {
		moves.push_back(lineBuffer);
	}

	if (moves.size() > 1) {
		if (moves[moves.size()-2][0] == 'u') {
			cout << moves[moves.size()-2] << endl;
		}
	}

	cout << moves[moves.size()-1] << endl;
}

int main()
{
	cout << "Content-Type: text/html\n\n" << endl;
	
	CGame cg;
	
	int player;
	vector<int> positions;
	vector<int> p1Pos;
	vector<int> p2Pos;
	
	player = getInput(positions);
	parsePositions(positions, p1Pos, p2Pos);
	
	cg.modifyBoard(p1Pos, p2Pos);
			
	if (player == 1) {
		cg.p2Move();
		outputMove("trace2.txt");
	}
	else if (player == 2) {
		cg.p1Move();
		outputMove("trace1.txt");
	}	
	
	if (cg.isWinner()) {
		cout << "WINNER" << endl;
	}
}