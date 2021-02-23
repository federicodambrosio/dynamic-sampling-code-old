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
	int N;                                              //how many elements in group
	double totalRate;                                   //sum of rate in the group
	const int groupId;
	const double max;
	const double min;
	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;

	singleGroup(double _min, double _max, int group, int _n = 0) : max(_max), min(_min), groupId(group), N(_n),
	                                                                    totalRate(0) {
		if (_n > 0) elements.reserve(_n);
		rng.seed(time(nullptr));
		randExt = new std::uniform_real_distribution<double>(0,1);
	}

	~singleGroup() {
		for (int i = 0; i < N; i++) {
			delete elements[i];
		}
		delete randExt;
	}

	elementCoG *extractElement();

	elementCoG *extractElement(int & counter, int & randNumbers);  //as above but # ops in counter and # iterations in
																//randNumbers
	elementCoG *extractElement(RunningStats & rs);                 //as above but # of iterations in rs

	elementCoG *addElement(elementCoG *it);

	elementCoG *addElement(elementCoG *it, int & counter);         //as above but # ops in counter

	void updateElement(elementCoG *it, double newRate);

	void updateElement(elementCoG *it, double newRate, int & counter); //as above but # ops in counter

	void deleteElement(elementCoG *it);

	void deleteElement(elementCoG *it, int & counter);             //as above but # ops in counter

};

class CascadeOfGroups {                                     //Section 3.4
	int N;                                                  //how many elements in the whole structure
	double _totalRate;                                      //sum of all rates
	double _minRate;
	double _maxRate;
	double _c;                                              //grouping constant
	std::vector<singleGroup*> _groups;
	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;
	double _divBase;

public:
	CascadeOfGroups(double minRate, double maxRate, int n_ = 0, double ratio = 2.);

	~CascadeOfGroups() {
		for (auto & _group : _groups) {
			delete _group;
		}
	}

	elementCoG* addElement(int payload, double rate);

	elementCoG* addElement(int payload, double rate, int & counter);

	elementCoG *extractElement(double random);

	elementCoG *extractElement(double random, int & counter, int & randNumbers);

	elementCoG *extractElement();

	elementCoG *extractElement(int & counter, int & randNumbers);

	elementCoG *extractElement(double random, RunningStats & deep, RunningStats & iter);

	elementCoG *updateElement(elementCoG *it, double newRate);

	elementCoG *updateElement(elementCoG *it, double newRate, int & counter);


};


#endif //STATIC_UPDATE_NESTEDGROUPS_H
