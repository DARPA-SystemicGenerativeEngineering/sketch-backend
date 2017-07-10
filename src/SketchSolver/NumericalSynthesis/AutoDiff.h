#pragma once
#include <gsl/gsl_vector.h>
#include "ValueGrad.h"
#include "BooleanNodes.h"
#include "BooleanDAG.h"
#include "NodeVisitor.h"
#include "VarStore.h"
#include <map>
#include "FloatSupport.h"

#include <iostream>

#define RELAX_BOOL 1

using namespace std;

class AutoDiff: NodeVisitor
{
	FloatManager& floats;
	BooleanDAG& bdag;
	map<string, int> floatCtrls; // Maps float ctrl names to indices within grad vector
	map<string, int> boolCtrls; // Maps bool ctrl names to indices within grad vector
	int nctrls; // number of float ctrls
	gsl_vector* ctrls; // ctrl values
	vector<ValueGrad*> values; // Keeps track of values along with gradients for each node
	vector<DistanceGrad*> distances; // Keeps track of distance metric for boolean nodes
	map<int, int> inputValues; // Maps node id to values set by the SAT solver
	double error = 0.0;
	gsl_vector* errorGrad;
	
	int DEFAULT_INP = -1;
	int assertCtr;
	bool foundFailure;
	
	static constexpr float ASSERT_PENALTY = 1.0;
	
public:
	int failedAssert;
	
	AutoDiff(BooleanDAG& bdag_p, FloatManager& _floats, const map<string, int>& floatCtrls_p, const map<string, int>& boolCtrls_p);
	~AutoDiff(void);
	
	virtual void visit( SRC_node& node );
	virtual void visit( DST_node& node );
	virtual void visit( CTRL_node& node );
	virtual void visit( PLUS_node& node );
	virtual void visit( TIMES_node& node );
	virtual void visit( ARRACC_node& node );
	virtual void visit( DIV_node& node );
	virtual void visit( MOD_node& node );
	virtual void visit( NEG_node& node );
	virtual void visit( CONST_node& node );
	virtual void visit( LT_node& node );
	virtual void visit( EQ_node& node );
	virtual void visit( AND_node& node );
	virtual void visit( OR_node& node );
	virtual void visit( NOT_node& node );
	virtual void visit( ARRASS_node& node );
	virtual void visit( UFUN_node& node );
	virtual void visit( TUPLE_R_node& node );
	virtual void visit( ASSERT_node& node );
	
	double run(const gsl_vector* ctrls_p, map<int, int>& inputValues_p, gsl_vector* errorGrad_p);
	
	void setvalue(bool_node& bn, ValueGrad* v) {
		values[bn.id] = v;
	}
	
	ValueGrad* v(bool_node& bn) {
		ValueGrad* val = values[bn.id];
		if (val == NULL) {
			gsl_vector* g = gsl_vector_alloc(nctrls);
			val = new ValueGrad(0, g);
			setvalue(bn, val);
		}
		return val;
	}
	
	ValueGrad* v(bool_node* bn) {
		return v(*bn);
	}
	
	void setdistance(bool_node& bn, DistanceGrad* d) {
		distances[bn.id] = d;
	}
	
	DistanceGrad* d(bool_node& bn) {
		DistanceGrad* dist = distances[bn.id];
		if (dist == NULL) {
			gsl_vector* g = gsl_vector_alloc(nctrls);
			dist = new DistanceGrad(0, g);
			setdistance(bn, dist);
		}
		return dist;
	}
	
	DistanceGrad* d(bool_node* bn) {
		return d(*bn);
	}
	
	void print() {
		for (int i = 0; i < bdag.size(); i++) {
			cout << bdag[i]->lprint() << " ";
			if (bdag[i]->getOtype() == OutType::FLOAT) {
				cout << v(bdag[i])->print() << endl;
			} else {
				if (d(bdag[i])->set) {
					cout << d(bdag[i])->print() << endl;
				} else {
					cout << v(bdag[i])->print() << endl;
				}
			}
		}
	}
	void printFull() {
		for (int i = 0; i < bdag.size(); i++) {
			cout << bdag[i]->lprint() << endl;
			if (bdag[i]->getOtype() == OutType::FLOAT) {
				cout << v(bdag[i])->printFull() << endl;
			} else {
				if(d(bdag[i])->set) {
					cout << d(bdag[i])->printFull() << endl;
				} else {
					cout << v(bdag[i])->printFull() << endl;
				}
			}
		}
	}
	
	bool isFloat(bool_node& bn) {
		return (bn.getOtype() == OutType::FLOAT);
	}
	
	bool isFloat(bool_node* bn) {
		return (bn->getOtype() == OutType::FLOAT);
	}
	
	int getInputValue(bool_node& bn) {
		if (inputValues.find(bn.id) != inputValues.end()) {
			int val = inputValues[bn.id];
			Assert(val == 0 || val == 1, "NYI: Integer values");
			return val;
		} else {
			return DEFAULT_INP;
		}
	}
	
	int getInputValue(bool_node* bn) {
		return getInputValue(*bn);
	}
	void computeError(float dist, int expected, gsl_vector* dg, bool_node& node, bool relax = false);
	
};
