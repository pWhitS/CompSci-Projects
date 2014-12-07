#ifndef CCLAUSE_HPP
#define CCLAUSE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

class CClause 
{
	int cid; //Clause ID
	long long pos; //holds the positive propositions
	long long neg; //holds the negative propositions
	vector<int> rwVec; //tracks which clauses this clause has been resolved with

	string itostr(int n);
	vector<int> bitConvert(long long num);

public:
	int init(vector<int> &_clauseVec, int id); //creates the clause from a vector of int
	
	bool isTautology(); //returns true if clause is a tautology
	int check_rw(int id); //see if id has been resolve with 
	int getNumProps(long long num); //counts number of possible resolutions
	int resolve(CClause _clause, CClause &_newClause, int newId); //resolves two clauses
	
	void set_pos(long long _pos); //set this clause's positive propositions
	void set_neg(long long _neg); //set this clause's negative propositions
	long long get_pos(); //gets the positive propositions
	long long get_neg(); //gets the negative propositions
	
	void set_cid(int id); //set this clause's id
	int get_cid(); //get this clause's id
	void add_rw_clause(int id);
		
	bool isEmpty(); //checks if clause is empty, a.k.a. if any propositions are marked
	bool isUnit(); //checks if clause is a unit clause
	int get_prop(); //only works for unit clauses
	
	int compare(CClause clause); //compares two clauses, equal - not equal
	void clear(); //empties the clause
	string toString(); //outputs bit representation of clause
	string toString(int dummy); //outputs numeric representation of clause
};

int CClause::init(vector<int> &_clauseVec, int id)
{	
	clear(); //empty the clause
	set_cid(id);
	
	for (int i=0; i < _clauseVec.size(); ++i) { 
		if (_clauseVec[i] > 0) { //number is positive
			pos += 1 << (_clauseVec[i]-1); //shift to the right _clauseVec[i]-1 number of times
		}
		else { // < 0, i.e. the number is negative
			neg += 1 << ((_clauseVec[i] * -1) - 1);
		}
	}
	
	return 0;
}

bool CClause::isTautology()
{
	if (pos & neg) { 
		return true;
	}
	return false;
}

int CClause::check_rw(int id)
{
	int size = rwVec.size();
	
	for (int i=0; i < size; ++i) {
		if (rwVec[i] == id) {
			return true;	
		}
	}
	return false;
}

int CClause::getNumProps(long long num) 
{
	//Count up the number of possible resolutions
	int count = 0; 
	
	while (num) { 
		if (num & 1) {
			++count;
		}
		num >>= 1; //bit shift 1 to the left
	}	
	
	return count; //return the number of possible resolutions
}

int CClause::resolve(CClause _clause, CClause &_newClause, int newId)
{	
	int totalRes; //hold total number of possible resolution variables
	CClause tempClause; //resolution data for clause 1 (this class)
	CClause tempClause2; //resolution data for clause 2 (passed in class)
	
	if (check_rw(_clause.get_cid())) {
		return 3; //the two clauses have already been resolved
	}
	
	tempClause.set_pos(pos & _clause.get_neg()); 
	tempClause.set_neg(neg & _clause.get_pos());
						
	tempClause2.set_pos(neg & _clause.get_pos());
	tempClause2.set_neg(pos & _clause.get_neg());
	
	totalRes = getNumProps(tempClause.get_pos()) + getNumProps(tempClause.get_neg());
	if (!totalRes) {
		return 1; //resolution is not possible
	}
	if (totalRes > 1) {
		return 2; //resolution produces tautologous clause
	}
	
					//Ex: ((1 xor 0) or (1 xor 1)) = (1 or 0) = 1
	_newClause.set_pos((pos ^ tempClause.get_pos()) | (_clause.get_pos() ^ tempClause2.get_pos()));
	_newClause.set_neg((neg ^ tempClause.get_neg()) | (_clause.get_neg() ^ tempClause2.get_neg()));
	_newClause.set_cid(newId);
	
	int newPropsNum = getNumProps(_newClause.get_pos()) + getNumProps(_newClause.get_neg());
	int cPropsNum1 = getNumProps(pos) + getNumProps(neg);
	int cPropsNum2 = getNumProps(_clause.get_pos()) + getNumProps(_clause.get_neg());
	
	//check if new clause is a superset of either resolution clauses
	if (newPropsNum > cPropsNum1 && newPropsNum > cPropsNum2) {
		return 5; //new clause is a superset 
	}
	
	//Check if the new clause is empty
	if (_newClause.isEmpty()) {
		return 4; //is NOT SAT
	}
	
	//Set _clause as already resolved with
	add_rw_clause(_clause.get_cid());
	
	return 0;
}

//---------GETTERS AND SETTERS-------------//
void CClause::set_pos(long long _pos)
{
	pos = _pos;
}

void CClause::set_neg(long long _neg)
{
	neg = _neg;
}

long long CClause::get_pos()
{
	return pos;
}

long long CClause::get_neg()
{
	return neg;
}

void CClause::set_cid(int id)
{
	cid = id;
}

int CClause::get_cid()
{
	return cid;
}

void CClause::add_rw_clause(int id)
{
	rwVec.push_back(id);
}
//-------END GETTERS AND SETTERS--------//

bool CClause::isEmpty()
{
	if (pos | neg) {
		return false;
	}
	return true;
}

bool CClause::isUnit()
{
	unsigned long long tempPos = pos;
	unsigned long long tempNeg = neg;
	int count = 0;

	while (tempPos || tempNeg) { 
		if (tempPos & 1) {
			++count;
		}
		if (tempNeg & 1) {
			++count;
		}
		
		tempPos >>= 1; //bit shift 1 to the left
		tempNeg >>= 1; //bit shift 1 to the left
	}	
	
	if (count == 1) {
		return true;
	}
	return false;
} 

int CClause::get_prop()
{
	//This only works with unit clauses
	int count = 1;

	if (pos) {
		int tempPos = pos;
		while (tempPos) { 
			if (tempPos & 1) {
				return count;
			}
			++count;
			tempPos >>= 1; //bit shift 1 to the left
		}	
	}
	else {
		int tempNeg = neg;
		while (tempNeg) { 
			if (tempNeg & 1) {
				return (count * -1);
			}
			++count;
			tempNeg >>= 1; //bit shift 1 to the left
		}	
	}
	
	return 0; //something went wrong
}


int CClause::compare(CClause clause)
{
	//compares two clauses
	if ((pos ^ clause.get_pos()) | (neg ^ clause.get_neg())) { 		
		return 0; //not equal
	}
	else { 
		return 1; //equal, because of value
	}			
}

void CClause::clear() 
{
	//set all class variables to 0
	pos = 0LL;
	neg = 0LL;
	rwVec.clear();
}

string CClause::itostr(int n)
{
	//Converts integers to strings. For output only
	ostringstream resultStr;
	resultStr << n;
	return resultStr.str();
}

vector<int> CClause::bitConvert(long long num) 
{
	//Converts long longs to their bit representation
	vector<int> ret;
	int count = 0;

	while (count < 64) {
		if (num & 1) {
			ret.push_back(1);
		}
		else {
			ret.push_back(0);
		}
		num >>= 1;
		++count;
	}	
	reverse(ret.begin(), ret.end()); //reverse order of bits
	
	return ret;
}

string CClause::toString() 
{
	vector<int> posBits = bitConvert(pos); 
	vector<int> negBits = bitConvert(neg);
	string result = "";

	result = "ID: " + itostr(cid);
	result += "\nPOS: ";
	for (int i=0; i < posBits.size(); ++i) {
		result += itostr(posBits[i]);
	}
	result += "\nNEG: ";
	for (int i=0; i < negBits.size(); ++i) {
		result += itostr(negBits[i]);
	}
	result += "\n";
	
	return result;
}

string CClause::toString(int dummy)
{
	vector<int> posBits = bitConvert(pos); 
	vector<int> negBits = bitConvert(neg);
	vector<int> finalClause;
	string result = "";
	
	reverse(posBits.begin(), posBits.end());
	reverse(negBits.begin(), negBits.end());
	
	for (int i=0; i < posBits.size(); ++i) {
		if (posBits[i]) 
			finalClause.push_back(i+1);
		if (negBits[i])
			finalClause.push_back((i * -1) - 1);
	}
	
	result += itostr(cid) + ": ";
	for (int i=0; i < finalClause.size(); ++i) {
		result += itostr(finalClause[i]);
		if (i < finalClause.size()-1) { //don't place comma after last proposition
			result += ",";
		}
	}
	
	return result;
}

#endif