#pragma once

#include "CommandLineArgs.h"
#include "BooleanDAG.h"
#include "BooleanDAGCreator.h"
#include "SATSolver.h"

#include "DagFunctionInliner.h"
#include "DagElimUFUN.h"
#include "BackwardsAnalysis.h"
#include "DagOptimizeCommutAssoc.h"
#include "CEGISSolver.h"
#include "ABCSATSolver.h"
#include "InputReader.h" // INp yylex_init, yyparse, etc.


#include <sstream>

extern timerclass solution;
extern timerclass modelBuilding;

using namespace std;

class InterpreterEnvironment
{
	typedef enum {READY, UNSAT} STATUS;
	STATUS status;
	CommandLineArgs& params;
	map<string, BooleanDAG*> functionMap;
	map<string, int> currentControls;
	BooleanDAG * bgproblem;
	SolverHelper* finder;
	SATSolver* _pfind;
	int assertionStep;
	string sessionName;
	string findName(){
		stringstream s;
		s<<sessionName;
		s<<"_find";
		return s.str();
	}
	string checkName(){
		stringstream s;
		s<<sessionName;
		s<<"_check_"<<assertionStep;
		return s.str();
	}
	string basename(){
		stringstream s;
		s<<sessionName<<"_"<<assertionStep;
		return s.str();
	}

	string procFname(string& fname){
		int x1 = fname.find_last_of("/");
		int x2 = fname.find_last_of("\\");
		int x3 = fname.find_last_of(".");

		x1 = x1>x2? x1: x2;
		x3 = x3 > 0? x3 : fname.size();
		++x1;
		
		return fname.substr(x1, x3-x1);
	}

	BooleanDAG* runOptims(BooleanDAG* result);

public:
	InterpreterEnvironment(CommandLineArgs& p): bgproblem(NULL), params(p), status(READY), assertionStep(0){
		_pfind = SATSolver::solverCreate(params.synthtype, SATSolver::FINDER, findName());
		finder = new SolverHelper(*_pfind);
		finder->setMemo(p.setMemo && p.synthtype == SATSolver::MINI);
		sessionName = procFname(params.inputFname);			  
	}
	
	void addFunction(const string& name, BooleanDAG* fun){
		functionMap[name] = fun;
	}	

	BooleanDAGCreator* newFunction(const string& name){
		BooleanDAG* tmp = new BooleanDAG(name);
		cout<<"CREATING "<<name<<endl;
		if(functionMap.count(name)>0){
			delete functionMap[name];
		}
		functionMap[name] = tmp;
		return new BooleanDAGCreator(tmp);		
	}


	void printControls(ostream& out){
		for(map<string, int>::iterator it = currentControls.begin(); it != currentControls.end(); ++it){
			out<<it->first<<"\t"<<it->second<<endl;
		}
	}


	int runCommand(const string& cmd, list<string*>& parlist);


	void printControls(const string& s){
		string tmp = s;
		if(tmp == ""){
			tmp = params.outputFname;
		}
		ofstream out(tmp.c_str());
		printControls(out);
	}

	BooleanDAG* prepareMiter(BooleanDAG* spec, BooleanDAG* sketch);

	void doInline(BooleanDAG& dag, map<string, BooleanDAG*> functionMap, int i);

	BooleanDAG* getCopy(const string& s){
		return functionMap[s]->clone();
	}

	/**
		This function takes ownership of dag. After this, 
		dag will be useless, and possibly deallocated.
	*/
	int assertDAG(BooleanDAG* dag, ostream& out);

	virtual ~InterpreterEnvironment(void);
};