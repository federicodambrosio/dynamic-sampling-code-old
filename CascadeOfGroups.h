//
// The class CascadeOfGroups implements both the Cascade of Groups and Reverse Cascade of
// Groups methods from the paper, depending on the _reversed parameter.
// 
//

#ifndef CASCADE_OF_GROUPS_H
#define CASCADE_OF_GROUPS_H

#include <cmath>
#include <iostream>
#include <random>

//each event in the structure is represented by an eventCoG
struct eventCoG {
	unsigned long groupId;			//in which group is the event
	unsigned long position;			//position in the group
	double rate;
	unsigned long payload;
	
	//generates an event with given payload and rate. Position and groupId are set by
	//the structure once the event is added
	eventCoG(unsigned long pl, double r) : payload(pl), rate(r), position(-1),groupId(-1) {}
};

//this structure represents a group of the (Reversed) Cascade of Groups
struct singleGroup {
	//events of the groups are stored in this vector
	std::vector<eventCoG *> events;	
	
	unsigned long N;						//how many events in group                                              
	double R;      					//sum of the rate in the group                             
	const unsigned long groupId;			//id of the group
	const double max;						//maximum rate for this group
	const double min;						//minimum rate for this group
	
	//random number generator + uniform real distribution between 0 and 1
	std::mt19937 rng;
	std::uniform_real_distribution<double> randExt;

	//creates empty group with given min and max given by the grouping rule
	singleGroup(double _min, double _max, unsigned long group) : max(_max), min(_min), groupId(group), R(0) {
		N = 0;
		rng.seed(time(nullptr));
	}
	
	//destroys the group and all the events inside it
	~singleGroup() {
		for (unsigned long i = 0; i < events.size(); i++) {
			delete events[i];
		}
	}
	
	//samples an event from the group according to its rate
	eventCoG *sampleEvent();

	//adds a (pre-created) event to the group
	eventCoG *addEvent(eventCoG *it);

	//update the rate of event *it with rate newRate
	void updateEvent(eventCoG *it, double newRate);

	//deletes the event *it
	void deleteEvent(eventCoG *it);

};

//this class represents the (Reversed) Cascade of Groups
//_reversed == false : Cascade of Groups
//_reversed == true : Reversed Cascade of Groups

class CascadeOfGroups {                                     
	unsigned long N;                  	//how many events in the whole structure
	double R;                    	//sum of all rates
	const double _minRate;					//minimum rate for this structure
	const double _maxRate;					//maximum rate for this structure
	const double _c;                       	//constant for the grouping rule
	const double _divBase;                  // =  1./log(_c)
	
	//determines if it's a Cascade (false) or Reversed Cascade of Groups (true)
	const bool _reversed;                    
	
	const unsigned long _g;                 //number of groups
	
	//all groups are stored here
	std::vector<singleGroup*> _groups;
	
	//random number generator + uniform real distribution between 0 and 1
	std::mt19937 rng;
	std::uniform_real_distribution<double> randExt;
	
	//gets group # for event of rate r according to grouping rule
	unsigned long getGroup(double r) const;

public:
	//if reverse = true, Reversed Cascade of Group 
	CascadeOfGroups(bool reverse, double minRate, double maxRate, double ratio = 2.);
	
	//destroys the structure by destroying each group 
	~CascadeOfGroups() {
		for (auto & _group : _groups) {
			delete _group;
		}
	}
	
	//adds an event with given payload and rate
	eventCoG* addEvent(unsigned long payload, double rate);
	
	//samples an event according to its rate
	eventCoG *sampleEvent();

	//updates the rate of event it to newRate
	eventCoG *updateEvent(eventCoG *it, double newRate);
	
};



#endif //CASCADE_OF_GROUPS_H
