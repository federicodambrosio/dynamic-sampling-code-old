//
// Created by federicodambrosio on 17-3-17.
//

#ifndef STATIC_UPDATE_NESTEDGROUPS_H
#define STATIC_UPDATE_NESTEDGROUPS_H

#include <cmath>
#include <iostream>
#include <random>
#include "RunningStats.hpp"

struct item {
	int groupId;
	int moveId;
	double rate;

	int payload;

	item(int pl, double r) : payload(pl), rate(r) {}
};

struct singleGroup {
	std::vector<item *> items;
	int N;
	int groupId;
	double totalRate;
	double max;
	double min;
	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;


	singleGroup(double _min, double _max, int group, int _N = 0) : max(_max), min(_min), groupId(group), N(0),
	                                                                    totalRate(0) {
		if (_N > 0) items.reserve(_N);
		rng.seed(time(0));
		randExt = new std::uniform_real_distribution<double>(0,1);

	}

	~singleGroup() {
		for (int i = 0; i < N; i++) {
			item * itt = items[i];
			delete itt;
		}
		delete randExt;
	}

	item *extractItem();

	item *extractItem(int & counter, int & randNumbers);

	item *extractItem(RunningStats & rs);

	item *addItem(item *it);

	item *addItem(item *it, int & counter);

	void updateItem(item *it, double newRate);

	void updateItem(item *it, double newRate, int & counter);

	void deleteItem(item *it);

	void deleteItem(item *it, int & counter);


};

class CascadeOfGroups {
	int _N;
	double _totalRate;
	double _minRate;
	double _maxRate;
	double _ratio;
	std::vector<singleGroup*> _groups;
	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;


	double _divBase;

public:

	CascadeOfGroups(double minRate, double maxRate, int N = 0, double ratio = 2.);

	~CascadeOfGroups() {
		for (int i = 0; i < _groups.size(); i++) {
			delete _groups[i];
		}
	}

	item* addItem(int payload, double rate);

	item* addItem(int payload, double rate, int & counter);

	item *extractItem(double random);

	item *extractItem(double random, int & counter, int & randNumbers);

	item *extractItem();

	item *extractItem(int & counter, int & randNumbers);


	item *extractItem(double random, RunningStats & deep, RunningStats & iter);

	item *updateItem(item *it, double newRate);

	item *updateItem(item *it, double newRate, int & counter);


};


#endif //STATIC_UPDATE_NESTEDGROUPS_H
