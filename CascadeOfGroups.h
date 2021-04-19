//
// Created by federicodambrosio on 17-3-17.
//

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

	elementCoG *sampleElement(int & counter, int & randNumbers);  //as above but # ops in counter and # iterations in
																//randNumbers
	elementCoG *sampleElement(RunningStats & rs);                 //as above but # of iterations in rs

	elementCoG *addElement(elementCoG *it);

	elementCoG *addElement(elementCoG *it, int & counter);         //as above but # ops in counter

	void updateElement(elementCoG *it, double newRate);

	void updateElement(elementCoG *it, double newRate, int & counter); //as above but # ops in counter

	void deleteElement(elementCoG *it);

	void deleteElement(elementCoG *it, int & counter);             //as above but # ops in counter

};
//cascade from 1 to g
class CascadeOfGroups {                                     //Section 3.4
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
	CascadeOfGroups(bool rev, double minRate, double maxRate, double ratio = 2.);

	~CascadeOfGroups() {
		for (auto & _group : _groups) {
			delete _group;
		}
	}

	elementCoG* addElement(int payload, double rate);

	elementCoG* addElement(int payload, double rate, int & counter);

	elementCoG *sampleElement(double random);

	elementCoG *sampleElement(double random, int & counter, int & randNumbers);

	elementCoG *sampleElement();

	elementCoG *sampleElement(int & counter, int & randNumbers);

	elementCoG *sampleElement(double random, RunningStats & deep, RunningStats & iter);

	elementCoG *updateElement(elementCoG *it, double newRate);

	elementCoG *updateElement(elementCoG *it, double newRate, int & counter);

	unsigned long getGroup(double r) const;


};



#endif //STATIC_UPDATE_NESTEDGROUPS_H
