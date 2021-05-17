
#include "AcceptanceRejection.h"

eventAR* AcceptanceRejection::addEvent(unsigned long payload, double rate) {
	
	auto * aa = new eventAR(payload, items.size(),rate);
	items.push_back(aa);
	if (rate > maxR) maxR = rate;
	R += rate;
	return aa;
}

eventAR * AcceptanceRejection::sampleEvent() {
	while (true) {  //repeats until an event is returned
		double rand = randExt(rng) * (double)items.size();
		unsigned long id = (unsigned long) std::floor(rand);
		rand = (rand - (double)id) * maxR;
		if (items[id]->rate>rand) return items[id];
	}
}


void AcceptanceRejection::updateEvent(eventAR * it, double newRate) {
	R += newRate - it->rate;
	
	//if the event becomes impossible
	if (newRate == 0)  {                    
		deleteEvent(it);
		return;
	}
	//if it's larger than the largest encountered, updates it
	if (newRate > maxR) maxR = newRate;

	//if an event was impossible and becomes possible
	if (it->rate == 0) {
		it->rate = newRate;
		addEvent(it);            //added with the new rate
		return;
	}
	//otherwise just updates the rate
	it->rate = newRate;
}

eventAR* AcceptanceRejection::addEvent(eventAR *it) {
	R += it->rate;
	items.push_back(it);
	it->position = items.size();
	return it;
}

void AcceptanceRejection::deleteEvent(eventAR *it) {
	//if it's already impossible, do nothing
	if (it->rate==0) return;
	
	
	//otherwise, switches it with the last and pops it, updates total rate
	R -= it->rate;
	items[it->position] = items[items.size()-1];
	items.pop_back();
	items[it->position]->position = it->position;
	it->rate = 0;
	
	
}

