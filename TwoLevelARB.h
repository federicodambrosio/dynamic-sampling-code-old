//
// This class implements the Two Level Acceptance-Rejection with Multiple Bins
//

#ifndef GRID_TWOLEVELARB_H
#define GRID_TWOLEVELARB_H

#include <vector>
#include <random>
#include <cmath>

struct element2ARB {
	unsigned long groupId;
	unsigned long moveId;
	double rate;
	unsigned long payload;

	element2ARB(int pl, double r) : payload(pl), rate(r), moveId(-1),groupId(-1) {}
};

struct bin{
	unsigned long groupId;
	unsigned long prevBin;
	unsigned long nextBin;

	bool first;
	bool last;

	double value;

	bin(unsigned long g, unsigned long prev, unsigned long next, bool f, bool l, double val) :
	groupId(g), prevBin(prev), nextBin(next), first(f), last(l), value(val){

	}
};

struct group2ARB {
	std::vector<element2ARB *> elements;
	unsigned long N;                                              //how many elements in group
	double totalRate;                                   //sum of rate in the group
	const unsigned long groupId;
	const double max;
	const double min;

	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;

	unsigned long lastBinId = 0;

	group2ARB(double _min, double _max, unsigned long group, unsigned long lastB) : max(_max), min(_min), groupId(group),
	totalRate(0), lastBinId(lastB) {
		N = 0;
		rng.seed(time(nullptr));
		randExt = new std::uniform_real_distribution<double>(0,1);
	}

	~group2ARB() {
		for (int i = 0; i < N; i++) {
			delete elements[i];
		}
		delete randExt;
	}

	element2ARB *sampleElement();

	element2ARB *addElement(element2ARB *it);

	void updateElement(element2ARB *it, double newRate);

	void deleteElement(element2ARB *it);

};

class TwoLevelARB {
	std::mt19937 rng;
	std::vector<group2ARB*> groups;                          //vector of events
	std::vector<bin> bins;

	std::uniform_real_distribution<double> * randExt;
	double totalRate = 0.;
	unsigned long N = 0;
	unsigned long updateCounter = 0;                        //when it gets > g, recomputes max

	const double _B;

	const unsigned long _g;
	const double _max;
	const double _min;
	const double _c;
	const double _divBase;

public:
	TwoLevelARB(double min, double max, double c, double B);

	element2ARB* addElement(double rate);                        //payload = order of creation
	element2ARB* addElement(element2ARB * it);                     //adds a pre-created event

	element2ARB * sampleElement();                                  //returns an event according to rate

	void updateElement(element2ARB* i, double newRate);                  //updates the rate of an event

	unsigned long getGroup(double r) const;


};


#endif //GRID_TWOLEVELARB_H
