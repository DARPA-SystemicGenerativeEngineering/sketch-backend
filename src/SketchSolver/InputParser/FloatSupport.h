#pragma once

#include "BasicError.h"
#include <vector>
#include <map>

using namespace std;

typedef float(*floatfun)(float);

class FloatManager;

template<typename Op>
class FloatOp {
	FloatManager& fm;
public:
	FloatOp(FloatManager& _fm) :fm(_fm) {}
	int operator()(int x, int y);
};

class FloatFun {
protected:
	FloatManager& fm;
	floatfun f;
public:
	FloatFun(FloatManager& _fm, floatfun _f) :fm(_fm), f(_f) {}
	inline int operator()(int x);
};


class FloatManager {
	map<float, int> floatIdx;
	vector<float> floats;
	map<string, floatfun> floatfuns;

public:
	const float epsilon;
	FloatManager(float _epsilon) :epsilon(_epsilon) {
		floatIdx[0.0] = 0;
		floats.push_back(0.0);
		floatfuns["arctan_math"] = atan;
		floatfuns["sin_math"] = sin;
		floatfuns["cos_math"] = cos;
		floatfuns["tan_math"] = tan;
		floatfuns["sqrt_math"] = sqrt;		
	}

	bool hasFun(const string& name) {
		return floatfuns.count(name) > 0;
	}
	FloatFun getFun(const string& name) {		
		return FloatFun(*this, floatfuns[name]);
	}

	float getFloat(int id) {
		if (id < 0) {
			return -floats[-id];
		}
		else {
			return	floats[id];
		}

	}

	int getIdx(float x) {
		//floatIdx only stores positive values. Negative values will yield negative indices. 
		//This means that negating the index will automatically negate the value that the index corresponds to.
		//That's why zero must always be stored in index zero.
		bool isNeg = false;
		if (x < -0.0) {
			x = -x;
			isNeg = true;
		}
		auto lbd = floatIdx.lower_bound(x - epsilon + (epsilon / 100));

		if (lbd != floatIdx.end()) {
			float dist = lbd->first - x;
			if (-epsilon < dist && dist < epsilon) {
				if (isNeg) {
					return -lbd->second;
				}
				else {
					return  lbd->second;
				}
			}
			lbd++;
			if (lbd != floatIdx.end()) {
				dist = lbd->first - x;
				if (-epsilon < dist && dist < epsilon) {
					if (isNeg) {
						return -lbd->second;
					}
					else {
						return  lbd->second;
					}
				}
			}
		}


		int pos = floatIdx.size();
		floatIdx[x] = pos;
		floats.push_back(x);
		Assert(floatIdx.size() == floats.size(), "What???");
		if (isNeg) {
			return -pos;
		}
		else {
			return pos;
		}
	}
};



template<typename Op>
inline
int FloatOp<Op>::operator()(int x, int y) {
	Op op;
	float xf = fm.getFloat(x);
	float yf = fm.getFloat(y);
	return fm.getIdx(op(xf, yf));
}

inline int FloatFun::operator()(int x) {
	float xf = fm.getFloat(x);
	return fm.getIdx(f(xf));
}

