//
// This implements the Tree of Groups method from the paper.
//

#ifndef TREE_OF_GROUPS_H
#define TREE_OF_GROUPS_H

#include "CompleteBinaryTree.h"
#include <random>
#include <vector>
#include <ctime>

struct eventTOG { //a single event in the Tree of Groups datastructure
	unsigned long payload;
	double rate;

	unsigned long position;      //id of the event inside a group
	unsigned long groupId;        //id of the group

	//creates an "empty" eventTOG with out-of-range indexes (that we can look for)
	eventTOG() : position(-1), groupId(-1), payload(0), rate(0.){}

	//creates a eventTOG, given all the parameters
	explicit eventTOG(unsigned long _payload, double _rate = 0, unsigned long _pos=0, int _groupId=-1)
	: payload(_payload), rate(_rate), position(_pos), groupId(_groupId){}
};

struct groupTOG {
	const unsigned long groupId;  //the id of the group
	const double maxR;  //max rate of a eventTOG in the group
	const double minR;  //min rate of a eventTOG in the group
	double R = 0;  //sum of the rates of all the events inside the group
	std::vector<eventTOG*> events;    //array of events
	unsigned long N = 0;     //number of events inside the group
	CBTNode *leaf;   //leaf in the Complete Binary Tree for this group

	groupTOG(unsigned long i, double max, double min, double _k, CBTNode* _leaf) :
		groupId(i), maxR(max), minR(min), leaf(_leaf){}
	
	//destroys the group by destroying its events
	~groupTOG() {
		for (auto & event : events) delete event;
	}
	//adds a pre-constructed event
	void addEvent(eventTOG* m);
	//deletes an event
	void deleteEvent(eventTOG* m);
	//updates the rate of the event
	void updateEvent(eventTOG *m, double r_);
};

class TOG {                                 //Tree of Groups, Section 3.4
	std::vector<groupTOG> groups;
	const double _max;                      //maximum possible rate in the whole structure
	const double _min;                      //minimum possible rate " " "
	unsigned long _g=0;                          //how many groups
	const double _c;                         //grouping constant, i.e. group i is form rmax/_c^(i-1) to rmax/_c^i
	const double _divBase;                         // 1/log2(_c)
	unsigned long N=0;                           //how many events

	//random number generator + uniform real distribution between 0 and 1
	std::mt19937 rng;
	std::uniform_real_distribution<double> randExt;
	
	//CBT to select a group and private method to update it
	CompleteBinaryTree tree;                
	void update();
public:

	//sets max, min and grouping constant 
	TOG(double pmax, double pmin, double _k =2 );
	//samples an event, returns a pointer
	eventTOG * sampleEvent();
	//updates the rate of an event, updates the tree
	void updateEvent(eventTOG * m, double newRate);	
	//add an already existing event, updates the tree
	void addEvent(eventTOG * m);	
	//same but generates the event and returns * to it
	eventTOG * addEvent(unsigned long payload, double rate);             
	//how many groups
	unsigned long NGroup() const{return _g;}
	//gets total rate
	double totalRate(){	return tree.getRate();}
	//returns group id for a given rate r
	unsigned long getGroup(double r) const;

};


#endif //TREE_OF_GROUPS_H
