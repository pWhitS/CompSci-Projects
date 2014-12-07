/***************************************************************
CPP File for solving satisfiability of conjunctions 
through resolution

Dependancies: CClause.hpp

Created By: Patrick Whitsell, Ryan Daniels, Justin Henderson
****************************************************************/

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "CClause.hpp"

using namespace std;

clock_t t;
int numClauses = 0;
vector<int> unitVec; //holds the indices of unit clauses

int nc = 0;

int findAssingnment(vector<CClause> &_clauseVec, fstream &fout)
{
	int uSize = unitVec.size();
	
	for (int i=0; i < uSize; ++i) {
		int unitClause = _clauseVec[unitVec[i]].get_prop();
		if (unitClause > 0) {
			fout << unitClause << ": True" << endl;
			cout << unitClause << ": True" << endl;
		}
		else if (unitClause < 0) {
			fout << (unitClause * -1) << ": False" << endl;
			cout << (unitClause * -1) << ": False" << endl;
		}
		else {
			cout << "Something went terribly wrong..." << endl;
		}
	}
		
	return 0;
}

bool isNewClause(vector<CClause> &_clauseVec, CClause &_newClause)
{
	int size = _clauseVec.size();
	
	for (int i=0; i < size; ++i) {
		if (_clauseVec[i].compare(_newClause)) {
			return false;
		}
	}
	return true;
}

bool resolveCNF(vector<CClause> &_clauseVec, fstream &fout)
{
	CClause newClause;
	CClause tClause1;
	CClause tClause2;
	
	int startNum = 0, 
		rStatus = 0;
			
	bool isDone = false; //loops until a solution has been found
	
	while (!isDone) {
		startNum = numClauses; //set the original number of clauses
		
		//UNIT CLAUSE RESOLVING
		for (int i=0; i < unitVec.size(); ++i) {
			for (int j=0; j < numClauses; ++j) {
				tClause1 = _clauseVec[unitVec[i]];
				tClause2 = _clauseVec[j];
				
				nc++;//DIAGNOSTICS		
				
				rStatus = tClause1.resolve(tClause2, newClause, numClauses);
				if (!rStatus) { //The clauses were resolved!
					//Make sure new clause does not already exist. 
					if (!isNewClause(_clauseVec, newClause)) { 
						continue;							   
					} 
					_clauseVec.push_back(newClause);
					++numClauses;
					
					//output new clause data to file
					fout << "New Clause! - " << newClause.get_cid() << " from clauses ";
					fout << tClause1.get_cid() << " and " << tClause2.get_cid() << " --> ";
					fout << newClause.toString(1) << endl;
					
					if (newClause.isUnit()) { //UNIT CLAUSE
						unitVec.push_back(newClause.get_cid());
					}
				}
				else if (rStatus == 4) { //Empty clause, not SAT
					//copy temp clauses back into original clauses
					_clauseVec[unitVec[i]] = tClause1;
					_clauseVec[j] = tClause2;
					_clauseVec.push_back(newClause); //add the empty clause
					
					//output new clause data to file
					fout << "New Clause! - " << newClause.get_cid() << " from clauses ";
					fout << tClause1.get_cid() << " and " << tClause2.get_cid() << " --> ";
					fout << newClause.toString(1) << endl;
					
					return false;
				}
				else if (rStatus == 2) { //resolution produced a tautology
					fout << "Clause Removed: Resolution of " << tClause1.get_cid();
					fout << " and " << tClause2.get_cid() << " results in a tautology." << endl;
				}
				else if (rStatus == 5) { //resolution produced a superset
					fout << "Clause Removed: Resolution of " << tClause1.get_cid() << " and ";
					fout << tClause2.get_cid() << " results in a superset clause." << endl;
				}
				
				//copy temp clauses back into original clauses
				_clauseVec[unitVec[i]] = tClause1;
				_clauseVec[j] = tClause2;
			}
		}
		//IF MORE CLAUSES ARE MADE, return to unit resolving first
		if (numClauses > startNum) {
			continue;
		}
		
		//BRUTE FORCE RESOLVING
		for (int i=0; i < numClauses; ++i) {
			for (int j=i+1; j < numClauses; ++j) {
				tClause1 = _clauseVec[i];
				tClause2 = _clauseVec[j];
				
				nc++;//DIAGNOSTICS		
				
				rStatus = tClause1.resolve(tClause2, newClause, numClauses);
				if (!rStatus) { //The clauses were resolved!
					//Make sure new clause does not already exist. 
					if (!isNewClause(_clauseVec, newClause)) { 
						continue;							   
					} 
					_clauseVec.push_back(newClause);
					++numClauses;
					
					//output new clause data to file
					fout << "New Clause! - " << newClause.get_cid() << " from clauses ";
					fout << tClause1.get_cid() << " and " << tClause2.get_cid() << " --> ";
					fout << newClause.toString(1) << endl;
					
					//UNIT CLAUSES, NOT IN USE
					if (newClause.isUnit()) {
						unitVec.push_back(newClause.get_cid());
						break;
					}
				}
				else if (rStatus == 4) { //Empty clause, not SAT
					//copy temp clauses back into original clauses
					_clauseVec[i] = tClause1;
					_clauseVec[j] = tClause2;
					_clauseVec.push_back(newClause); //add the empty clause
					
					//output new clause data to file
					fout << "New Clause! - " << newClause.get_cid() << " from clauses ";
					fout << tClause1.get_cid() << " and " << tClause2.get_cid() << " --> ";
					fout << newClause.toString(1) << endl;
					
					return false;
				}
				else if (rStatus == 2) { 
					fout << "Clause Removed: Resolution of " << tClause1.get_cid();
					fout << " and " << tClause2.get_cid() << " results in a tautology." << endl;
				}
				else if (rStatus == 5) { //resolution produced a superset
					fout << "Clause Removed: Resolution of " << tClause1.get_cid() << " and ";
					fout << tClause2.get_cid() << " results in a superset clause." << endl;
				}
				
				//copy temp clauses back into original clauses
				_clauseVec[i] = tClause1;
				_clauseVec[j] = tClause2;
			}
			//break to unit resolving to check new clauses with unit clauses
			if (numClauses > startNum) {
				break;
			}
		}
		cout << ".";
		//This means no new clauses were created
		if (startNum == numClauses) {
			return true;
		}
	}
	
	return false;
}

int initClauses(vector< vector<int> > &_cnfVec, vector<CClause> &cVec)
{
	CClause clause;
	int retcode = 0;
	
	for (int i=0; i < _cnfVec.size(); ++i) {
		clause.init(_cnfVec[i], numClauses);
		++numClauses;
		
		if (clause.isTautology()) {
			retcode = i;
			continue;
		}
		if (clause.isUnit()) {
			unitVec.push_back(clause.get_cid());
		}
		cVec.push_back(clause);
	}
	
	return retcode;
}

int readFromFile(vector< vector<int> > &_cnfVec, string filename) 
{
	string lineBuffer, propBuffer;
	vector<int> disjunctVec;
	int propNum;
	
	fstream fin(filename.c_str(), fstream::in); //open the input file
	
	while (getline(fin, lineBuffer)) {
		if (lineBuffer.length() == 0) { //empty line or no lines in file
            return 1;
		}
		istringstream iss(lineBuffer);
		while (getline(iss, propBuffer, ',')) {
			propNum = strtol(propBuffer.c_str(), NULL, 10);
			if (!propNum) { //if propNum is 0
				return 2;
			}
			disjunctVec.push_back(propNum);
		}
		_cnfVec.push_back(disjunctVec);
		disjunctVec.clear();
	}
	fin.close(); //close the input file
	
	if (!_cnfVec.size()) { //_cnfVec has size 0
        return 3;
	}
	
	return 0;
}

int main(int argc, char* argv[]) 
{
	t = clock();
	
	if (argc < 3) {
		cout << "Error: Input files are missing!" << endl;
		exit(0);
	}
	else if (argc > 3) {
		cout << "Error: Too many inputs!" << endl;
		exit(0);
	}
	
	vector< vector<int> > cnfVec;
	vector<CClause> clauseVec;
	
	//read in clauses from the input file
	cout << "\nReading clauses from file..." << endl;
	if (readFromFile(cnfVec, argv[1])) { 
		cout << "Error: Malformed input file." << endl;
		return 0;
	}
	//open output file
	fstream fout(argv[2], fstream::out); 
	
	cout << "Preparing clauses for resolution..." << endl;
	initClauses(cnfVec, clauseVec); //initialize clauses from input data
	
	fout << "--Given Clauses--" << endl;
	for (int i=0; i < clauseVec.size(); ++i) {
		fout << clauseVec[i].toString(0) << endl;
	}
	fout << "**********" << endl;
	
	cout << "Resolving clauses..." << endl;
	bool isSat = resolveCNF(clauseVec, fout); //function to resolve clauses
	
	fout << "**********" << endl;
	if (isSat) {
		fout << "No new clauses can be created!" << endl;
		fout << "The Formula Is Satisfiable!" << endl;
		cout << "\n\nA Satisfying Assignment Exists!" << endl;
		
		findAssingnment(clauseVec, fout);
	}
	else {
		fout << "An empty clause has been found!" << endl;
		fout << "The Formula Is Not Satisfiable!" << endl;
		cout << "\n\nA Satisfying Assignment Does Not Exist!" << endl;
	}
	cout << endl;
	
	//DIAGNOSTICS INFORMATION
	t = clock() - t;
	cout << "Clock Ticks: " << t << " (" << ((float)t)/CLOCKS_PER_SEC << " seconds)" << endl;
	cout << "Main Loops: " << nc << endl;
}