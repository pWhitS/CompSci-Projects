#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "annet.hpp"

using namespace std;

#define EMPTY_TILE 0
#define P1_TILE 1
#define P2_TILE 2

class CGame 
{
public:
	struct Position {
		int row;
		int col;
		
		void setPosition(int _row, int _col);
		bool equals(Position pos);
		
		string itostr(int n);
		string toString();
	};
	
private:	
	struct Board {
		int id[23][23];
		int tiles[23][23];
	};
		
	Board gameBoard;
	int p1_tiles;
	int p2_tiles;
	
	vector<double> tile_weights;
	vector<double> pickUp_weights;
	
	fstream s1out; //player1 placement and pick up score traces
	fstream s2out; //player2 placement and pick up score traces
	fstream t1out; //player1 move trace
	fstream t2out; //placer2 move trace
	fstream v1out; //player1 values
	fstream v2out; //player2 values
	
	string itostr(int n);

	annet move_ann;
	annet pickup_ann;

public:
	CGame();	
	void endGame();
	
	int id_to_rowcol(int id, int &row, int &col);
	void modifyBoard(vector<int> player1, vector<int> player2);
	
	int getWeights(vector<double> &w, string filename);
	
	int firstMove();
	int p1Move();
	int p2Move();
	
	int hasTilesMove(int player, Position &moveTo);
	int noTilesMove(int player, Position &moveTo);
	
	void init_ann(const vector<double> &tw, const vector<double> &pw);
	double scoreMove(Position move, int player);
	double scorePickUps(Position pickUp, int player);
	
	void outputScore(int player, Position pos, bool isUp);
	
	int getNumConsecutive(Position pos, int player);
	int getNumAdjacents(Position pos, bool withCorners);

	int getPossibleMoves(vector<Position> &pMoves);	
	int getPossiblePickUps(vector<Position> &pPickUps, int player);
	int cleanMoves(vector<Position> &moves);
	int emptyAdjacents(Position pos);
	
	bool isOccupied(Position pos);
	bool isOccupied(Position pos, int player); 
	int getNumTiles(int player);
	
	int isWinner();
	void noWinner();
	
	string toString();
};

CGame::CGame()
{
	int count = 1;
	for (int i=0; i < 23; ++i) {
		for (int j=0; j < 23; ++j) {
			gameBoard.id[i][j] = count++;	
			gameBoard.tiles[i][j] = EMPTY_TILE;
		}
	}
	
	p1_tiles = 12;
	p2_tiles = 12;	
	
	getWeights(tile_weights, "placementWeights.txt");
	getWeights(pickUp_weights, "pickUpWeights.txt");
	
	t1out.open("trace1.txt", fstream::out);
	t2out.open("trace2.txt", fstream::out);
	s1out.open("scores1.txt", fstream::out);
	s2out.open("scores2.txt", fstream::out);
	v1out.open("values1.txt", fstream::out);
	v2out.open("values2.txt", fstream::out);		
		
	move_ann.setUp(3,2,1);
	pickup_ann.setUp(3,2,1);	
	
	init_ann(tile_weights, pickUp_weights);
}

//converts a tile's id number to it's row and column numbers//
int CGame::id_to_rowcol(int id, int &row, int &col)
{
	for (int i=0; i < 23; ++i) {
		for (int j=0; j < 23; ++j) {
			if (gameBoard.id[i][j] == id) {
				row = i;
				col = j;
				return 1;
			}
		}
	}
	return 0;
}

void CGame::modifyBoard(vector<int> player1, vector<int> player2)
{
	int r, c;

	for (int i=0; i < 23; ++i) {
		for (int j=0; j < 23; ++j) {
			gameBoard.tiles[i][j] = EMPTY_TILE;
		}
	}

	for (int i=0; i < player1.size(); ++i) {
		if (player1[i] == -1) {
			continue;
		}
		id_to_rowcol(player1[i], r, c);
		gameBoard.tiles[r][c] = P1_TILE;
		
		if (p1_tiles > 0) {
			--p1_tiles;
		}
	}
	
	for (int i=0; i < player2.size(); ++i) {
		if (player2[i] == -1) {
			continue;
		}
		id_to_rowcol(player2[i], r, c);
		gameBoard.tiles[r][c] = P2_TILE;
		
		if (p2_tiles > 0) {
			--p2_tiles;
		}
	}	
}

void CGame::endGame()
{
	t1out.close();
	t2out.close();
	s1out.close();
	s2out.close();
}

int CGame::getWeights(vector<double> &w, string filename)
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

int CGame::firstMove()
{
	gameBoard.tiles[11][11] = P1_TILE;
	--p1_tiles; 
	
	t1out << "d:11,11" << endl; 
	
	return 0;
}

int CGame::p1Move()
{
	Position moveTo;
	
	if (p1_tiles > 0) { //player one still has tiles
		hasTilesMove(1, moveTo);
		gameBoard.tiles[moveTo.row][moveTo.col] = P1_TILE;
		t1out << "d:" << moveTo.row << "," << moveTo.col << endl; //output move trace
		
		return --p1_tiles; //decrease, then return 
	}
	
	if (noTilesMove(1, moveTo)) { //pick up tile, then place it elsewhere
		gameBoard.tiles[moveTo.row][moveTo.col] = P1_TILE;
		t1out << "d:" << moveTo.row << "," << moveTo.col << endl; //output move trace
	}
	
	return p1_tiles;
}

int CGame::p2Move()
{
	Position moveTo;
	
	if (p2_tiles > 0) { //player one still has tiles;
		hasTilesMove(2, moveTo);
		gameBoard.tiles[moveTo.row][moveTo.col] = P2_TILE;
		t2out << "d:" << moveTo.row << "," << moveTo.col << endl; //output move trace
		
		return --p2_tiles; //decrease, then return 
	}
	
	if (noTilesMove(2, moveTo)) { //pick up a tile and place it elsewhere
		gameBoard.tiles[moveTo.row][moveTo.col] = P2_TILE;
		t2out << "d:" << moveTo.row << "," << moveTo.col << endl; //output move trace
	}
	
	return p2_tiles;
}

int CGame::hasTilesMove(int player, Position &moveTo)
{
	vector<Position> possibleMoves;
	double score, topScore;
	double avgError;	
	Position bestMove;
	
	getPossibleMoves(possibleMoves);

	topScore = scoreMove(possibleMoves[0], player);
	bestMove = possibleMoves[0];
	for (int i=1; i < possibleMoves.size(); ++i) {
		score = scoreMove(possibleMoves[i], player);
		
		if (score > topScore) {
			topScore = score;
			bestMove = possibleMoves[i];
		}
	}
	moveTo.setPosition(bestMove.row, bestMove.col);
	
	outputScore(player, moveTo, false); //output to score trace
	
	return topScore;
}

int CGame::noTilesMove(int player, Position &moveTo)
{
	vector<Position> possiblePickUps;
	Position bestPickUp;
	int score, topScore;
	
	getPossiblePickUps(possiblePickUps, player);
	
	if (possiblePickUps.size() < 1) {
		return 0;		
	}
	
	topScore = scorePickUps(possiblePickUps[0], player);
	bestPickUp = possiblePickUps[0];
	for (int i=1; i < possiblePickUps.size(); ++i) {
		score = scorePickUps(possiblePickUps[i], player);
		
		if (score < topScore) {
			topScore = score;
			bestPickUp = possiblePickUps[i];
		}
	}
	
	outputScore(player, bestPickUp, true); //output to score trace
	
	gameBoard.tiles[bestPickUp.row][bestPickUp.col] = EMPTY_TILE; //pick up tile from the board
	hasTilesMove(player, moveTo); //find new placement for tile
	
	if (player == P1_TILE) {
		t1out << "u:" << bestPickUp.row << "," << bestPickUp.col << endl; //output pick up trace
	}
	else if (player == P2_TILE) {
		t2out << "u:" << bestPickUp.row << "," << bestPickUp.col << endl; //output pick up trace
	}
	
	return possiblePickUps.size();
}

void CGame::init_ann(const vector<double> &tw, const vector<double> &pw)
{
	vector<vector<weight> > inWeights;
	vector<vector<weight> > outWeights;
	vector<weight> wVec;
	weight w;
	
	//tile weights
	for (int i=0; i < tw.size()-2; ++i) {
		w.value = tw[i];
		wVec.push_back(w);
	}
	inWeights.push_back(wVec);
	wVec.clear();	
	
	for (int i=tw.size()-2; i < tw.size(); ++i) {
		w.value = tw[i];
		wVec.push_back(w);
	}
	outWeights.push_back(wVec);
	move_ann.setWeights(inWeights, outWeights);
		
	wVec.clear();
	inWeights.clear();
	outWeights.clear();
	
	//placement weights
	for (int i=0; i < pw.size()-2; ++i) {
		w.value = pw[i];
		wVec.push_back(w);
	}
	inWeights.push_back(wVec);
	wVec.clear();
	
	for (int i=pw.size()-2; i < pw.size(); ++i) {
		w.value = pw[i];
		wVec.push_back(w);
	}
	outWeights.push_back(wVec);
	pickup_ann.setWeights(inWeights, outWeights);	
}

double CGame::scoreMove(Position move, int player)
{
	double score;
	double avgError;
		
	vector<double> inputsVec;	
	vector<double> hiddenVals;
	vector<double> outputVals;
	
	int numAdj = getNumAdjacents(move, true); //get move's adjacent and corner pieces
	int numOwnCon = getNumConsecutive(move, player); //number of own pieces in a row
	int numOppCon; //number of opponent pieces in row

	if (player == 1) {
		numOppCon = getNumConsecutive(move, 2);
	}
	else if (player == 2) {
		numOppCon = getNumConsecutive(move, 1);
	}
	
	inputsVec.push_back(numAdj);
	inputsVec.push_back(numOwnCon);
	inputsVec.push_back(numOppCon);

	move_ann.feedForward(inputsVec, false);
	move_ann.getLayerValues(hiddenVals, outputVals);
	score = outputVals[0];

	return score; //returns the maxScore
}

double CGame::scorePickUps(Position pickUp, int player)
{
	double score = 0;
	score = scoreMove(pickUp, player);	
	return score;
}

void CGame::outputScore(int player, Position move, bool isUp)
{
	vector<double> hiddenVals;
	vector<double> outputVals;
	string prefix;
		
	if (isUp) {
		prefix = "u:";
	}
	else {
		prefix = "d:";
	}
	
	int numAdj = getNumAdjacents(move, true); //get move's adjacent and corner pieces
	int numOwnCon = getNumConsecutive(move, player); //number of own pieces in a row
	int numOppCon; //number of opponent pieces in row
	
	double score = scoreMove(move, player);
	move_ann.getLayerValues(hiddenVals, outputVals);
	
	if (player == 1) {
		numOppCon = getNumConsecutive(move, 2);
		
		v1out << prefix << numAdj << ",";
		v1out << numOwnCon << ",";
		v1out << numOppCon << endl;
		
		s1out << prefix;
		for (int i=0; i < hiddenVals.size(); ++i) {
			s1out << hiddenVals[i] << ",";
		}
		s1out << score << endl;
	}
	else if (player == 2) {
		numOppCon = getNumConsecutive(move, 1);
				
		v2out << prefix << numAdj << ",";
		v2out << numOwnCon << ",";
		v2out << numOppCon << endl;
		
		s2out << prefix;
		for (int i=0; i < hiddenVals.size(); ++i) {
			s2out << hiddenVals[i] << ",";
		}
		s2out << score << endl;
	}
}

int CGame::getNumConsecutive(Position pos, int player)
{
	Position tPos;
	int tempPlayer;
	int numConsecutive = 0;
	int topConsecutive = 0;
		
	for (int i=0; i < 4; ++i) { //find consecutive tiles
		switch (i) {
		case 0: //up and down
			tPos.setPosition(pos.row - 1, pos.col); 
			while (isOccupied(tPos, player)) {
				++numConsecutive;
				tPos.setPosition(tPos.row - 1, tPos.col); //continue moving up
			}
			tPos.setPosition(pos.row + 1, pos.col);
			while (isOccupied(tPos, player)) {
				++numConsecutive;
				tPos.setPosition(tPos.row + 1, tPos.col); //continue moving down
			}
			break;
		case 1: //left and right
			tPos.setPosition(pos.row, pos.col - 1);
			while (isOccupied(tPos, player)) {
				++numConsecutive;
				tPos.setPosition(tPos.row, tPos.col - 1); //continue moving left
			}
			tPos.setPosition(pos.row, pos.col + 1);
			while (isOccupied(tPos, player)) {
				++numConsecutive;
				tPos.setPosition(tPos.row, tPos.col + 1); //continue moving right
			}
			break;
		case 2: //up-left and down-right
			tPos.setPosition(pos.row - 1, pos.col - 1);
			while (isOccupied(tPos, player)) {
				++numConsecutive;
				tPos.setPosition(tPos.row - 1, tPos.col - 1); //continue moving up-left
			}
			tPos.setPosition(pos.row + 1, pos.col + 1);
			while (isOccupied(tPos, player)) {
				++numConsecutive;
				tPos.setPosition(tPos.row + 1, tPos.col + 1); //continue moving down-right
			}
			break;
		case 3: //up-right and down-left
			tPos.setPosition(pos.row - 1, pos.col + 1);
			while (isOccupied(tPos, player)) {
				++numConsecutive;
				tPos.setPosition(tPos.row - 1, tPos.col + 1); //continue moving up-right
			}
			tPos.setPosition(pos.row + 1, pos.col - 1);
			while (isOccupied(tPos, player)) {
				++numConsecutive;
				tPos.setPosition(tPos.row + 1, tPos.col - 1); //continue moving down-left
			}
			break;
		}
		
		if (numConsecutive > topConsecutive) {
			topConsecutive = numConsecutive;
		}
		numConsecutive = 0;
	}
	
	return topConsecutive;
}

int CGame::getNumAdjacents(Position pos, bool withCorners)
{
	Position tempPos[8];
	int max = 4;
	int numAdjacents = 0;
	
	tempPos[0].setPosition(pos.row - 1, pos.col); //up
	tempPos[1].setPosition(pos.row + 1, pos.col); //down
	tempPos[2].setPosition(pos.row, pos.col - 1); //left
	tempPos[3].setPosition(pos.row, pos.col + 1); //right
	
	if (withCorners) {
		tempPos[4].setPosition(pos.row - 1, pos.col - 1); //up-left
		tempPos[5].setPosition(pos.row - 1, pos.col + 1); //up-right
		tempPos[6].setPosition(pos.row + 1, pos.col - 1); //down-left
		tempPos[7].setPosition(pos.row + 1, pos.col + 1); //down-right
		max = 8;
	}
	
	for (int i=0; i < max; ++i) {
		if (isOccupied(tempPos[i])) {
			++numAdjacents;
		}
	}
	
	return numAdjacents;
}

int CGame::getPossibleMoves(vector<Position> &pMoves)
{
	Position curPos, tempPos;
	bool reset = false;
	
	for (int i=0; i < 23; ++i) {
		for (int j=0; j < 23; ++j) {
			curPos.setPosition(i, j);
			if (isOccupied(curPos)) {
				tempPos.setPosition(curPos.row - 1, curPos.col); //up
				pMoves.push_back(tempPos);
				
				tempPos.setPosition(curPos.row + 1, curPos.col); //down
				pMoves.push_back(tempPos);
				
				tempPos.setPosition(curPos.row, curPos.col - 1); //left
				pMoves.push_back(tempPos);
				
				tempPos.setPosition(curPos.row, curPos.col + 1); //right
				pMoves.push_back(tempPos);				
			}
		}
	}
	//delete moves that are not legal
	cleanMoves(pMoves);
	
	return 0;
}

int CGame::getPossiblePickUps(vector<Position> &pPickUps, int player)
{
	Position tempPos;

	for (int i=0; i < 23; ++i) {
		for (int j=0; j < 23; ++j) {
			tempPos.setPosition(i, j);
			if (isOccupied(tempPos, player) && emptyAdjacents(tempPos) > 1) {	
				pPickUps.push_back(tempPos);
			}
		}
	}
	
	return 0;
}

int CGame::cleanMoves(vector<Position> &moves) 
{
	//delete occupied moves
	for (int i=0; i < moves.size(); ++i) {
		if (isOccupied(moves[i])) {
			moves[i] = moves[moves.size()-1];
			moves.pop_back();
			--i;
		}
	}
	//find and remove duplicate moves
	for (int i=0; i < moves.size(); ++i) {
		for (int j=0; j < moves.size(); ++j) {
			if (i == j) {
				continue;
			}
			if (moves[i].equals(moves[j])) {
				moves[i] = moves[moves.size()-1];
				moves.pop_back();
				j = -1;
			} 
		}
	}

	return 0;
}

int CGame::emptyAdjacents(Position pos)
{
	int count = 0;
	int topCount = 0;
	Position tempPos[4];
	
	tempPos[0].setPosition(pos.row - 1, pos.col); //up
	tempPos[1].setPosition(pos.row, pos.col - 1); //left
	tempPos[2].setPosition(pos.row + 1, pos.col); //down
	tempPos[3].setPosition(pos.row, pos.col + 1); //right
	
	for (int i=0; i < 4; ++i) {
		if (!isOccupied(tempPos[i])) {
			++count;
		}
		else {
			count = 0;
		}
		
		if (count > topCount) {
			topCount = count;
		}
	}
	
	tempPos[0].setPosition(pos.row - 1, pos.col); //up
	tempPos[1].setPosition(pos.row, pos.col + 1); //right
	tempPos[2].setPosition(pos.row + 1, pos.col); //down
	tempPos[3].setPosition(pos.row, pos.col - 1); //left
	
	count = 0;
	for (int i=0; i < 4; ++i) {
		if (!isOccupied(tempPos[i])) {
			++count;
		}
		else {
			count = 0;
		}
		
		if (count > topCount) {
			topCount = count;
		}
	}
	
	return topCount;
}

bool CGame::isOccupied(Position pos)
{
	if (gameBoard.tiles[pos.row][pos.col] != EMPTY_TILE) {
		return true;
	}
	return false;
}

bool CGame::isOccupied(Position pos, int player) 
{
	//returns true if occupied by a certain player's piece
	if (gameBoard.tiles[pos.row][pos.col] == player) {
		return true;
	}
	return false;
}

int CGame::getNumTiles(int player)
{
	int size = 0;
	
	if (player == P1_TILE) {
		size = p1_tiles;
	}
	else if (player == P2_TILE) {
		size = p2_tiles;
	}
	
	return size;
}

int CGame::isWinner()
{
	Position tempPos;
	int player1 = 0, player2 = 0;
	int winner = 0;

	for (int i=0; i < 23; ++i) {
		for (int j=0; j < 23; ++j) {
			tempPos.setPosition(i, j);
			if (isOccupied(tempPos, P1_TILE)) {
				player1 = getNumConsecutive(tempPos, 1) + 1;
			}
			else if (isOccupied(tempPos, P2_TILE)) {
				player2 = getNumConsecutive(tempPos, 2) + 1;
			}
			
			if (player1 == 5) {
				winner = 1;
				t1out << "WIN" << endl;
				t2out << "LOSE" << endl;
				break;
			}
			if (player2 == 5) {
				winner = 2;
				t2out << "WIN" << endl;
				t1out << "LOSE" << endl;
				break;
			}
		}
		if (winner) {
			break;
		}
	}

	return winner;
}

void CGame::noWinner()
{
	t1out << "DRAW" << endl;
	t2out << "DRAW" << endl;
}

string CGame::itostr(int n)
{
	ostringstream resultStr;
	resultStr << n;
	return resultStr.str();
}

string CGame::toString()
{
	string result = "Game Board: \n";
	
	result += "player 1 tiles: " + itostr(p1_tiles) + "\n";
	result += "player 2 tiles: " + itostr(p2_tiles) + "\n";
	
	for (int i=0; i < 23; ++i) {
		for (int j=0; j < 23; ++j) {
			result += itostr(gameBoard.tiles[i][j]) + " ";
		}
		result += " - " + itostr(i) + "\n";
	}	
	
	return result;
}

//---------POSITION METHODS-----------//
void CGame::Position::setPosition(int _row, int _col)
{
	row = _row;
	col = _col;	
}

bool CGame::Position::equals(Position pos)
{
	if (pos.row == row && pos.col == col) {
		return true; //positions are equal
	}
	return false;
}

string CGame::Position::itostr(int n)
{
	ostringstream resultStr;
	resultStr << n;
	return resultStr.str();
}

string CGame::Position::toString()
{
	string result = "row: ";
	result += itostr(row);
	result += " - col: ";
	result += itostr(col);
	
	return result;
}
//-------END POSITION METHODS-----------//