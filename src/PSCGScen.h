/*
Author: Brian C. Dandurand (2017-2018)
Based on and modified from code developed by Jeffrey Christiansen, Brian Dandurand, and Fabricio Oliveira
at RMIT in Melbourne Australia with funding under project ARC DP 140100985 during 2015-2017.
CIs of that projects were Prof. Andrew Eberhard, Prof. Natashia Boland, and PI Prof. Jeffrey Linderoth.
*/

#ifndef PSCGSCEN_H
#define PSCGSCEN_H

//#include "StructureDefs.h"
#include <list>
#include <iostream>
#include <memory>
#include <algorithm>
#include <ilcplex/ilocplex.h>
#include "DecTssModel.h"
#include "OsiCpxSolverInterface.hpp"
//#include "PSCGParams.h"
//#include "CoinUtils.h"

#define ptrModel IloModel*
#define ptrRange IloRange*
#define ptrptrRange IloRange**
#define ptrObjective IloObjective*
#define ptrNumVarArray IloNumVarArray*

#define ptrDouble double*

#define MIP_TOL 1e-9


//ILOSTLBEGIN

#if 1
enum SolverReturnStatus {
  PSCG_OK=0,
  PSCG_OPTIMAL=0,
  PSCG_OPT_INT_FEAS,
  PSCG_INT_FEAS,
  PSCG_ABANDONED,
  PSCG_PRIMAL_INF_INT, //z does not satisfy integrality
  PSCG_PRIMAL_INF_REC, //z does not have recourse
  PSCG_PRIMAL_INF_INT_REC, //z both does not satisfy integrality and does not have recourse
  PSCG_PRIMAL_INF, //z both does not satisfy integrality and does not have recourse
  PSCG_DUAL_INF,
  PSCG_PRIMAL_LIM,
  PSCG_DUAL_LIM,
  PSCG_ITER_LIM,
  PSCG_ERR=100,
  PSCG_INF=200,
  PSCG_UNBOUND=201,
  PSCG_UNKNOWN=202
};
#endif


class PSCGScen{

protected:

int nThreads;

int n1;
int n2;
int nS;
int tS; //scenario
bool initialised;

double pr;
double *c;
double *d;

#if 0
vector<int> branchIndices;
vector<double> restoreLBs;
vector<double> restoreUBs;
#endif
//vector<double> branchLBs;
//vector<double> branchUBs;
double *origLBs_;
double *origUBs_;

double LagrBd;
double objVal;
double gapVal;

double *x;
double *dispersions;
double *intDiscVec_;
double *y;

double *aggrXY0;
double *aggrXY1;
double aggrA0;
double aggrA1;

int nVertices;
int maxNVertices;
int oldestVertexIndex;
double* x_vertex;
double* y_vertex;
//double* x_vertex_opt;
//double* y_vertex_opt;
vector< vector<double> > xyVertices; //row ordered
//vector< vector<double> > yVertices;
vector<double> vecWeights;
int bestVertexIndex;

//vector<double> baseWeightObj;
//IloExpr quadraticTerm;

double ALVal;
double sqrNormDiscr;
int solverStatus_;


public:
PSCGScen(int nthreads=1):
nThreads(nthreads),
n1(0),n2(0),nS(0),tS(-1),initialised(false),solverStatus_(0),
x(NULL),y(NULL),c(NULL),d(NULL),x_vertex(NULL),y_vertex(NULL),oldestVertexIndex(-1),bestVertexIndex(-1),
nVertices(0),maxNVertices(0),LagrBd(-COIN_DBL_MAX),objVal(-COIN_DBL_MAX),pr(0.0){;}

//copy constructor
PSCGScen(const PSCGScen &other):
nThreads(other.nThreads),
n1(0),n2(0),nS(0),tS(-1),initialised(false),solverStatus_(0),
x(NULL),y(NULL),c(NULL),d(NULL),x_vertex(NULL),y_vertex(NULL),oldestVertexIndex(-1),bestVertexIndex(-1),
nVertices(0),maxNVertices(0),LagrBd(-COIN_DBL_MAX),objVal(-COIN_DBL_MAX),pr(0.0){;}

//int initialiseSMPS(SMIP_fileRequest* probSpecs, int scenario);
//int initialiseSMPS(SMIP_fileRequest* probSpecs, DecTssModel &smpsModel, int scenario);
void finishInitialisation();

~PSCGScen(){
  delete [] x;
  delete [] dispersions;
  delete [] intDiscVec_;
  //delete [] y;
  delete [] c;
  //delete [] d;
  delete [] x_vertex;
  //delete [] y_vertex;
  delete [] origLBs_;
  delete [] origUBs_;
  delete [] aggrXY0;
  delete [] aggrXY1;
#if 0
  mpModel.end();
  mpObjective.end();
  mpWeightConstraints.end();
  mpVertexConstraints.end();
  mpWeight0.end();
  mpWeightVariables.end();
  mpAuxVariables.end();
  cplexMP.end();
  weightSoln.end();
#endif
}
virtual void setInitialSolution(const int *indices, const double *startSol){
    cout << "setInitialSolution(): default implementation, does nothing." << endl;
}

#if 0
virtual int initialLPSolve(const double* omega=NULL){
cerr << "initialLPSolve(): default implementation does nothing" << endl;
return 0;
}
#endif
virtual int solveLagrangianProblem(const double* omega=NULL, bool doInitialSolve=false)=0;

//virtual int solveAugmentedLagrangianMIP(const double* omega, const double* z, const double rho, const double* scal)=0;
virtual int solveFeasibilityProblem()=0;
virtual int getCPLEXErrorStatus(){
    cerr << "getCPLEXErrorStatus(): default implementation, does nothing, returning 0" << endl;
    return 0;
}
int getSolverStatus(){return solverStatus_;}
virtual const char* getColTypes(){
cout << "getColTypes(): Default implementation: doing nothing" << endl;
    return NULL;
}
virtual bool varIsInt(int index){return false;}
double* getC(){return c;}
double* getD(){return d;}
double* getDispersions(){return dispersions;}
virtual double getMIPBestNodeVal(){
    cout << "getMIPBestNodeVal(): default implementation, does nothing" << endl;
    return -COIN_DBL_MAX;
}
virtual void setMIPPrintLevel(int outputControl=0, int outputControlMIP=0, bool doReducePrint=true){
   cout << "setPrintLevel(): default implementation, does nothing" << endl;
}
void setNThreads(int nthreads){nThreads=nthreads;}
virtual int solveLagrangianWithXFixedToZ(const double *z, const double *omega, double *origLBs, double *origUBs)=0;
//virtual int solveFeasibilityProblemWithXFixedToZ(const double *z, const double *origLBs, const double *origUBs, const char *colTypes)=0;
virtual void setSolverStatus()=0;
#if 0
virtual void upBranchOnVar(int varIndex, double bound)=0;
virtual void downBranchOnVar(int varIndex, double bound)=0;
virtual void setLBs(const double *lbs, int nLBs){
cerr << "setLBs(): Default implementation does nothing." << endl;
}
virtual void setUBs(const double *ubs, int nUBs){
cerr << "setLBs(): Default implementation does nothing." << endl;
}
virtual void setBounds(const double *lbs, const double *ubs, int nBds){
cerr << "setBounds(): Default implementation does nothing." << endl;
}
#endif
virtual double getLB(const int ind){
cerr << "getLB(): Default implementation does nothing." << endl;
    return -COIN_DBL_MAX;
}
virtual double getUB(const int ind){
cerr << "getUBound(): Default implementation does nothing." << endl;
    return COIN_DBL_MAX;
}
virtual void setBound(const int ind, const double lb, const double ub, bool indNonRedundant=true){
cerr << "setBound(): Default implementation does nothing." << endl;
}
virtual void setBounds(const vector<int> &inds, const vector<double> &lbs, const vector<double> &ubs){
cerr << "setBounds(): Default implementation does nothing." << endl;
}
virtual void restoreBounds(){
cerr << "restoreBounds(): Default implementation does nothing." << endl;
}
#if 0
void updateOptSoln(){
   if(x_vertex_opt==NULL) x_vertex_opt = new double[n1];
   if(y_vertex_opt==NULL) y_vertex_opt = new double[n2];
   memcpy(x_vertex_opt,x_vertex,n1*sizeof(double));
   memcpy(y_vertex_opt,y_vertex,n2*sizeof(double));
}
double evaluateVertexOptSolution(const double* omega){
    double retVal=0.0;
    if(omega==NULL) {for(int ii=0; ii<n1; ii++) retVal+= (c[ii])*x_vertex_opt[ii];}
    else {for(int ii=0; ii<n1; ii++) retVal+= (c[ii]+omega[ii])*x_vertex_opt[ii];}
    for(int jj=0; jj<n2; jj++) retVal+= d[jj]*y_vertex_opt[jj];
    return retVal;
}
#endif
double evaluateVertexSolution(const double* omega){
    double retVal=0.0;
    if(omega==NULL){for(int ii=0; ii<n1; ii++) retVal+= (c[ii])*x_vertex[ii];}
    else{for(int ii=0; ii<n1; ii++) retVal+= (c[ii]+omega[ii])*x_vertex[ii];}
    for(int jj=0; jj<n2; jj++) retVal+= d[jj]*y_vertex[jj];
    return retVal;
}
double evaluateSolution(const double* omega){
    double retVal=0.0;
    if(omega==NULL){for(int ii=0; ii<n1; ii++) retVal+= (c[ii])*x[ii];}
    else{for(int ii=0; ii<n1; ii++) retVal+= (c[ii]+omega[ii])*x[ii];}
    for(int jj=0; jj<n2; jj++) retVal+= d[jj]*y[jj];
    return retVal;
}
double evaluateSolution(const double* omega, const double *z, const double rho, const double *scaling_vector){
//TODO
    double retVal=0.0;
    if(omega==NULL){for(int ii=0; ii<n1; ii++) retVal+= (c[ii])*x[ii];}
    else{for(int ii=0; ii<n1; ii++) retVal+= (c[ii]+omega[ii])*x[ii];}
    for(int jj=0; jj<n2; jj++) retVal+= d[jj]*y[jj];
    return retVal;
}
void evaluateVertexHistory(const double *omega){
    double retVal;
    for(int vv=0; vv<nVertices; vv++){
    	retVal=0.0;
    	if(omega==NULL){for(int ii=0; ii<n1; ii++) retVal+= (c[ii])*xyVertices[vv][ii];}
    	else{for(int ii=0; ii<n1; ii++) retVal+= (c[ii]+omega[ii])*xyVertices[vv][ii];}
    	//for(int jj=0; jj<n2; jj++) retVal+= d[jj]*yVertices[vv][jj];
    	for(int jj=n1; jj<n1+n2; jj++) retVal+= c[jj]*xyVertices[vv][jj];
	cout << "\tVertex " << vv << " results in a value of " << retVal << endl;
    }
}
void evaluateVertexHistory(const double *omega, shared_ptr<ofstream> outStream){
    double retVal;
    for(int vv=0; vv<nVertices; vv++){
    	retVal=0.0;
    	if(omega==NULL){for(int ii=0; ii<n1; ii++) retVal+= (c[ii])*xyVertices[vv][ii];}
    	else{for(int ii=0; ii<n1; ii++) retVal+= (c[ii]+omega[ii])*xyVertices[vv][ii];}
    	//for(int jj=0; jj<n2; jj++) retVal+= d[jj]*yVertices[vv][jj];
    	for(int jj=n1; jj<n1+n2; jj++) retVal+= c[jj]*xyVertices[vv][jj];
	*(outStream) << "\tVertex " << vv << " results in a value of " << retVal << endl;
    }
}

double optimiseLagrOverVertexHistory(const double *omega){
    //double oldLagrBd = LagrBd;
    double vertexVal;
    double retVal=COIN_DBL_MAX; //LagrBd=COIN_DBL_MAX;
    int optIndex = -1;
    for(int vv=0; vv<nVertices; vv++){
    	vertexVal=0.0;
    	if(omega==NULL){for(int ii=0; ii<n1; ii++) vertexVal+= (c[ii])*xyVertices[vv][ii];}
    	else{for(int ii=0; ii<n1; ii++) vertexVal+= (c[ii]+omega[ii])*xyVertices[vv][ii];}
    	//for(int jj=0; jj<n2; jj++) vertexVal+= d[jj]*yVertices[vv][jj];
    	for(int jj=n1; jj<n1+n2; jj++) vertexVal+= c[jj]*xyVertices[vv][jj];
	//cout << "\tVertex " << vv << " results in a value of " << retVal << endl;
	if(vertexVal < retVal){
	    //LagrBd = retVal;
	    retVal=vertexVal;
	    optIndex = vv;
	}
    }
#if 0
    if(optIndex != -1){
        for(int ii=0; ii<n1; ii++){ x_vertex[ii]=xVertices[optIndex][ii];}
        for(int jj=0; jj<n2; jj++){ y_vertex[jj]=yVertices[optIndex][jj];}
        //polishSolution();
    }
#endif
    bestVertexIndex=optIndex;
    return retVal;
//virtual void setColSolution(const double *colsol) = 0;
}

bool checkWhetherVertexIsRedundant(){
    for(int vv=0; vv<nVertices; vv++){
	if(checkPairOfVerticesForEquality(vv)){
	    return true;
	}	
    }
    return false;
}

bool checkPairOfVerticesForEquality(int vv){
      for(int ii=0; ii<n1+n2; ii++){
	if(fabs(xyVertices[vv][ii]-x_vertex[ii]) > 1e-10){
	    return false;
	}
      }
#if 0
      for(int jj=0; jj<n2; jj++){
	if(fabs(yVertices[vv][jj]-y_vertex[jj]) > 1e-10){
	    return false;
	}
      }
#endif
      return true;
}

double getXVertexEntry(int entry, int vertex){
  return xyVertices[vertex][entry];
}

double*  computeIntegralityDiscrVec(){
    for(int ii=0; ii<n1; ii++){
	if(getColTypes()[ii]!='C'){
	    intDiscVec_[ii] = fabs(x[ii]-round(x[ii]));
	}
	else{
	    intDiscVec_[ii] = 0.0;
	}
    }
    for(int ii=0; ii<n2; ii++){
	if(getColTypes()[n1+ii]!='C'){
	    intDiscVec_[n1+ii]= fabs(y[ii]-round(y[ii]));
	}
	else{
	    intDiscVec_[n1+ii] = 0.0;
	}
    }
    return intDiscVec_;
    
}

double computeIntegralityDiscr(){
    double retval=0.0;
    for(int ii=0; ii<n1; ii++){
	if(getColTypes()[ii]!='C'){
	    retval+= fabs(x[ii]-round(x[ii]));
	}
    }
    for(int ii=0; ii<n2; ii++){
	if(getColTypes()[n1+ii]!='C'){
	    retval+= fabs(y[ii]-round(y[ii]));
	}
    }
    return retval;
}

void resetAggrInfo(){
    for(int ii=0; ii<n1+n2; ii++){ 
	aggrXY0[ii]=0.0;
	aggrXY1[ii]=0.0;
    }
    aggrA0=0;
    aggrA1=0;
}

//Precondition: Index ii variable is binary, current var val violates its integrality
void computeAggrXYAt(int kk){
    resetAggrInfo();
    for(int wI=0; wI<nVertices; wI++) {
	if(kk >=0 && kk<n1+n2){
	  if(fabs(xyVertices[wI][kk] - 1) < MIP_TOL){
	    aggrA1 += vecWeights[wI];
	    for(int ii=0; ii<n1+n2; ii++){
	      aggrXY1[ii] += vecWeights[wI]*xyVertices[wI][ii];
	    }
	  }
	  else if(fabs(xyVertices[wI][kk]) < MIP_TOL){
	    aggrA0 += vecWeights[wI];
	    for(int ii=0; ii<n1+n2; ii++){
	      aggrXY0[ii] += vecWeights[wI]*xyVertices[wI][ii];
	    }
	  }
	  else{
	     cerr << "computeAggrXYAt(): xyVertices[wI][kk] is not binary." << endl;
	     exit(1);
	  }
	}
	else{
	  cerr << "computeAggrXYAt(): kk is out of index range:" << kk << endl;
	}

    }
    for(int ii=0; ii<n1+n2; ii++){
	aggrXY0[ii] /= aggrA0;
	aggrXY1[ii] /= aggrA1;
    } 
#if 0
    cout << kk << ": aggrA0 and aggrA1: " << aggrA0 << " " << aggrA1 << "\t";
    cout  << x[kk] << endl;
#endif
}

double* computeDispBasedOnIntDisc(const double* z=NULL){
    resetDispersionsToZero();
    if(z==NULL) z=x;
    for(int kk=0; kk<n1+n2; kk++){
	if(getColTypes()[kk]!='C'){
	    //cout << kk <<": ";
	    if(fabs(x[kk]-round(x[kk])) > MIP_TOL){
	        computeAggrXYAt(kk);
		for(int ii=0; ii<n1+n2; ii++){
		    dispersions[ii] += aggrA0*fabs(aggrXY0[ii]-z[ii]) + aggrA1*fabs(aggrXY1[ii]-z[ii]);
	 	    //cout << " (" << ii << "," << dispersions[ii] << ")";
		}
	    }
	    //add to dispersions based on profile of violation of integrality
	    //cout << endl;
	}
    }
    //printDispersions();
    return dispersions;
}


void compareLagrBds(const double* omega){
   cout << LagrBd << " ***versus*** " << evaluateVertexSolution(omega) << endl;
}
virtual void printColTypesFirstStage(){;}
virtual void printColBds(){;}
void printOrigBDs(){
cout << "Printing original bounds:" <<endl;
    for(int ii=0; ii<n1+n2; ii++){
	cout << " ("<<origLBs_[ii] <<","<< origUBs_[ii] << ")";
    }
    cout << endl;
}
void printC(){
cout << "Printing c: " << endl;
 for(int ii=0; ii<n1; ii++){
  cout << " " << c[ii];
 }
 cout << endl;
}
virtual void printLagrSoln(){
return;
}
#if 0
bool roundIfClose(double &toBeRounded){
    if(fabs(toBeRounded-floor(toBeRounded)) < 1e-6){ 
	toBeRounded = floor(toBeRounded);
	return true;
    }
    if(fabs(ceil(toBeRounded)-toBeRounded) < 1e-6){ 
	toBeRounded = ceil(toBeRounded);
	return true;
    }
    return false;
}
#endif

//double getWeight0(){return weight0;}
//double getWeight(int vv){return weightSoln[vv];}
double getWeight(int vv){return vecWeights[vv];}


void printWeights(){
  cout << "Printing weight solutions in continuous MP: " << endl;
//try{
#if 0
  cout << weight0 << " ";
  for(int ii=0; ii<nVertices; ii++){
    cout << "  " << weightSoln[ii];
  }
  cout << endl;
#endif
  for(int ii=0; ii<nVertices; ii++){
    cout << "  " << vecWeights[ii];
  }
  cout << endl;
#if 0
}
catch(IloException& e){
  cout << "printWeights() error: " << e.getMessage() << endl;
  //cout << "Exception caught...Refreshing solution..." << endl;
  //refresh();
  e.end();
}
#endif

}
void printWeights(shared_ptr<ofstream> outStream){
  *(outStream) << "Printing weight solutions in continuous MP: " << endl;
//try{
#if 0
  *(outStream) << weight0 << " ";
  for(int ii=0; ii<nVertices; ii++){
    *(outStream) << "  " << weightSoln[ii];
  }
  *(outStream) << endl;
#endif
  for(int ii=0; ii<nVertices; ii++){
    *(outStream) << "  " << vecWeights[ii];
  }
  *(outStream) << endl;
}
#if 0
//catch(IloException& e){
  *(outStream) << "printWeights() error: " << e.getMessage() << endl;
  //*(outStream) << "Exception caught...Refreshing solution..." << endl;
  //refresh();
  e.end();
}

}
#endif

//x,y,x_vertex, and y_vertex should all be set to something meaningful
double updateGapVal(const double *omega){

#if 0

  gapVal=0.0;
  if(omega==NULL){for(int ii=0; ii<n1; ii++) {gapVal-= (c[ii])*(x_vertex[ii]-x[ii]);}}
  else{for(int ii=0; ii<n1; ii++) {gapVal-= (c[ii]+omega[ii])*(x_vertex[ii]-x[ii]);}}
  for(int jj=0; jj<n2; jj++) {gapVal-= d[jj]*(y_vertex[jj]-y[jj]);}

#endif
  gapVal=-LagrBd + evaluateSolution(omega);
#if 0
  if(omega==NULL){for(int ii=0; ii<n1; ii++) {gapVal+= (c[ii])*(x[ii]);}}
  else{for(int ii=0; ii<n1; ii++) {gapVal+= (c[ii]+omega[ii])*(x[ii]);}}
  for(int jj=0; jj<n2; jj++) {gapVal+= d[jj]*(y[jj]);}
#endif

  return gapVal;
}

double getGapVal(){return gapVal;}

#if 0
double computeXDispersion(const double *z){
    double disp = 0.0;
    for(int ii=0; ii<n1; ii++){
	disp+= (x_vertex[ii]-z[ii])*(x_vertex[ii]-z[ii]);
    }
    return disp;
}
#endif

//Return true if vertex added.
bool addVertex();

int findVVIndexNonProductive(){
  int idx = -1;
  for(int ii=0; ii<nVertices; ii++){
    if(vecWeights[ii] < 1e-20){
	idx=ii;
	break;
    }
  }
  return idx;
}

bool replaceVertexAtIndex(int iii);
#if 0
bool replaceVertexAtIndex(int iii){
    if(checkWhetherVertexIsRedundant()){
	return false;
    }

	//mpWeightVariables.add(IloNumVar(mpWeightConstraints[0](1.0)));
	//mpWeightVariables[nVertices].setBounds(0.0,1.0);
	assert(iii>=0);
	assert(iii<maxNVertices);
	mpWeightVariables[iii].setBounds(0.0,IloInfinity);
	for(int i=0; i<n1; i++) {
	   xVertices[iii][i] = x_vertex[i];
	}
	for(int i=0; i<n2; i++) { 
	   yVertices[iii][i] = y_vertex[i];
	}

	for(int i=0; i<n1; i++) {
	    mpVertexConstraints[i].setLinearCoef(mpWeightVariables[iii], x_vertex[i]);
	}

	//weightObjective[iii]=0.0;//.add(0.0);

	bestVertexIndex=iii;
	vecWeights[iii]=0.0;
	return true;
}
#endif

bool replaceOldestVertex(){
    if(checkWhetherVertexIsRedundant()){
	return false;
    }
//cout << "Oldest vertex index was: " << oldestVertexIndex;
    replaceVertexAtIndex(oldestVertexIndex);
    oldestVertexIndex++;
    oldestVertexIndex %= nVertices;
//cout << " ...but is now " << oldestVertexIndex << endl;
    return true;
}


void clearVertexHistory(){
  oldestVertexIndex=-1;
  nVertices=0;
  resetDispersionsToZero();
}


virtual void printLinCoeffs(){
return;
}

void printVertices(){
  assert(nVertices>0);
cout << "Printing Vertices: " << endl;
  for(int ii=0; ii<n1; ii++){
      for(int vv=0; vv<nVertices; vv++){
	cout << " " << xyVertices[vv][ii];
      }
      cout << endl;
  }
}
void printVertices(shared_ptr<ofstream> outStream){
  assert(nVertices>0);
*(outStream) << "Printing Vertices: " << endl;
  for(int ii=0; ii<n1; ii++){
      for(int vv=0; vv<nVertices; vv++){
	*(outStream) << " " << xyVertices[vv][ii];
      }
      *(outStream) << endl;
  }
}

void printX(){
 assert(n1>0);
 for(int ii=0; ii<n1; ii++){
  cout << " " << x[ii];
 }
 cout << endl;
}

void printY(){
 assert(n2>0);
 for(int jj=0; jj<n2; jj++){
  cout << " " << y[jj];
 }
 cout << endl;
}

virtual void printXBounds(){
    cout << "printXBounds() default: doing nothing..." << endl;
}
virtual void printYBounds(){
    cout << "printYBounds() default: doing nothing..." << endl;
}
virtual bool printModifiedYBounds(){
    cout << "printModifiedYBounds() default: doing nothing..." << endl;
    return false;
}

#if 0
void clearVertexHistory(){
  while(nVertices >0){
    removeBackVertex();
  }
	for(int i=0; i<n1; i++) {
	   xVertices[i].clear();
	}

	for(int i=0; i<n2; i++) { 
	   yVertices[i].clear();
	}
	baseWeightObj.clear();
	mpWeightVariables//.clear();
	mpVertexConstraints[i]
	weightSoln//.clear();
	weightObjective//.clear();
	nVertices=0;

		mpWeightVariables[0].end();
		mpWeightVariables.remove(0);
		weightObjective.remove(0);
}
#endif
virtual void updateSolnInfo()=0;

virtual void fixVarAt(int index, double fixVal){
    cout << "fixVarAt() is doing nothing..." << endl;
}
//virtual void fixXToZ(const double *z)=0;
virtual void fixXToZ(const double *z)=0;
virtual void unfixX(const double* origLBs, const double* origUBs)=0;

#if 0
void fixWeightToZero(int index){
    if(index >=0 && index < nVertices){
	mpWeightVariables[index].setBounds(0.0,0.0);
    }
}
void unfixWeight(int index){
    if(index >=0 && index < nVertices){
	mpWeightVariables[index].setBounds(0.0,IloInfinity);
    }
}
#endif


int getNVertices(){return nVertices;}

//Not tested, purpose is to modify quadratic master problem formulation to reflect removing of vertices.
#if 0
void removeBackVertex() {
    if(nVertices>0) {
		for(int i=0;i<n1;i++) xVertices[i].erase(xVertices[i].begin());

		for(int j=0;j<n2;j++) yVertices[j].erase(yVertices[j].begin());

		//mpModel.remove(mpWeightVariables[0]);
		mpWeightVariables[0].end();
		mpWeightVariables.remove(0);
		weightObjective.remove(0);
		baseWeightObj.erase(baseWeightObj.begin());
		nVertices--;
    }
}
#endif

#if 0
void setQuadraticTerm(const double scaling_const) {
	for (int i = 0; i < n1; i++) {
		quadraticTerm.setQuadCoef(mpAuxVariables[i], mpAuxVariables[i], 0.5 * scaling_const);
	}
}
#endif

#if 0
void setQuadraticTerm(const double rho, const double *scaling_vector) {
	for (int i = 0; i < n1; i++) {
		quadraticTerm.setQuadCoef(mpAuxVariables[i], mpAuxVariables[i], 0.5 * scaling_vector[i]);
	}
}
#endif

void solveMPLineSearch(const double *omega, const double *z, const double rho, const double *scaling_vector, int vertIndex=-1, double *z_average=NULL); 
void solveMPHistory(const double *omega, const double *z, const double *zLBs, const double *zUBs, 
	const double rho, const double *scaling_vector, bool updateDisp=false);
void solveMPHistoryTrustRegion(const double *omega, const double *z, const double *zLBs, const double *zUBs, 
	const double rho, const double *scaling_vector, bool updateDisp=false);
void solveMPHistory2Norm(const double *omega, const double *z, const double *zLBs, const double *zUBs, 
	const double rho, const double *scaling_vector, bool updateDisp=false);
void solveMPVertices(const double *omega, const double *z, const double rho, const double *scaling_vector);
//void computeWeightsForCurrentSoln(const double *z); 

void resetDispersionsToZero(){
    for(int ii=0; ii<n1+n2; ii++){ dispersions[ii]=0.0;}
}
double* computeDispersions(double *z=NULL){
    resetDispersionsToZero();
    if(z==NULL) z=x;
    for(int wI=0; wI<nVertices; wI++) {
        for(int ii=0; ii<n1; ii++){
	   dispersions[ii] += vecWeights[wI]*fabs(xyVertices[wI][ii] - z[ii]);
        }
        for(int jj=n1; jj<n1+n2; jj++){
	   dispersions[jj] += vecWeights[wI]*fabs(xyVertices[wI][jj] - x[jj]);
        }
    }
    return dispersions;
}


void printDispersions(){
  cout << "Printing dispersions for scenario " << tS << endl;
  for(int ii=0; ii<n1; ii++){ cout << " " << dispersions[ii];}
  cout << endl;
  for(int ii=n1; ii<n1+n2; ii++){ cout << " " << dispersions[ii];}
  cout << endl;
}

void checkWeightsSumToOne(){
    double weightSum=0.0;
    for(int wI=0; wI<nVertices; wI++) {
	weightSum += vecWeights[wI];
    }
    cout << "Weight sum: " << weightSum << endl;
}

//void getLagrangianGradient(SMIP_qu_getLagrangianGradient* question, SMIP_ans_getLagrangianGradient* answer);

double getDefaultPenaltyParameter();

int getNumVertices(){return nVertices;}
double * getX(){return x;}
double * getY(){return y;}
double * getXVertex(){return x_vertex;}
double * getYVertex(){return y_vertex;}

void setXToVertex(){
    resetDispersionsToZero();
    //for(int i=0;i<n1;i++) x[i]=x_vertex[i];
    for(int i=0;i<n1;i++) x[i]=xyVertices[bestVertexIndex][i];
}
//void setYToVertex(){for(int i=0;i<n2;i++) y[i]=y_vertex[i];}
void setYToVertex(){for(int i=n1;i<n1+n2;i++) x[i]=xyVertices[bestVertexIndex][i];}

void refresh(const double *omega, const double *z, const double rho, const double *scaling_vector){
    
    optimiseLagrOverVertexHistory(omega); //prepares next call of solveMPLineSearch(omega,z,rho,scaling_vector)
    solveMPLineSearch(omega, z, rho, scaling_vector);

}

virtual void polishSolution(){
cerr << "polishSolution(): Doing nothing." << endl;
}

#if 0
void polishWeights(){
#if 0
cout << "PolishWeights(): before" << endl;
printWeights();
#endif
//assert(weight0 >=0 && weight0 <=1);
	double weightSum=0.0;
	if(weight0 >=0 && weight0 <=1){
    	    weightSum += weight0;
	}
	else if(weight0 > 1.0){
//cout << "Flagging: weight0 = " << weight0 << endl;
    	    weight0=1.0;
    	    weightSum += weight0;
	}
	else{
//cout << "Flagging: weight0 = " << weight0 << endl;
	    weight0=0.0;
	}

	for(int wI=0; wI<nVertices; wI++) {
	    if(weightSoln[wI] >=0 && weightSoln[wI] <=1){
    	        weightSum += weightSoln[wI];
	    }
	    else if(weightSoln[wI] > 1.0){
//cout << "Flagging: weight[" << wI << "] = " << weightSoln[wI] << endl;
    	        weightSoln[wI]=1.0;
    	        weightSum += weightSoln[wI];
	    }
	    else{
//cout << "Flagging: weight[" << wI << "] = " << weightSoln[wI] << endl;
		weightSoln[wI]=0.0;
	    }
	}
if(weightSum <= 0.99 || weightSum >= 1.01){
printWeights();
}
	assert(weightSum > 0.99);
	assert(weightSum < 1.01);
	weight0 /= weightSum;
	for(int wI=0; wI<nVertices; wI++) {
	    weightSoln[wI] /=weightSum;
	}
#if 0
cout << "PolishWeights(): after" << endl;
printWeights();
weightSum=weight0;
for(int wI=0; wI<nVertices; wI++){weightSum+= weightSoln[wI];}
cout << "PolishWeights(): sum: " << weightSum << endl;
#endif
}
#endif

double getLagrBd(){return LagrBd;}
double getProbabilities(){return pr;}

double updateALValues(const double *omega, const double *z, const double rho, const double *scaling_vector){
    ALVal = 0.0;
    sqrNormDiscr = 0.0;
    for(int ii=0; ii<n1; ii++) {
		sqrNormDiscr += rho*scaling_vector[ii]*(x[ii]-z[ii])*(x[ii]-z[ii]);
		if(omega==NULL){ALVal += (c[ii])*x[ii];}
		else{ALVal += (c[ii]+omega[ii])*x[ii];}
    }
    ALVal += 0.5*sqrNormDiscr;
    
    for(int jj=0; jj<n2; jj++) {
		ALVal += d[jj]*y[jj];
    }
    return ALVal;
}
double getALVal(){return ALVal;}
double getSqrNormDiscr(){return sqrNormDiscr;}
virtual bool checkSolnForFeasibility(const double *soln, vector<double> &constrVec){
printColTypesFirstStage();
    return false;
}


};



class PSCGScen_SMPS : public PSCGScen{
public:
PSCGScen_SMPS(int nThreads=1):PSCGScen(nThreads),LagrMIPInterface_(NULL){;}

//copy constructor
PSCGScen_SMPS(const PSCGScen_SMPS &other):PSCGScen(other),LagrMIPInterface_(NULL){;}

int initialiseSMPS(DecTssModel &smpsModel, int scenario);

virtual void fixVarAt(int index, double fixVal){
      LagrMIPInterface_->setColBounds(index,fixVal,fixVal);
}


//virtual int initialLPSolve(const double* omega=NULL);
virtual int solveLagrangianProblem(const double* omega=NULL, bool doInitialSolve=false);

//virtual int solveAugmentedLagrangianMIP(const double* omega, const double* z, const double rho, const double* scal);
virtual int solveFeasibilityProblem();
virtual void polishSolution(){
  for(int ii=0; ii<n1; ii++){
      if(LagrMIPInterface_->getColLower()[ii] > x_vertex[ii]){
cerr << "polishSolution(): Flagging" << endl;
	//x_vertex[ii] = LagrMIPInterface_->getColLower()[ii];
      }
      if(LagrMIPInterface_->getColUpper()[ii] < x_vertex[ii]){
cerr << "polishSolution(): Flagging" << endl;
	//x_vertex[ii] = LagrMIPInterface_->getColUpper()[ii];
      }
      //if(x_vertex[ii]==-0) x_vertex[ii]=0.0;
      if(getColTypes()[ii]!='C')  x_vertex[ii]=round(x_vertex[ii]);
  }
  for(int ii=0; ii<n2; ii++){
      if(LagrMIPInterface_->getColLower()[n1+ii] > y_vertex[ii]){
cerr << "polishSolution(): Flagging" << endl;
	//y_vertex[ii] = LagrMIPInterface_->getColLower()[n1+ii];
      }
      if(LagrMIPInterface_->getColUpper()[n1+ii] < y_vertex[ii]){
cerr << "polishSolution(): Flagging" << endl;
	//y_vertex[ii] = LagrMIPInterface_->getColUpper()[n1+ii];
      }
      //if(y_vertex[ii]==-0) y_vertex[ii]=0.0;
      if(getColTypes()[n1+ii]!='C')  y_vertex[ii]=round(y_vertex[ii]);
  }
}
virtual void setSolverStatus(){
	OsiCpxSolverInterface *osi = LagrMIPInterface_;

    	if (osi->isAbandoned()) {
#if 0
        std::cout << "BOUND: is abandoned" << std::endl;
#endif
        	solverStatus_ = PSCG_ABANDONED;
    	}
    	else if (osi->isProvenOptimal()) {
#if 0
        std::cout << "BOUND: is lp optimal" << std::endl;
#endif
        	solverStatus_ = PSCG_OPTIMAL;
        	//PSCGNodeDesc *desc = dynamic_cast<PSCGNodeDesc*>(desc_);


    	}
	else if (osi->isProvenPrimalInfeasible()) {
#if 0
        std::cout << "BOUND: is primal inf" << std::endl;
#endif
        	solverStatus_ = PSCG_PRIMAL_INF;
    	}
    	else if (osi->isProvenDualInfeasible()) {
#if 0
        std::cout << "BOUND: is dual inf" << std::endl;
#endif
        	solverStatus_ = PSCG_DUAL_INF;
    	}
    	else if (osi->isPrimalObjectiveLimitReached()) {
#if 0
        	std::cout << "BOUND: is primal limit" << std::endl;
#endif
        	solverStatus_ = PSCG_PRIMAL_LIM;
    	}
    	else if (osi->isDualObjectiveLimitReached()) {
#if 0
        std::cout << "BOUND: is dual limit" << std::endl;
#endif
        	solverStatus_ = PSCG_DUAL_LIM;
    	}
    	else if (osi->isIterationLimitReached()) {
#if 0
        std::cout << "BOUND: is iter limit" << std::endl;
#endif
        	solverStatus_ = PSCG_ITER_LIM;
    	}
    	else {//handle other cases
		int errCode = getCPLEXErrorStatus();
		switch(errCode){
		  case CPXMIP_INFEASIBLE:
		      solverStatus_ = PSCG_PRIMAL_INF; 
		      break;
		  case CPXMIP_OPTIMAL_INFEAS:
		      break;
		  default:
        	    std::cout << "UNKNOWN SOLVER STATUS" << std::endl;
		    std::cout << "CPLEX Error Code is: " << getCPLEXErrorStatus() << endl;
        	    assert(0);
		}
    	}
}
#if 0
virtual void upBranchOnVar(int varIndex, double bound){
    if(varIndex >= 0 && varIndex < n1+n2){
	LagrMIPInterface_->setColLower(varIndex,bound);
	//cout << "Col bds at: " << varIndex << " are " << LagrMIPInterface_->getColLower() << " and " << LagrMIPInterface_->getColUpper() << endl;
    }
}
virtual void setLBs(const double *lbs, int nLBs){
    for(int ii=0; ii<nLBs; ii++){
	LagrMIPInterface_->setColLower(ii,lbs[ii]);
    }
}
virtual void downBranchOnVar(int varIndex, double bound){
    if(varIndex >= 0 && varIndex < n1+n2){
	LagrMIPInterface_->setColUpper(varIndex,bound);
    }
}
virtual void setUBs(const double *ubs, int nUBs){
    for(int ii=0; ii<nUBs; ii++){
	LagrMIPInterface_->setColUpper(ii,ubs[ii]);
    }
}
#endif
virtual double getLB(const int ind){
    return LagrMIPInterface_->getColLower()[ind];
}
virtual double getUB(const int ind){
    return LagrMIPInterface_->getColUpper()[ind];
}
virtual void setBound(const int ind, const double lb, const double ub, bool indNonRedundant){
    //Save the current bound, if it is not already saved
#if 0
    if(!indNonRedundant){
        int ii;
        for(ii=0; ii<branchIndices.size(); ii++){
	    if(ind==ii) break;
    	}
    	if(ii==branchIndices.size()){
	    branchIndices.push_back(ind);
	    restoreLBs.push_back(getLB(ind));
	    restoreUBs.push_back(getUB(ind));
    	}
    }
    else{
	    branchIndices.push_back(ind);
	    restoreLBs.push_back(getLB(ind));
	    restoreUBs.push_back(getUB(ind));
    }
#endif
    LagrMIPInterface_->setColLower(ind,lb);
    LagrMIPInterface_->setColUpper(ind,ub);

}
#if 0
virtual void setBounds(const vector<int> &inds, const vector<double> &lbs, const vector<double> &ubs){
    restoreBounds();
    branchIndices.resize(inds.size());
    std::copy(inds.begin(),inds.end(), branchIndices.begin());


    branchLBs.resize(inds.size());
    std::copy(lbs.begin(),lbs.end(), branchLBs.begin());
    branchUBs.resize(inds.size());
    std::copy(ubs.begin(),ubs.end(), branchUBs.begin());

    restoreLBs.resize(inds.size());
    restoreUBs.resize(inds.size());

    for(int ii=0; ii<inds.size(); ii++){
	restoreLBs[ii]=LagrMIPInterface_->getColLower()[inds[ii]];
	restoreUBs[ii]=LagrMIPInterface_->getColUpper()[inds[ii]];
	LagrMIPInterface_->setColLower(inds[ii],lbs[ii]);
	LagrMIPInterface_->setColUpper(inds[ii],ubs[ii]);
    }

}
#endif
virtual void restoreBounds(){
#if 0
vector<int> branchIndices;
vector<double> restoreLBs;
vector<double> restoreUBs;
    for(int ii=0; ii<branchIndices.size(); ii++){
	LagrMIPInterface_->setColLower(branchIndices[ii],restoreLBs[ii]);
	LagrMIPInterface_->setColUpper(branchIndices[ii],restoreUBs[ii]);
    }
    restoreLBs.clear();
    restoreUBs.clear();
    branchIndices.clear();
#endif
    for(int iii=0; iii<n1+n2; iii++){
	LagrMIPInterface_->setColLower(iii,origLBs_[iii]);
	LagrMIPInterface_->setColUpper(iii,origUBs_[iii]);
	assert(origLBs_[iii]==getLB(iii));
	assert(origUBs_[iii]==getUB(iii));
    }
}

//Elements are 'C', 'I', 'B'
virtual const char* getColTypes(){
    return LagrMIPInterface_->getCtype();
} 

virtual bool varIsInt(int index){return LagrMIPInterface_->getCtype()[index]!='C';}

virtual int getCPLEXErrorStatus(){
	OsiCpxSolverInterface *osi = LagrMIPInterface_;
	return CPXgetstat(osi->getEnvironmentPtr(), osi->getLpPtr());
}

virtual double getMIPBestNodeVal(){
	double retval=0.0;
	OsiCpxSolverInterface *osi = LagrMIPInterface_;
	CPXgetbestobjval(osi->getEnvironmentPtr(), osi->getLpPtr(), &retval);
	return retval;
}
virtual void setInitialSolution(const int *indices, const double *startSol){
    OsiCpxSolverInterface *osi = LagrMIPInterface_;
    //LagrMIPInterface_->setColSolution(startSol);
    int beg=0;
    int effortLevel = CPX_MIPSTART_REPAIR;
    char *names=NULL;
    //for(int ii=0; ii<n1+n2; ii++) indices[ii]=ii;
    CPXaddmipstarts(osi->getEnvironmentPtr(), osi->getLpPtr(), 1, n1+n2, &beg, indices, startSol,&effortLevel,&names);
}

void setCPXMIPParameters(){
	OsiCpxSolverInterface *osi = LagrMIPInterface_;
	//osi->setIntParam(OsiOutputControl,0);
	//osi->setIntParam(OsiMIPOutputControl,0);
	osi->setHintParam(OsiDoReducePrint,true);

	//if (nThreads >= 0) { osi->setIntParam(OsiParallelThreads, nThreads); }
	//CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_Preprocessing_BoundStrength, 1);
	osi->setHintParam(OsiDoPresolveInInitial,true);
	osi->setHintParam(OsiDoScale,true);
	osi->setHintParam(OsiDoCrash,true);
	osi->setHintParam(OsiLastHintParam, true);
	//setGapTolerances(1e-9,1e-9);

	//osi->setDblParam(OsiDualTolerance, 1e-9);

//#define MIP_TOL 1e-9
	//osi->setIntParam(OsiOutputControl,0);
	//osi->setIntParam(OsiMIPOutputControl,0);
	//if (nThreads >= 0) { osi->setIntParam(OsiParallelThreads, nThreads); }
	CPXsetintparam( osi->getEnvironmentPtr(), CPXPARAM_Parallel, CPX_PARALLEL_DETERMINISTIC); //no iteration message until solution
	if (nThreads >= 0) CPXsetintparam( osi->getEnvironmentPtr(), CPXPARAM_Threads, nThreads); //no iteration message until solution
	CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_AbsMIPGap, MIP_TOL);
	CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_MIPGap, MIP_TOL*1e-3);
	CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_Integrality, MIP_TOL);
	CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_Simplex_Tolerances_Optimality, MIP_TOL);
	CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_Simplex_Tolerances_Feasibility, MIP_TOL);
        CPXsetintparam( osi->getEnvironmentPtr(), CPXPARAM_Simplex_Display, 0); //no iteration message until solution
        CPXsetintparam( osi->getEnvironmentPtr(), CPXPARAM_Tune_Display, 0); //turn off display
        CPXsetintparam( osi->getEnvironmentPtr(), CPX_PARAM_SCRIND, CPX_OFF); //turn off display
        CPXsetintparam( osi->getEnvironmentPtr(), CPXPARAM_Barrier_Display, 0); //turn off display
	osi->messageHandler()->setLogLevel(0);

}

virtual void setMIPPrintLevel(int outputControl=0, int outputControlMIP=0, bool doReducePrint=true){
	OsiCpxSolverInterface *osi = LagrMIPInterface_;
	//osi->setIntParam(OsiOutputControl,outputControl);
	//osi->setIntParam(OsiMIPOutputControl,outputControlMIP);
	osi->setHintParam(OsiDoReducePrint,doReducePrint);
}

int setGapTolerances(double absGap, double relGap){
	OsiCpxSolverInterface *osi = LagrMIPInterface_;
	CPXsetdblparam( osi->getEnvironmentPtr(), CPX_PARAM_EPAGAP, absGap );
	return CPXsetdblparam( osi->getEnvironmentPtr(), CPX_PARAM_EPGAP, relGap );
}

virtual void printXBounds(){
cout << "Printing subproblem bounds: " << endl;
  for(int ii=0; ii<n1; ii++){
      cout << " (" << LagrMIPInterface_->getColLower()[ii] << "," << LagrMIPInterface_->getColUpper()[ii] << ")";
  }
  cout << endl;
}
virtual void printYBounds(){
cout << "Printing subproblem bounds: " << endl;
  for(int ii=0; ii<n2; ii++){
      cout << " (" << LagrMIPInterface_->getColLower()[n1+ii] << "," << LagrMIPInterface_->getColUpper()[n1+ii] << ")";
  }
  cout << endl;
}
virtual bool printModifiedYBounds(){
    bool found=false;
#if 0
    for(int ii=0; ii<branchIndices.size(); ii++){
	if(branchIndices[ii]>n1){
	    found=true;
      	    cout << " (" << branchIndices[ii] << "," << LagrMIPInterface_->getColLower()[branchIndices[ii]] << "," << LagrMIPInterface_->getColUpper()[branchIndices[ii]] << ")";
	}
    }
#endif
    return found;
}
virtual void printLinCoeffs(){
cout << "Printing linear coefficients: " << endl;
  for(int ii=0; ii<n1+n2; ii++){
    cout << " " << LagrMIPInterface_->getObjCoefficients()[ii];
  }
  cout << endl;
}
virtual void printLagrSoln(){
cout << "Printing col solns: " << endl;
  const double* solution = LagrMIPInterface_->getColSolution();
  for(int ii=0; ii<n1+n2; ii++){
    cout << " " << solution[ii];
  }
  cout << endl;
}
double computeMIPVal(const double *omega){
  const double* solution = LagrMIPInterface_->getColSolution();
  const double* coeff = LagrMIPInterface_->getObjCoefficients();
  double retVal=0.0;
 if(omega==NULL){
  for(int ii=0; ii<n1+n2; ii++){
    retVal +=solution[ii]*coeff[ii];
  }
 }
 else{
  for(int ii=0; ii<n1; ii++){
    retVal +=solution[ii]*(coeff[ii]+omega[ii]);
  }
  for(int ii=n1; ii<n2; ii++){
    retVal +=solution[ii]*coeff[ii];
  }
 }
  return retVal;
}

#if 0
int changeFromMIQPToMILP(){
	OsiCpxSolverInterface *osi = LagrMIPInterface_;
        if(CPXgetprobtype(osi->getEnvironmentPtr(),osi->getLpPtr()) == CPXPROB_MIQP){
	    return CPXchgprobtype(osi->getEnvironmentPtr(), osi->getLpPtr(), CPXPROB_MILP);
	}
	else{return 0;}

}
int changeFromMILPToMIQP(){
	OsiCpxSolverInterface *osi = LagrMIPInterface_;
        if(CPXgetprobtype(osi->getEnvironmentPtr(),osi->getLpPtr()) == CPXPROB_MILP){
	    return CPXchgprobtype(osi->getEnvironmentPtr(), osi->getLpPtr(), CPXPROB_MIQP);
	}
	else{
	    return 0;
	}
}
#endif


virtual void printColTypesFirstStage(){
    for(int i=0; i<n1; i++){
	printColTypeFirstStage(i);
    }
    cout << endl;
}
virtual void printColTypeFirstStage(int i){
	if(getColTypes()[i]=='C')
	    cout << " C";
	else if(getColTypes()[i]=='B')
	    cout << " B";
	else if(getColTypes()[i]=='I')
	    cout << " I";
}
virtual void printColumnBound(int i){
	cout << " (" << LagrMIPInterface_->getColLower()[i] << "," << LagrMIPInterface_->getColUpper()[i] << ")";
}
virtual void printColBds(){
	for(int i=0; i<n1+n2; i++) printColumnBound(i);
	cout << endl;
}

OsiCpxSolverInterface* getOSI(){return LagrMIPInterface_;}

virtual bool checkSolnForFeasibility(const double *soln, vector<double> &constrVec){
    const CoinPackedMatrix *mat = LagrMIPInterface_->getMatrixByCol();
    if(mat->getNumRows() > constrVec.size()) constrVec.resize(mat->getNumRows());
    double *constrVals = constrVec.data();
    const double* rowLHS = LagrMIPInterface_->getRowLower();
    const double* rowRHS = LagrMIPInterface_->getRowUpper();
    mat->times(soln,constrVals);
#if 0
for(int ii=0; ii<mat->getNumRows(); ii++) cout << rowLHS[ii] << " <= " << constrVals[ii] << " <= " << rowRHS[ii] << endl;
#endif
    for(int ii=0; ii<mat->getNumRows(); ii++){

	if( !( (rowLHS[ii] <=  constrVals[ii]+MIP_TOL) && (constrVals[ii]-MIP_TOL <= rowRHS[ii]) ) ) 
	{
#if 1
//for(int ii=0; ii<mat->getNumRows(); ii++) cout << rowLHS[ii] << " <= " << constrVals[ii] << " <= " << rowRHS[ii] << endl;
//cout << rowLHS[ii] << " <= " << constrVals[ii] << " <= " << rowRHS[ii] << endl;

#endif
	    return false;
	}
    }
    return true;
}

#if 0
virtual void fixXToZ(const double *z){
   for(int ii=0; ii<n1; ii++){
      LagrMIPInterface_->setContinuous(ii);
      LagrMIPInterface_->setColBounds(ii,z[ii],z[ii]);
   }
   return; 
}
#endif
virtual void fixXToZ(const double *z){
   for(int ii=0; ii<n1; ii++){
      //LagrMIPInterface_->setContinuous(ii);
      if(getColTypes()[ii]!='C'){
      //if(colTypes[ii]=='I' || colTypes[ii]=='B'){
          LagrMIPInterface_->setColBounds(ii,round(z[ii]),round(z[ii]));
      }
      else{
          LagrMIPInterface_->setColBounds(ii,z[ii],z[ii]);
      }
   }
   return; 
}

//undo the fixing of fixXToZ(), for when there is no need to reset variable types
virtual void unfixX(const double* origLBs, const double* origUBs){
   for(int ii=0; ii<n1; ii++){
      LagrMIPInterface_->setColBounds(ii,origLBs[ii],origUBs[ii]);
   }
   //cout << endl;
   return;

}
//undo the fixing of fixXToZ(), includes resetting variable types
#if 0
virtual void unfixX(const double* origLBs, const double* origUBs, const char* colTypes){
   for(int ii=0; ii<n1; ii++){
      //cout << "  " << colTypes[ii];
      //if(colTypes[ii]=='I' || colTypes[ii]=='B'){
      if(getColTypes()[ii]!=0){
	 LagrMIPInterface_->setInteger(ii);
      }
      else{
	 LagrMIPInterface_->setContinuous(ii);
      }
      LagrMIPInterface_->setColBounds(ii,origLBs[ii],origUBs[ii]);
   }
   //cout << endl;
   return;
}
#endif

virtual int solveLagrangianWithXFixedToZ(const double *z, const double *omega, double *origLBs, double *origUBs){
//cout << "*********" << endl;
//printColTypesFirstStage();
//printColBds();
   for(int ii=0; ii<n1; ii++){
	origLBs[ii]=getLB(ii);
	origUBs[ii]=getUB(ii);
   }
   fixXToZ(z);
//printColTypesFirstStage();
//printColBds();
   OsiCpxSolverInterface *osi = LagrMIPInterface_;
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_AbsMIPGap, MIP_TOL*1e3);
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_MIPGap, MIP_TOL);
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_Integrality, MIP_TOL*1e3);
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_Simplex_Tolerances_Optimality, MIP_TOL*1e3);
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_Simplex_Tolerances_Feasibility, MIP_TOL*1e3);

   solverStatus_ = solveLagrangianProblem(omega,true);

   //unfixX(origLBs, origUBs, colTypes); 
   unfixX(origLBs, origUBs); 
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_AbsMIPGap, MIP_TOL);
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_MIPGap, MIP_TOL*1e-3);
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_MIP_Tolerances_Integrality, MIP_TOL);
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_Simplex_Tolerances_Optimality, MIP_TOL);
   CPXsetdblparam( osi->getEnvironmentPtr(), CPXPARAM_Simplex_Tolerances_Feasibility, MIP_TOL);
//printColTypesFirstStage();
//printColBds();
   return solverStatus_;
}
#if 0
virtual int solveFeasibilityProblemWithXFixedToZ(const double *z, const double *origLBs, const double *origUBs, const char *colTypes){
   fixXToZ(z,colTypes);
   solverStatus_ = solveFeasibilityProblem();
   unfixX(origLBs, origUBs); 
   return solverStatus_;
}
#endif

virtual void updateSolnInfo(){
	OsiCpxSolverInterface *osi = LagrMIPInterface_;
	if(solverStatus_==PSCG_OPTIMAL || solverStatus_==PSCG_ITER_LIM){	
		if(solverStatus_==PSCG_ITER_LIM) cerr << "Flagging: SMPS MIP solver iteration limit reached." << endl;
		const double* solution = osi->getColSolution();
		//LagrBd = osi->getObjValue()*osi->getObjSense();
		memcpy(x_vertex,solution,(n1+n2)*sizeof(double));
		//memcpy(y_vertex,solution+n1,n2*sizeof(double));
		//for(int ii=0; ii<n1; ii++){ (x_vertex[ii]);}
		//for(int ii=0; ii<n2; ii++){ (y_vertex[ii]);}
		//polishSolution();
	}
	else{
		cerr << "Flagging: SMPS MIP solver indicated isProvenOptimal() == false." << endl;
		cerr << "CPLEX error code: " << getCPLEXErrorStatus() << endl;
	        assert(solverStatus_==PSCG_OPTIMAL || solverStatus_==PSCG_ITER_LIM);	
	}
}


private:
OsiCpxSolverInterface *LagrMIPInterface_;

};

#endif
