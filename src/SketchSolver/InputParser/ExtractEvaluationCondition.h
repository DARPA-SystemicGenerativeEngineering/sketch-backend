#ifndef EXTRACTEVALUATIONCONDITION_H_
#define EXTRACTEVALUATIONCONDITION_H_

#include "BooleanDAG.h"
#include <set>

using namespace std;


/**
 * The boolean formula corresponding to the guard for a node 
 * is represented as a network of t-nodes.
 * 
 * Each t-node corresponds to a clause of the form 'control==nidx'
 * 
 * 
 * 
 */


class t_node{
	public:
	bool_node* control;
	int nidx;	
	vector<t_node*> children;
	bool_node* node;
	
	
	t_node(bool_node* c): control(c),nidx(0), node(NULL){};		
	
	 
	
	void print(ostream& out){
		set<t_node*> ts;
		out<<" BEGIN ----------"<<endl;
		print(out, ts);
		out<<" END   ----------"<<endl;
	}
	
	string tostring(){
		string tmp = control->get_name();
		tmp += "_";
		if(nidx==0){
			tmp += "_NOT";	
		}			
		return tmp;
	}
	
	void print(ostream& out, set<t_node*>& ts){
		if( ts.find(this) == ts.end()){
			ts.insert(this);
			for(int i=0; i<children.size(); ++i){
				out<<tostring()<<" -> "<<children[i]->tostring()<<endl;
				children[i]->print(out, ts);	
			}			
		}
	}	
	
	
	bool_node* guard(vector<bool_node*>& store){
		if( nidx == 0){
			NOT_node* nn = new NOT_node();
			nn->mother = control;
			nn->addToParents();
			store.push_back(nn);
			return nn;
		}else{
			return control;	
		}
	}
	
	
	bool_node* childDisjunct(vector<bool_node*>& store){
		Assert( children.size() > 0 , "This function is being misused");
		bool_node* cur = children[0]->node;
		
		for(int j=1; j<children.size(); ++j){
			OR_node* on = new OR_node();
			on->mother = cur;
			on->father = children[j]->node;
			Assert( on->mother != NULL, "Mother can't be null");
			Assert( on->father != NULL, "Father can't be null");
			on->addToParents();
			cur = on;
			store.push_back(on);	
		}
		return cur;
	}
	
	bool_node* circuit(vector<bool_node*>& store){		
		if( children.size() == 0){
			node = guard(store);
		}else{
			bool_node* cur = childDisjunct(store);
			AND_node* anode = new AND_node();
			anode->mother = cur;
			anode->father = guard(store);
			anode->addToParents();
			store.push_back(anode);
			node = anode;
		}
		return node;
	}

};


/**
 * This class creates a circuit P(I) from a node n 
 * such that P(I) is true iff the value of n can flow
 * to the output.  
 * 
 * To use it simply call t_build(n, parent, partn)
 * where n is the node you are interested in. 
 * parent should be NULL (it is used internally when n is called).
 * And partn is 
 * 
 * 
 */


class ExtractEvaluationCondition{

char buf[100];

map<string, t_node*> visited;



public:
vector<bool_node*> store;
map<bool_node* , t_node*> tvisited;

int ivisit;

virtual void reset(){
	for(map<string, t_node*>::iterator it = visited.begin(); it!= visited.end(); ++it){
		delete it->second;
	}
	visited.clear();
	store.clear();
	tvisited.clear();
		
}


virtual ~ExtractEvaluationCondition(){
	reset();
}


void tn_build(bool_node* bn, bool_node* parent, t_node* partn){
	++ivisit;
	if( typeid(*bn) == typeid(ARRACC_node) && parent != bn->mother  ){
		ARRACC_node* an = dynamic_cast<ARRACC_node*>(bn);	
		int found = 0;	
		for(int i=0; i<an->multi_mother.size(); ++i){
			if( an->multi_mother[i] == parent ){
				++found;
				sprintf(buf, "%dI%d", an, i);
				string tmp(buf);
				if(visited.find(tmp) != visited.end()){
					t_node* tn = visited[tmp];
					partn->children.push_back(tn);
					Assert( tn->node != NULL, "This can't be happening hgfrkj"<<tn<<", "<<tn->control);
				}else{				
					t_node* tn = new t_node(an->mother);
					tn->nidx = i;
					partn->children.push_back(tn);
					visited[tmp] = tn;					
					for(int j=0; j<an->children.size(); ++j){						
						tn_build(an->children[j], bn, tn);						
					}
					
					tn->circuit(store);					
					Assert( tn->node != NULL, "This can't be happening pm;askd");
				}
			}
		}
		
		Assert( found > 0, "This is very strange; this shouldn't happen.");
		return;
	}
	
	if( tvisited[bn] == partn ){ 
		return;	
	}
	tvisited[bn] = partn;
	
	for(int j=0; j<bn->children.size(); ++j){						
		tn_build(bn->children[j], bn, partn);			
	}
}

};


#endif

