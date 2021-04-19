//
// Created by Federico D'Ambrosio on 31/03/2021.
//

#ifndef GRID_TWOLEVELAR_H
#define GRID_TWOLEVELAR_H
#include <vector>
#include <random>
#include <ctime>

struct element2AR {
	unsigned long groupId;
	unsigned long moveId;
	double rate;
	int payload;

	element2AR(int pl, double r) : payload(pl), rate(r), moveId(-1),groupId(-1) {}
};

struct group2AR {
	std::vector<element2AR *> elements;
	unsigned long N;                                              //how many elements in group
	double totalRate;                                   //sum of rate in the group
	const unsigned long groupId;
	const double max;
	const double min;
	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;

	group2AR(double _min, double _max, unsigned long group) : max(_max), min(_min), groupId(group), totalRate(0) {
		N = 0;
		rng.seed(time(nullptr));
		randExt = new std::uniform_real_distribution<double>(0,1);
	}

	~group2AR() {
		for (int i = 0; i < N; i++) {
			delete elements[i];
		}
		delete randExt;
	}

	element2AR *sampleElement();

	element2AR *addElement(element2AR *it);

	void updateElement(element2AR *it, double newRate);

	void deleteElement(element2AR *it);

};

class TwoLevelAR {
	std::mt19937 rng;
	std::vector<group2AR*> groups;                          //vector of events
	std::uniform_real_distribution<double> * randExt;
	double totalRate = 0.;
	unsigned long N = 0;
	double currMax = 0;
	unsigned long updateCounter = 0;                        //when it gets > g, recomputes max

	const unsigned long _g;
	const double _max;
	const double _min;
	const double _c;
	const double _divBase;

public:
	TwoLevelAR(double min, double max, double c);

	element2AR* addElement(double rate);                        //payload = order of creation
	element2AR* addElement(element2AR * it);                     //adds a pre-created event

	element2AR * sampleElement();                                  //returns an event according to rate

	void updateElement(element2AR* i, double newRate);                  //updates the rate of an event

	unsigned long getGroup(double r) const;

	double updateMax();                                         //after g calls, triggers a max update

};


#endif //GRID_TWOLEVELAR_H
