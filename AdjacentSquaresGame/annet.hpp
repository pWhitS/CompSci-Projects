/***********************************************************************
annet.hpp

Artificial Neural Network Class with one hidden layer
************************************************************************/

#ifndef ANNET_HPP
#define ANNET_HPP	

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

//*** WEIGHT TYPE
struct weight
{
	double value;
    double priorDelta;
};

//*** NODE TYPE
struct node
{
	double value;
    double errTerm;
};

//*** ANN TYPE
class annet
{
	vector<double> input;
	vector<node> hidLayer;
	vector<node> outLayer;
	vector<vector<weight> > inWght;
	vector<vector<weight> > outWght;
	double learnRate;
	double momentum;

public:
	annet() { }
	annet(int nInputs, int nHidden, int nOut) { setUp(nInputs, nHidden, nOut); }

	void clear()
		{hidLayer.clear();outLayer.clear();inWght.clear();outWght.clear();
		momentum = learnRate = 0.0; input.clear();}
	
	double getLearnRate() { return learnRate; }
	double getMomentum() { return momentum; }
	void setLearnRate(double rate)  { learnRate = rate; }
	void setMomentum(double mentum) { momentum = mentum; }

	int setUp(int nInputs, int nHidden, int nOut);
	int feedForward(const vector<double> inputVals, bool useExtraWeight);
	int calculateError(const vector<double> outputTargets, double &aveErr);
	void adjustWeights(bool useMomentum);
	
	void getWeights(vector<vector<weight> > &iWt, vector<vector<weight> > &oWt);
	void getWeights(vector<double> &inw, vector<double> &outw);
	void setWeights(const vector<vector<weight> > &iWt, const vector<vector<weight> > &oWt);
	
	int setLayerValues(const vector<double> &inVals,
					   const vector<double> &hidVals, 
					   const vector<double> &outVals);
	
	void toString();
	void getLayerValues(vector<double> &hidVals, vector<double> &outVals);
	void outputErrorTerms();
};

#endif