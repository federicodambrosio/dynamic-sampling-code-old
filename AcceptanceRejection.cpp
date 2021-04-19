//
// Created by federicodambrosio on 9-3-17.
//

#include "AcceptanceRejection.h"

elementAR* AcceptanceRejection::addElement(double rate) {
	if (rate<=0) return nullptr;
	//an event with payload = size of the data structure is created
	auto * aa = new elementAR(items.size(), items.size(),rate);
	items.push_back(aa);
	if (rate > maxR) maxR = rate;
	currentRate += rate;
	return aa;
}

elementAR * AcceptanceRejection::sampleElement() {
	while (true) {  //repeats until an event is returned
		double rand = randExt->operator()(rng) * (double)items.size();
		int id = (int) std::floor(rand);
		rand = (rand - (double)id) * maxR;
		if (items[id]->rate>rand) return items[id];
	}
}


void AcceptanceRejection::updateElement(elementAR * it, double newRate) {
	currentRate += newRate - it->rate;

	if (newRate == 0)  {                    //if the event becomes impossible
		deleteElement(it);
		return;
	}
	//if it's larger than the largest encountered, updates it
	if (newRate > maxR) maxR = newRate;

	//if an event was impossible and becomes possible
	if (it->position == -1) {
		it->rate = newRate;
		addElement(it);            //added with the new rate
		return;
	}
	//otherwise just updates the rate
	it->rate = newRate;
}

elementAR* AcceptanceRejection::addElement(elementAR *it) {
	currentRate += it->rate;
	items.push_back(it);
	it->position = items.size();
	return it;
}

void AcceptanceRejection::deleteElement(elementAR *it) {
	if (it->position==-1) {
		it->rate = 0;                   //if it already was outside the structure, do nothing
		return;
	}
	//otherwise, switches it with the last and pops it
	items[it->position] = items[items.size()-1];
	items.pop_back();
	items[it->position]->position = it->position;
	it->rate = 0;
	it->position = -1;
}

