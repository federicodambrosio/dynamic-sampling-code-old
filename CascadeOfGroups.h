//
// The class CascadeOfGroups implements both Cascade of Groups and Reverse Cascade of 
// Groups (see parameter reverse) data structures from the paper.  



#ifndef STATIC_UPDATE_NESTEDGROUPS_H
#define STATIC_UPDATE_NESTEDGROUPS_H

#include <cmath>
#include <iostream>
#include <random>
#include "RunningStats.hpp"

struct elementCoG {
	int groupId;
	int moveId;
	double rate;
	int payload;

	elementCoG(int pl, double r) : payload(pl), rate(r), moveId(-1),groupId(-1) {}
};

struct singleGroup {
	std::vector<elementCoG *> elements;
	unsigned long N;                                              //how many elements in group
	double totalRate;                                   //sum of rate in the group
	const unsigned long groupId;
	const double max;
	const double min;
	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;

	singleGroup(double _min, double _max, unsigned long group) : max(_max), min(_min), groupId(group), totalRate(0) {
		N = 0;
		rng.seed(time(nullptr));
		randExt = new std::uniform_real_distribution<double>(0,1);
	}

	~singleGroup() {
		for (int i = 0; i < N; i++) {
			delete elements[i];
		}
		delete randExt;
	}

	elementCoG *sampleElement();

	elementCoG *addElement(elementCoG *it);

	void updateElement(elementCoG *it, double newRate);

	void deleteElement(elementCoG *it);

};
//cascade from 1 to g
class CascadeOfGroups {                                     
	int N;                                                  //how many elements in the whole structure
	double _totalRate;                                      //sum of all rates
	const double _minRate;
	const double _maxRate;
	const double _c;                                        //grouping constant
	const bool _reversed;                                   //if true, groups from r_max to r_min
															//else, from r_min to r_max
	const unsigned long _g;                                 //number of groups
	std::vector<singleGroup*> _groups;
	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;
	const double _divBase;                                  // 1/log(_c)

public:
	//if reverse = true, Reversed Cascade of Group 
	CascadeOfGroups(bool reverse, double minRate, double maxRate, double ratio = 2.);

	~CascadeOfGroups() {
		for (auto & _group : _groups) {
			delete _group;
		}
	}

	elementCoG* addElement(int payload, double rate);

	elementCoG *sampleElement();

	elementCoG *updateElement(elementCoG *it, double newRate);

	unsigned long getGroup(double r) const;	//gets group # for event of rate r
	
	std::vector<double> rates();			//total rates of all groups
	
};



#endif //STATIC_UPDATE_NESTEDGROUPS_H
