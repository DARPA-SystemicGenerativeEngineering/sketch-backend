#include "BasicNumericalHelper.h"


BasicNumericalHelper::BasicNumericalHelper(FloatManager& _fm, BooleanDAG* _dag, map<int, int>& _imap): NumericalSolverHelper(_fm, _dag, _imap) {
	dag->lprint(cout);
	// Collect the list of boolean nodes which can be ignored.
	for (int i = 0; i < dag->size(); i++) {
		// ignore boolean nodes that don't have either float children or float parents
		bool_node* n = (*dag)[i];
		if (n->getOtype() == OutType::BOOL && (n->hasFloatParent() || n->hasFloatChild())) {
			boolNodes.insert(i);
		} 
	}
	
	// generate ctrls mapping
	set<int> ctrlNodeIds;
	vector<bool_node*>& ctrls = dag->getNodesByType(bool_node::CTRL);
	int ctr = 0;
	for (int i = 0; i < ctrls.size(); i++) {
		if (ctrls[i]->getOtype() == OutType::FLOAT) {
			ctrlMap[ctrls[i]->get_name()] = ctr++;
			ctrlNodeIds.insert(ctrls[i]->id);
		}
	}
	ncontrols = ctr;
	if (ncontrols == 0) {
		ncontrols = 1;
	}
	
	cout << "NControls: " << ncontrols << endl;
	
	state = gsl_vector_alloc(ncontrols);
	eval = new AutoDiff(*dag, fm, ctrlMap);
	seval = new SimpleEvaluator(*dag, fm, ctrlMap, boolCtrlMap);
	opt = new SnoptWrapper(eval, dag, imap, ctrlMap, boolNodes, ncontrols);
	//cg = new ConflictGenerator(eval, imap, dag, ignoredBoolNodes, ctrlNodeIds);
	cg = new SimpleConflictGenerator(imap, boolNodes);
	opt->randomizeCtrls(state);

	
	GradUtil::tmp = gsl_vector_alloc(ncontrols);
	GradUtil::tmp1 = gsl_vector_alloc(ncontrols);
	GradUtil::tmp2 = gsl_vector_alloc(ncontrols);
	GradUtil::tmp3 = gsl_vector_alloc(ncontrols);
	GradUtil::tmpT = gsl_vector_alloc(ncontrols);
}

BasicNumericalHelper::~BasicNumericalHelper(void) {
	delete GradUtil::tmp;
	delete GradUtil::tmp1;
	delete GradUtil::tmp2;
	delete GradUtil::tmp3;
	delete GradUtil::tmpT;
}

void BasicNumericalHelper::setInputs(vector<vector<int>>& allInputs_, vector<int>& instanceIds_) {
	allInputs = allInputs_;
	instanceIds = instanceIds_;
}

bool BasicNumericalHelper::checkInputs(int rowid, int colid) {
	int nid = imap[colid];
	if (boolNodes.find(nid) == boolNodes.end()) {
		return false;
	}
	cout << (*dag)[nid]->lprint() << endl;
	cout << allInputs[rowid][colid] << endl;
	return true;
}

bool BasicNumericalHelper::checkSAT() {
	bool sat = opt->optimize(allInputs, state);
	if (sat) {
		state = opt->getMinState();
	}
	return sat;
}

bool BasicNumericalHelper::ignoreConflict() {
	return false;
}

vector<tuple<int, int, int>> BasicNumericalHelper::collectSuggestions() {
	vector<tuple<int, int, int>> suggestions;
	for (int i = 0; i < allInputs.size(); i++) {
		vector<tuple<float, int, int>> s = seval->run(state, imap);
		sort(s.begin(), s.end());
		reverse(s.begin(), s.end());
		for (int k = 0; k < s.size(); k++) {
			int idx = get<1>(s[k]);
			if (allInputs[i][idx] == EMPTY) {
				suggestions.push_back(make_tuple(i, idx, get<2>(s[k])));
			}
		}
	}
	return suggestions;
}

vector<pair<int, int>> BasicNumericalHelper::getConflicts(int rowid, int colid) {
	return cg->getConflicts(state, allInputs, instanceIds, rowid, colid);
}

void BasicNumericalHelper::getControls(map<string, float>& ctrls) {
	for (auto it = ctrlMap.begin(); it != ctrlMap.end(); it++) {
		ctrls[it->first] = gsl_vector_get(state, it->second);
	}
}
