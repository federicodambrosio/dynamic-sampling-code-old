#include "TOG.h"

void groupTOG::addEvent(eventTOG *m) {
	//update total rate of the group
	R += m->rate;

	//update Ids in the eventTOG
	m->position = N;
	m->groupId = groupId;

	//add the eventTOG to data structure and update total N
	events.push_back(m);
	N++;
}

void groupTOG::deleteEvent(eventTOG *m) {
	//if it was the only eventTOG in the structure
	if (N == 1 && m->position == 0){
		N=0;
		R=0;
		events.pop_back();
		return;
	}

	unsigned long eventId = m->position; //get the Id

	//update rates
	R -= m->rate;

	//delete eventTOG and subs in its place the last in the structure
	events[eventId] = events[N - 1];
	events[eventId]->position = eventId;

	events.pop_back();
	N--;
}

void groupTOG::updateEvent(eventTOG *m, double r_) {
	R += r_ - m->rate;   //update total rate
	m->rate = r_;                     //update rate of the eventTOG
}

TOG::TOG(double pmax, double pmin, double _ratio) : _max(pmax), _min(pmin), _c(_ratio), _divBase(1. / log(_ratio)){
	std::random_device randomSeed;
	//rng.seed(randomSeed());
	rng.seed(time(nullptr));

	unsigned long index = 0;
	double min = _min;
	while (min < _max) {
		double max = _c * min;
		if (max > _max) max = _max;

		//adds group to array, groups is assigned a leaf in the tree
		groups.emplace_back(index, max, min, _c,tree.addLeaf(index, 0.));

		min = max;                                         //next group starts where this ends
		index++;
		_g++;
	}
}

void TOG::updateEvent(eventTOG * m, double newRate) {
	if (newRate == m->rate) return;             //if the rates is unchanged, do nothing

	unsigned long groupId = m->groupId;                   //what's its current group

	if (groupId == -1) {                        //previously impossible eventTOG, it had no group
		if (newRate==0) return;                 //still impossible, do nothing
		m->rate = newRate;
		addEvent(m);                          //added to the structure with its new rate
		
		update();								//updates the tree
		
		return;
	}

	if (newRate == 0) {                         //previously possible event becomes impossible
		groups[groupId].deleteEvent(m);      //removed from group
		N--;
		m->groupId = - 1;                       //no group
		m->position = -1;
		m->rate = 0;
		tree.updateLeaf(groups[groupId].leaf, groups[groupId].R);
		
		update();
		
		return;
	}

	unsigned long newGroupId = getGroup(newRate);
	
	if (groupId == newGroupId) {       //rate changed, but it's still in the same group
		groups[groupId].updateEvent(m,newRate);  //updates the event inside the group and that's it
		tree.updateLeaf(groups[groupId].leaf, groups[groupId].R);
		update();
		return;										//updates the tree
	} else {      //rate changed, different group now
		groups[groupId].deleteEvent(m);          //delete from previous group
		tree.updateLeaf(groups[groupId].leaf, groups[groupId].R);
		m->rate = newRate;
		groups[newGroupId].addEvent(m);          //add to new group
		tree.updateLeaf(groups[newGroupId].leaf, groups[newGroupId].R);
		
		update();									//updates the tree

		return;
	}
}

eventTOG *TOG::sampleEvent() {
	while(true){
		CBTNode * extractedNode = tree.sampleLeaf();      //samples group from CBT
		unsigned long groupId = extractedNode->payload;
		while (true){                                       //Acceptance Rejection on that group
			double rand = randExt(rng) * (double)groups[groupId].N;
			unsigned long eventId =(int)std::floor(rand);
			double randEvent = (rand - (double)eventId) * groups[groupId].maxR;
			if (groups[groupId].events[eventId]->rate >= randEvent ) {
				return groups[groupId].events[eventId];
			}
		}
	}
}

void TOG::update() {            //updates the tree connected to the structure
	tree.updateTree();
}


void TOG::addEvent(eventTOG * m) {
	if (m->rate == 0) {
		m->groupId = -1;
		return;
	}
	unsigned long groupId = getGroup(m->rate);
	groups[groupId].addEvent(m);
	tree.updateLeaf(groups[groupId].leaf, groups[groupId].R);
	
	update(); //updates the tree
	N++;
}


eventTOG* TOG::addEvent(unsigned long payload, double rate) {
	auto * m= new eventTOG(payload, rate);
	addEvent(m);
	return m;
}

unsigned long TOG::getGroup(double r) const {
	if (r == _min) return 0;
	if (r == _max) return _g - 1;
	return (unsigned long)std::floor(std::log(r/_min)*_divBase);
}

