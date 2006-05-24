

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <string>
#include <set>
#include <vector>
#include <dirent.h>
#include <queue>
#include "MiniSATSolver.h"


 void MiniSATSolver::annotate(const string& msg){
	Dout( cout<<msg );
	FileOutput(output<<msg<<endl);
}

 void MiniSATSolver::annotateInput(const string& name, int i, int sz){
	Dout( cout<<"x "<<name<<" ");
	FileOutput(output<<"x "<<name<<" ");
	for(int t=0; t<sz; ++t){
		Dout( cout<<(i+t)<<" ");
		FileOutput(output<<(i+t)<<" ");
	}
	Dout(cout<<endl);
	FileOutput(output<<endl);
}


void MiniSATSolver::addClause(int tmp[], int sz, vec<Lit>& lits){
	lits.clear();
	for(int i=0; i<sz; ++i){	
		int var = abs(tmp[i]);
		lits.push( (tmp[i] > 0) ? Lit(var) : ~Lit(var) );		
	}	
	s->addClause(lits);
} 


//This function encodes x == a ? b:c;
 void MiniSATSolver::addChoiceClause(int x, int a, int b, int c, int gid){
	Dout( cout<<" "<<x<<"= "<<a<<" ? "<<b<<":"<<c<<";"<<endl );
	FileOutput( output<<x<<" CHOICE "<<a<<" "<<b<<" "<<c<<endl );
	vec<Lit> lits;
	{ int tmp[] = { -(x), -(a), (b) }; addClause(tmp, 3, lits);}
	{ int tmp[] = { -(x), (c), (a) }; addClause(tmp, 3, lits);}
	{ int tmp[] = { -(x), (c), (b) }; addClause(tmp, 3, lits);}
	{ int tmp[] = { (x), -(c), -(b) }; addClause(tmp, 3, lits);}
	{ int tmp[] = { (x), (a), -(c) }; addClause(tmp, 3, lits);}
	{ int tmp[] = { (x), -(a), -(b) }; addClause(tmp, 3, lits);}
}


//This function encodes x == a xor b;
 void MiniSATSolver::addXorClause(int x, int a, int b, int gid){
	Dout( cout<<" "<<x<<"= "<<a<<" xor "<<b<<"; "<<endl );
	FileOutput( output<<x<<" XOR "<<a<<" "<<b<<endl );
	vec<Lit> lits;	
	{ int tmp[] = { -(x), -(a), -(b) }; addClause(tmp, 3, lits);}
	{ int tmp[] = { -(x), (a), (b) }; addClause(tmp, 3, lits);}
	{ int tmp[] = { (x), -(a), (b) }; addClause(tmp, 3, lits);}
	{ int tmp[] = { (x), (a), -(b) }; addClause(tmp, 3, lits);}
}

//This function encodes x == a or b;
 void MiniSATSolver::addOrClause(int x, int a, int b, int gid){
	Dout( cout<<" "<<x<<"= "<<a<<" or "<<b<<"; "<<endl );
	FileOutput( output<<x<<" OR "<<a<<" "<<b<<endl );
	vec<Lit> lits;	
	{ int tmp[] = { (x), -(a)}; addClause(tmp, 2, lits);}
	{ int tmp[] = { (x), -(b)}; addClause(tmp, 2, lits);}
	{ int tmp[] = { -(x), (a), (b)}; addClause(tmp, 3, lits);}
}


//This function encodes a[0] == a[1] or a[2] or ... a[size];
 void MiniSATSolver::addBigOrClause(int* a, int size, int gid){
	Dout( cout<<" "<<a[0]<<"= " );
	FileOutput( output<<a[0]<<" BOR "<<size<<" " );
	vec<Lit> lits;
	for(int i=0; i<size; ++i){
		Dout(cout<<a[i+1]<<" or ");
		{ int tmp[] = { (a[0]), -(a[i+1])}; addClause(tmp, 2, lits);}
		FileOutput( output<<a[i+1]<<" " );
	}
	FileOutput( output<<endl );
	Dout(cout<<"; "<<endl);
	a[0] = -a[0];
	addClause(a, size+1, lits);
}


//This function encodes x == a and b;
 void MiniSATSolver::addAndClause(int x, int a, int b, int gid){
	Dout( cout<<" "<<x<<"= "<<a<<" and "<<b<<"; "<<endl );
	FileOutput( output<<x<<" AND "<<a<<" "<<b<<endl );
	vec<Lit> lits;	
	{ int tmp[] = { -(x), (a)}; addClause(tmp, 2, lits);}
	{ int tmp[] = { -(x), (b)}; addClause(tmp, 2, lits);}
	{ int tmp[] = { (x), -(a), -(b)}; addClause(tmp, 3, lits);}
}

//This function encodes x = a;
 void MiniSATSolver::addEqualsClause(int x, int a, int gid){
	Dout( cout<<" "<<x<<"= "<<a<<"; "<<flush<<endl );
	FileOutput( output<<x<<" EQ "<<a<<endl );
	vec<Lit> lits;	
	{ int tmp[] = { -(x), (a)}; addClause(tmp, 2, lits);}
	{ int tmp[] = { (x), -(a)}; addClause(tmp, 2, lits);}
}


//This function encodes x == a;
 void MiniSATSolver::addEquateClause(int x, int a, int gid){
	Dout( cout<<" "<<x<<"= "<<a<<"; "<<flush<<endl );
	FileOutput( output<<"x OUTXOR "<<x<<" "<<-a<<endl );
	vec<Lit> lits;	
	{ int tmp[] = { -(x), (a)}; addClause(tmp, 2, lits);}
	{ int tmp[] = { (x), -(a)}; addClause(tmp, 2, lits);}
}


 void MiniSATSolver::setVarClause(int x, int gid){
	Dout( cout<<" set "<<x<<";"<<endl );
	FileOutput( output<<"x SET "<<x<<" ;"<<endl );
	int var = abs(x);
	s->addUnit( (x > 0) ? Lit(var) : ~Lit(var) );
}


 void MiniSATSolver::assertVarClause(int x, int gid){
	Dout( cout<<" assert "<<x<<";"<<endl );
	FileOutput( output<<"x OUTASSERT "<<x<<" ;"<<endl );
	int var = abs(x);
	s->addUnit( (x > 0) ? Lit(var) : ~Lit(var) );
}

 void MiniSATSolver::printDiagnostics(char c){
 	SolverStats& stats = s->stats;	
	cout << c << "restarts              : "<<stats.starts<<endl;
	cout << c << "conflicts             : "<<stats.conflicts<<endl;
	cout << c << "decisions             : "<<stats.decisions<<endl; 
	cout << c << "propagations          : "<<stats.propagations<<endl;
	cout << c << "inspects              : "<<stats.inspects<<endl;
	cout << c << "conflict literals     : "<<stats.tot_literals<<"    "<<
	((stats.max_literals - stats.tot_literals)*100 / (double)stats.max_literals)<<" % "<<endl;
 }






int MiniSATSolver::getVarVal(int id){
	return s->model[id].toInt();
}
 
int MiniSATSolver::newVar(){
 	s->newVar();
	return s->nVars()-1;
}
	 
int MiniSATSolver::newInVar(){
 	s->newVar();
	return s->nVars()-1;
}
	 	 
void MiniSATSolver::disableVarBranch(int i){

}
 
bool MiniSATSolver::ignoreOld(){
	return false;	
}
	 
 void MiniSATSolver::deleteClauseGroup(int i){
	
}

 int MiniSATSolver::solve(){
 	s->simplifyDB();
 	if( ! s->okay() ){ return UNSATISFIABLE; }
	bool result = s->solve();
	if( result) {
		return SATISFIABLE;	
	}else{
		return UNSATISFIABLE;
	}
}

 void MiniSATSolver::reset(){
	Assert ( s->okay() , "This can't happen");
}

 void MiniSATSolver::cleanupDatabase(){
	
}

 void MiniSATSolver::clean(){
 	delete s;
 	s = new Solver();
}


	