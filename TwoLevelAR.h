//
// This class implements the Two Level Acceptance-Rejection
//

#ifndef GRID_TWOLEVELAR_H
#define GRID_TWOLEVELAR_H

#include <vector>
#include <random>
#include <cmath>

//each event in the structure is represented by an event2AR
struct event2AR {
	double rate;
	unsigned long payload;
	
	unsigned long groupId;			//in which group is it
	unsigned long position;			//position in the events vector 
	
	//generates an event with given payload and rate
	event2AR(int pl, double r) : payload(pl), rate(r), position(-1),groupId(-1) {}
};

//each groups has at least one bin, of rate "volume" B
struct bin{
	unsigned long groupId;			//which group the bin belongs to
	unsigned long prevBin;			//"pointer" to previous bin
	unsigned long nextBin;			//"pointer" to next bin
	
	//identifies whether it is the first or last bin
	bool first;
	bool last;

	//how much rate in the bin
	double value;
	
	//generates a bin with the given values
	bin(unsigned long g, unsigned long prev, unsigned long next, bool f, bool l, double val) :
	groupId(g), prevBin(prev), nextBin(next), first(f), last(l), value(val){

	}
};
//each group of the structure is represented by a group2AR
struct group2AR {
	std::vector<event2AR *> events;				//where events are stored
	unsigned long N;                            //how many events in group
	double R;                       	//sum of rate in the group
	const unsigned long groupId;				//id of the group
	const double max;							//upper boundary of rate in the group
	const double min;							//lower boundary of rate in the group
	
	//random number generator + uniform real distribution between 0 and 1
	std::mt19937 rng;
	std::uniform_real_distribution<double> randExt;
	
	//"pointer" to the last bin of this group
	unsigned long lastBinId = 0;
	
	//creates a group given all its parameters
	group2AR(double _min, double _max, unsigned long group, unsigned long lastB) : max(_max), min(_min), groupId(group),
	R(0), lastBinId(lastB) {
		N = 0;
		rng.seed(time(nullptr));
	}
	
	//deletes all events in the group before destroying it
	~group2AR() {
		for (int i = 0; i < N; i++) {
			delete events[i];
		}
	}
	
	//samples an event from this group according to its rate
	event2AR *sampleEvent();
	
	//adds a pre-created event to the group
	event2AR *addEvent(event2AR *it);

	//updates the rate of event it to newRate
	void updateEvent(event2AR *it, double newRate);

	//deletes event it
	void deleteEvent(event2AR *it);

};

class TwoLevelAR {
	std::vector<group2AR> groups;              	//where groups are stored
	std::vector<bin> bins;						//where bins are stored

	
	double R = 0.;						//total rate in the data structure
	unsigned long N = 0;						//how many events in the data structure

	const double _B;							//volume of the bins

	const unsigned long _g;						//number of groups
	const double _max;							//upper boundary of rate in the structure
	const double _min;							//lower boundary of rate in the structure
	const double _c;							//constant for the grouping rule
	const double _divBase;						//1./log(_c)
	
	//random number generator + uniform real distribution between 0 and 1
	std::uniform_real_distribution<double> randExt;
	std::mt19937 rng;

public:
	//sets min, max, grouping constant c and bin size B
	TwoLevelAR(double max, double min, double c, double B);

	//add an event with given payload and rate
	event2AR* addEvent(unsigned long payload, double rate);                        
	
	//add a precreated event
	event2AR* addEvent(event2AR * it);                    
	
	//samples an event according to its rate
	event2AR * sampleEvent();                       
	
	//updates the rate of event i
	void updateEvent(event2AR* i, double newRate);                  

	//returns group id for a given rate r
	unsigned long getGroup(double r) const;


};


#endif //GRID_TWOLEVELAR_H
