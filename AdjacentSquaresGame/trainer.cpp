#include <iostream>
#include "CGame.hpp"
#include "CCritic.hpp"

using namespace std;

void playGame()
{
	CGame cg;
	int winner, count = 0;
	
	cg.firstMove();
	cg.p2Move();
	
	while (cg.getNumTiles(1) && cg.getNumTiles(2) && !cg.isWinner()) {
		++count;
		cg.p1Move();
		if ((winner = cg.isWinner())) {
			break;
		}
		cg.p2Move();
	}
	
	if (!winner) {	
		while (!(winner = cg.isWinner()) && (count < 100)) {
			++count;
			cg.p1Move();
			if ((winner = cg.isWinner())) {
				break;
			}
			cg.p2Move();		
		}
	}
	
	if (!winner) {
		cout << "Draw... no winner found after " << count * 2 << " moves." << endl;
	}
	else {
		cout << "Player" << winner << " Wins!" << endl;
		cout << "Moves: " << count << endl;
	}
	
	cg.endGame();
}

void criticize()
{
	CCritic cc;
	cc.execute_backProp();	
	cc.endCritic();
}

//This function clears, closes, and reopens all files to prevent formatting errors in the files//
int failSafe()
{
	fstream fout; //will be used to clear out all files, except weights
	
	//clear scores
	fout.open("scores1.txt", fstream::out);
	fout.close();
	fout.open("scores2.txt", fstream::out);
	fout.close();
	
	//clear traces
	fout.open("trace1.txt", fstream::out);
	fout.close();
	fout.open("trace2.txt", fstream::out);
	fout.close();
	
	//clear values
	fout.open("values1.txt", fstream::out);
	fout.close();
	fout.open("values2.txt", fstream::out);
	fout.close();
	
	return 0;
}

int main()
{
	for (int i=0; i < 10000; ++i) {
		playGame();
		criticize();
	
		if (failSafe()) {
			cout << "fail safe tripped" << endl;
			break;
		}
	}
	
	return 0;
}