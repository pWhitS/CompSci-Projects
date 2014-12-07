/***********************************************************************
annet.cpp

Artificial Neural Network Class with one hidden layer - externally defined
function members
************************************************************************/
#include <iostream>
#include "annet.hpp"

using namespace std;

/*********************************************************************
HELPER FUNCTION PROTOTYPES (STATIC SO NOT VISIBLE OUTSIDE THIS FILE)
**********************************************************************/

static double sigmoid(double sum);


/*********************************************************************
MEMBER FUNCTIONS
**********************************************************************/

/*--------------------------------------------------------------------
annet::setUp

Setup the physical parts of the ANN.  Does not initialize weights.  If
the number of inputs is something other than 0, then there is an error.
In this case, the network must be cleared first.  It is an error for any
parameter to be < 1.  Initializes learning rate to 0.05 and momentum to 0.

PARAMETERS
nInputs: Number of inputs to the ANN
nHidden: Number of nodes in the hidden layer of the ANN
nOut: Number of outputs from the ANN

RETURNS
0: SUCCESS
1: ALREADY SET UP
2: BAD VALUE FOR NUMBER OF INPUTS
3: BAD VALUE FOR NUMBER OF HIDDEN NODES
4: BAD VALUE FOR  NUMBER OF OUTPUT NODES
----------------------------------------------------------------------*/
int annet::setUp(int nInputs, int nHidden, int nOut)
{
	//*** CHECK FOR ERRORS AND RETURN APPROPRIATE ERROR CODE IF FOUND
	if (input.size() != 0) //*** ALREADY SET UP
	   return 1;
   
	if (nInputs < 1) //*** NUM OF INPUTS TOO SMALL
	   return 2;
	  
	if (nHidden < 1) //*** NUM OF HIDDEN NODES TOO SMALL
	   return 3;
   
	if (nOut < 1) //*** NUM OF OUTPUT NODES TOO SMALL
	   return 4;
   
	//*** NO ERRORS, SET NUMERIC VALUES AND RESIZE NODE VECTORS
	learnRate = 0.05;
	momentum = 0.0;
	input.resize(nInputs + 1); //*** LAST IS INPUT FOR EXTRA WEIGHT
	hidLayer.resize(nHidden + 1); //*** LAST IS INPUT FOR EXTRA WEIGHT
	outLayer.resize(nOut);

	//*** RESIZE INPUT WEIGHT VECTOR AND ITS ELEMENTS
	inWght.resize(nInputs + 1); //*** +1 GIVES ROOM FOR EXTRA WEIGHT
	for (int i = 0; i < inWght.size(); i++)
	   inWght[i].resize(nHidden);
   
	//*** RESIZE OUTPUT WEIGHT VECTOR AND ITS ELEMENTS
	outWght.resize(nHidden + 1); //*** +1 GIVES ROOM FOR EXTRA WEIGHT
	for (int i = 0; i < outWght.size(); i++)
	   outWght[i].resize(nOut);
   
	//*** INITIALIZE INPUTS FOR EXTRA WEIGHTS
	input.back() = hidLayer.back().value = 1.0;

	//*** INITALIZE WEIGHT PRIOR DELTAS TO ALL 0
	for (int i = 0; i < inWght.size(); i++)
	   for (int j = 0; j < inWght[i].size(); j++)
		  inWght[i][j].priorDelta = 0.0;
	  
	for (int i = 0; i < outWght.size(); i++)
	   for (int j = 0; j < outWght[i].size(); j++)
		  outWght[i][j].priorDelta = 0.0;
   
	return 0;
}

/*--------------------------------------------------------------------
annet::feedForward

Given input, feed forward through the network generating output.

PARAMETERS
inputVals: Inputs to the ANN

RETURNS
0: SUCCESS
1: WRONG NUMBER OF INPUTS
----------------------------------------------------------------------*/
int annet::feedForward(const vector<double> inputVals, bool useExtraWeight)
{ 
	int wghtLim,
		layerLim;
	double sum; //*** HOLDS WEIGHTED SUM FOR A NODE

	//*** MAKE SURE WE HAVE THE RIGHT NUMBER OF INPUTS
	if ((input.size() - 1) != inputVals.size())
	   return 1;

	//*** COPY INPUT IN 
	for (int i = 0; i < inputVals.size(); i++)
	   input[i] = inputVals[i];
   
	//*** SET LIM FOR INPUT SO THAT WE USE/DON'T USE EXTRA WEIGHT AS DESIRED
	if (useExtraWeight)
	   wghtLim = input.size();
	else
	   wghtLim = input.size() - 1;
   
	//*** CALCULATE VALUES FOR THE HIDDEN LAYER
	layerLim = hidLayer.size() - 1;  //*** DON'T CHANGE CONSTANT 1 FOR OUPUT EXTRA
									 //    WEIGHT
	for (int i = 0; i < layerLim; i++) //*** STEP THROUGH HIDDEN NODES
	   {
	   sum = 0.0;
	   for (int j = 0; j < wghtLim; j++) //*** STEP THROUGH INPUTS
		  sum += input[j] * inWght[j][i].value;
	  
	   hidLayer[i].value = sigmoid(sum);
	   }
   
	//*** SET LIM FOR HIDDEN VALS SO THAT WE USE/DON'T USE EXTRA WEIGHT AS DESIRED
	if (useExtraWeight)
	   wghtLim = hidLayer.size();
	else
	   wghtLim = hidLayer.size() - 1;
   
	//*** CALCULATE VALUES FOR THE OUTPUT LAYER
	layerLim = outLayer.size();  
	for (int i = 0; i < layerLim; i++) //*** STEP THROUGH OUTPUT NODES
	   {
	   sum = 0.0;
	   for (int j = 0; j < wghtLim; j++) {//*** STEP THROUGH HIDDEN LAYER VALUES
		  sum += hidLayer[j].value * outWght[j][i].value;
		}
	  
	   outLayer[i].value = sigmoid(sum);
	   }
   
	return 0;
}


/*--------------------------------------------------------------------
annet::calculateError

Given targets, calculate value of error terms.  Second parameter allows return
of average output error.

PARAMETERS
outputTargets: Output targets for this run of the ANN
aveErr: Average output error

RETURNS
0: SUCCESS
1: WRONG NUMBER OF TARGETS
----------------------------------------------------------------------*/
int annet::calculateError(const vector<double> outputTargets, double &aveErr)
{ 
	int layerLim,
		wghtLim;
	double val,  //*** TEMP FOR HOLDING THE VALUE OF A NODE
		   outErrSum = 0.0,  //*** SUM OF MAGNITUDES OF OUTPUT LAYER ERRORS
		   wErrSum; //*** SUM OF WEIGHTED ERROR INTO NODE

	//*** CHECK TO ENSURE THAT WE HAVE THE CORRECT NUMBER OF TARGETS
	//    IF BAD, THEN RETURN ERROR CODE

	if (outputTargets.size() != outLayer.size())
		return 1;
   
	//*** CALCULATE ERROR TERMS FOR OUTPUT LAYER
	layerLim = outLayer.size();
	for (int i = 0; i < layerLim; i++)
	   {
	   val = outLayer[i].value;
	   outLayer[i].errTerm = val * (1 - val) * (outputTargets[i] - val);
	   outErrSum += fabs(outLayer[i].errTerm);
	   }
   
	//*** CALCULATE THE AVERAGE ERROR AND STORE IT IN SECOND PARAMETER
	aveErr = outErrSum / outLayer.size();
   
	//*** CALCULATE HIDDEN LAYER ERROR TERMS
	layerLim = hidLayer.size() - 1; //*** LAST ONE IS DUMMY FOR EXTRA WEIGHT
	wghtLim = outLayer.size();
	for (int i = 0; i < layerLim; i++)
	   {
	   wErrSum = 0.0;
	   for (int j = 0; j < wghtLim; j++) //*** WEIGHTED ERROR SUM FOR HID NODE i
		  {
		  wErrSum += outLayer[j].errTerm * outWght[i][j].value;
		  }
	   val = hidLayer[i].value;
	   hidLayer[i].errTerm = val * (1 - val) * wErrSum;
	   }
	   
	return 0;
}

/*--------------------------------------------------------------------
annet::adjustWeights

Assuming the feedforward and calculation of error terms has already 
happened, adjust weights

PARAMETERS
useMomentum: If true, use momentum term, otherwise don't

RETURNS: NONE
----------------------------------------------------------------------*/
void annet::adjustWeights(bool useMomentum)
{ 
	double alpha,  //*** MOMENTUM TERM CONSTANT
		   mTrm;  //*** MOMENTUM TERM

	//*** SET MOMENTUM CONSTANT (0 IF useMomentum is false)
	if (useMomentum)
	   alpha = momentum;
	else
	   alpha = 0.0;
   
	//*** DO INPUT WEIGHTS
	for (int i = 0; i < inWght.size(); i++)
	   for (int j = 0; j < inWght[i].size(); j++)
		  {
		  mTrm = alpha * inWght[i][j].priorDelta;
		  inWght[i][j].priorDelta = learnRate * input[i] * hidLayer[j].errTerm + mTrm;
		  inWght[i][j].value += inWght[i][j].priorDelta;
		  }
	
	//*** DO OUTPUT WEIGHTS
	for (int i = 0; i < outWght.size(); i++)
	   for (int j = 0; j < outWght[i].size(); j++)
		  {
		  mTrm = alpha * outWght[i][j].priorDelta;
		  outWght[i][j].priorDelta = learnRate * hidLayer[i].value * 
													 outLayer[j].errTerm + mTrm;
		  outWght[i][j].value += outWght[i][j].priorDelta;
		  }
		  
}

/*--------------------------------------------------------------------
annet::getWeights

Return weights in vectors given through parameters

PARAMETERS
iWt: Input weight vector
oWt: Output weight vector

RETURNS: NONE
----------------------------------------------------------------------*/
void annet::getWeights(vector<vector<weight> > &iWt, 
                       vector<vector<weight> > &oWt)
{
	//*** CLEAR, THEN SIZE OUTER VECTORS
	iWt.clear();
	iWt.resize(inWght.size());
	oWt.clear();
	oWt.resize(outWght.size());

	//*** COPY INPUT WEIGHTS
	for (int i = 0; i < iWt.size(); i++)
	   for (int j= 0; j < inWght.size(); j++)
		  iWt[i].push_back(inWght[i][j]);
	  
	//*** COPY OUTPUT WEIGHTS
	for (int i = 0; i < oWt.size(); i++)
	   for (int j= 0; j < outWght.size(); j++)
		  oWt[i].push_back(outWght[i][j]);
}

void annet::getWeights(vector<double> &inw, vector<double> &outw)
{
	inw.clear();
	outw.clear();

	//copy input weights
	for (int i=0; i < inWght[0].size(); ++i) {
		inw.push_back(inWght[0][i].value);
	}
	
	//copy output weights
	for (int i=0; i < outWght[0].size(); ++i) {
		outw.push_back(outWght[0][i].value);
	}
}

void annet::setWeights(const vector<vector<weight> > &iWt, 
				       const vector<vector<weight> > &oWt)
{
	inWght = iWt;
	outWght = oWt;
}

int annet::setLayerValues(const vector<double> &inVals,
						  const vector<double> &hidVals, 
						  const vector<double> &outVals)
{
	if (inVals.size() != input.size()-1) {
		return 1;
	}
	if (hidVals.size() != hidLayer.size()) {
		return 2;
	}
	if (outVals.size() != outLayer.size()) {
		return 3; //error
	}
	
	for (int i=0; i < inVals.size(); ++i) {
		input[i] = inVals[i];
	}
	for (int i=0; i < hidVals.size(); ++i) {
		hidLayer[i].value = hidVals[i];	
	}
	for (int i=0; i < outVals.size(); ++i) {
		outLayer[i].value = outVals[i];
	}
	
	return 0;
}

void annet::getLayerValues(vector<double> &hidVals, vector<double> &outVals)
{	
	for (int i=0; i < hidLayer.size(); ++i) {
		hidVals.push_back(hidLayer[i].value);	
	}
	
	for (int i=0; i < outLayer.size(); ++i) {
		outVals.push_back(outLayer[i].value);
	}

	/*
	cout << "--Hidden Layer Values---\n" << endl;
	for (int i=0; i < hidLayer.size(); ++i) {
		cout << i << ": " << hidLayer[i].value << endl;
	}
	cout << endl;

	cout << "---Output Layer Values---\n" << endl;
	for (int i=0; i < outLayer.size(); ++i) {
		cout << i << ": " << outLayer[i].value << endl;
	}*/
}

void annet::outputErrorTerms()
{
	cout << "--Hidden Error Terms--" << endl;
	for (int i=0; i < hidLayer.size(); ++i) {
		cout << i << ": " << hidLayer[i].errTerm << endl;
	}
	cout << endl;
}

void annet::toString()
{
	cout << "--VALUES--" << endl;
	cout << "Inputs - ";
	for (int i=0; i < input.size(); ++i) {
		cout << i << ": " << input[i] << "  ";
	}
	cout << endl;
	
	cout << "Hidden - ";
	for (int i=0; i < hidLayer.size(); ++i) {
		cout << i << ": " << hidLayer[i].value << "  ";
	}
	cout << endl;
	
	cout << "Output - ";
	for (int i=0; i < outLayer.size(); ++i) {
		cout << i << ": " << outLayer[i].value << "  ";
	}
	cout << endl;
	
	cout << "\n--WEIGHTS--" << endl;
	for (int i=0; i < inWght.size(); ++i) {
		cout << "Input Node " << i << endl;
		cout << "Weights - ";
		for (int j=0; j < inWght[i].size(); ++j) {
			 cout << j << ": " << inWght[i][j].value << "  ";
		}
		cout << "\n" << endl;
	}
	
	for (int i=0; i < outWght.size(); ++i) {
		cout << "Hidden Node " << i << endl;
		cout << "Weights - ";
		for (int j=0; j < outWght[i].size(); ++j) {
			 cout << j << ": " << outWght[i][j].value << "  ";
		}
		cout << "\n" << endl;
	}
	
}

/*********************************************************************
HELPER FUNCTION DEFINITIONS (STATIC SO NOT VISIBLE OUTSIDE THIS FILE)
**********************************************************************/
static double sigmoid(double sum)
{
	return 1.0 / (1.0 + exp(-sum));
}
