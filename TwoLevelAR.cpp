#include "TwoLevelAR.h"
#include <iostream>

event2AR *group2AR::sampleEvent() {
	while (true) {
		double random = randExt(rng) * (double)N;
		int eventId = std::floor(random);
		random = (random - (double) eventId) * max;
		if (random <= events[eventId]->rate) return events[eventId];
	}
}

event2AR *group2AR::addEvent(event2AR *it) {
	if (it->rate > max or it->rate < min) {
		std::cout << "WRONG GROUP (add)" << std::endl;
		return nullptr;
	}
	events.push_back(it);
	it->position = events.size() - 1;
	it->groupId = groupId;
	R += it->rate;
	N++;

	return it;
}

void group2AR::updateEvent(event2AR *it, double newRate) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}
	R += newRate - it->rate;
	it->rate = newRate;
}

void group2AR::deleteEvent(event2AR *it) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (delete)" << std::endl;
		return;
	}
	//if it's the only element, clears the structure
	if (N == 1) {
		events.clear();
		R = 0;
		N = 0;
		return;
	}

	N--;
	R -= it->rate;
	it->groupId = -1;
	events[it->position] = events[events.size() - 1];
	events[it->position]->position = it->position;
	events.pop_back();
}

TwoLevelAR::TwoLevelAR(double min, double max, double c, double B) : _max(max), _min(min), _c(c),
_divBase(1./std::log(c)), _g ((unsigned long) std::ceil(std::log(max/min) / std::log(c))), _B(B){

	rng.seed(time(nullptr));
	//creates groups according to grouping rule
	for (unsigned long i = 0; i < _g; i++){
		const double gMin = _min * std::pow(c,i);
		const double gMax = _min * std::pow(c,i+1);
		groups.emplace_back(gMin,gMax,i,bins.size());
		bins.emplace_back(i,0,0,true,true,0.);
	}

}

event2AR *TwoLevelAR::addEvent(unsigned long pl, double rate) {
	const auto groupId = getGroup(rate);
	if (groupId >= _g){
		std::cout << "Error, impossible group"<<std::endl;
		return nullptr;
	}

	auto pointer = groups[groupId].addEvent(new event2AR(pl,rate));
	
	//updates bin and creates a new one if necessary
	bins[groups[groupId].lastBinId].value += rate;
	if (bins[groups[groupId].lastBinId].value > _B){
		double diff = bins[groups[groupId].lastBinId].value - _B;
		bins[groups[groupId].lastBinId].value = _B;
		bins.emplace_back(groupId,groups[groupId].lastBinId,0,false,true,diff);
		groups[groupId].lastBinId = bins.size() - 1;
	}

	N += 1;
	R += rate;

	return pointer;
}

event2AR *TwoLevelAR::addEvent(event2AR *it) {
	const auto groupId = getGroup(it->rate);
	if (groupId >= _g){
		std::cout << "Error, impossible group"<<std::endl;
		return nullptr;
	}

	auto pointer = groups[groupId].addEvent(it);
	
	//updates bin and creates a new one if necessary
	bins[groups[groupId].lastBinId].value += it->rate;
	if (bins[groups[groupId].lastBinId].value > _B){
		double diff = bins[groups[groupId].lastBinId].value - _B;
		bins[groups[groupId].lastBinId].value = _B;
		bins.emplace_back(groupId,groups[groupId].lastBinId,0,false,true,diff);
		bins[groups[groupId].lastBinId].nextBin = bins.size() - 1;
		bins[groups[groupId].lastBinId].last = false;
		groups[groupId].lastBinId = bins.size() - 1;
	}

	N += 1;
	R += it->rate;


	return pointer;
}


event2AR *TwoLevelAR::sampleEvent() {
	while (true) {  //repeats until an event is returned
		double rand = randExt(rng) * (double)bins.size();
		int id = (int) std::floor(rand);
		rand = (rand - (double)id) * _B;
		if (bins[id].value > rand) return groups[bins[id].groupId].sampleEvent();
	}
}

void TwoLevelAR::updateEvent(event2AR *i, double newRate) {
	R += newRate - i->rate;
	
	//if it was impossible and becomes possible	
	if (i->rate == 0. and newRate > 0.){
		i->rate = newRate;
		addEvent(i);

		return;
	}

	//if it was possible
	if (i->rate > 0.){
		unsigned long const currGroup = i->groupId;
		if (currGroup >= _g){
			std::cout << "Error, impossible group"<<std::endl;
		}
		//if it becomes impossible
		if (newRate == 0){
			groups[currGroup].deleteEvent(i);
			}
		//updates the bin
		bins[groups[currGroup].lastBinId].value += newRate - i->rate;
		//deletes a bin if needed
		if (bins[groups[currGroup].lastBinId].value < 0){
			unsigned long curr = groups[currGroup].lastBinId;
			unsigned long prev = bins[groups[currGroup].lastBinId].prevBin;
			bins[prev].value += bins[groups[currGroup].lastBinId].value;
			bins[prev].last = true;
			groups[currGroup].lastBinId = prev;
			if (curr < bins.size() - 1){
				bins.pop_back();
			}
		}
		//creates a bin if necessary
		if (bins[groups[currGroup].lastBinId].value > _B){
		   double diff = bins[groups[currGroup].lastBinId].value - _B;
		   bins[groups[currGroup].lastBinId].value = _B;
		   bins.emplace_back(currGroup,groups[currGroup].lastBinId,0,false,true,diff);
		   bins[groups[currGroup].lastBinId].nextBin = bins.size() - 1;
		   bins[groups[currGroup].lastBinId].last = false;
		   groups[currGroup].lastBinId = bins.size() - 1;
		}
	}


}

unsigned long TwoLevelAR::getGroup(double r) const {
	if (r == _min) return 0;
	if (r == _max) return _g - 1;
	return (unsigned long)std::floor(std::log(r/_min)*_divBase);
}
