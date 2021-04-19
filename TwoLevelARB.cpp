//
// Created by Federico D'Ambrosio on 01/04/2021.
//

#include "TwoLevelARB.h"
#include <iostream>

element2ARB *group2ARB::sampleElement() {
	while (true) {
		double random = randExt->operator()(rng) * (double)N;
		int elementId = std::floor(random);
		random = (random - (double) elementId) * max;
		if (random <= elements[elementId]->rate) return elements[elementId];
	}
}

element2ARB *group2ARB::addElement(element2ARB *it) {
	if (it->rate > max or it->rate < min) {
		std::cout << "WRONG GROUP (add)" << std::endl;
		return nullptr;
	}
	elements.push_back(it);
	it->moveId = elements.size() - 1;
	it->groupId = groupId;
	totalRate += it->rate;
	N++;

	return it;
}

void group2ARB::updateElement(element2ARB *it, double newRate) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}
	double diff = newRate - it->rate;
	it->rate = newRate;
	totalRate = totalRate + diff;
}

void group2ARB::deleteElement(element2ARB *it) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (delete)" << std::endl;
		return;
	}
	if (N == 1) {
		elements.clear();
		totalRate = 0;
		N = 0;
		return;
	}

	N--;
	totalRate -= it->rate;
	it->groupId = -1;
	elements[it->moveId] = elements[elements.size() - 1];
	elements[it->moveId]->moveId = it->moveId;
	elements.pop_back();
}

TwoLevelARB::TwoLevelARB(double min, double max, double c, double B) : _max(max), _min(min), _c(c),
_divBase(1./std::log(c)), _g ((unsigned long) std::ceil(std::log(max/min) / std::log(c))), _B(B){

	rng.seed(time(nullptr));
	randExt = new std::uniform_real_distribution<double>(0,1);

	for (unsigned long i = 0; i < _g; i++){
		const double gMin = _min * std::pow(c,i);
		const double gMax = _min * std::pow(c,i+1);
		groups.push_back(new group2ARB(gMin,gMax,i,bins.size()));
		bins.emplace_back(i,0,0,true,true,0.);
	}

}

element2ARB *TwoLevelARB::addElement(double rate) {
	const auto groupId = getGroup(rate);
	if (groupId >= _g){
		std::cout << "Error, impossible group"<<std::endl;
		return nullptr;
	}

	auto pointer = groups[groupId]->addElement(new element2ARB(N,rate));

	bins[groups[groupId]->lastBinId].value += rate;
	if (bins[groups[groupId]->lastBinId].value > _B){
		double diff = bins[groups[groupId]->lastBinId].value - _B;
		bins[groups[groupId]->lastBinId].value = _B;
		bins.emplace_back(groupId,groups[groupId]->lastBinId,0,false,true,diff);
		groups[groupId]->lastBinId = bins.size() - 1;
	}

	N += 1;
	totalRate += rate;

	return pointer;
}

element2ARB *TwoLevelARB::addElement(element2ARB *it) {
	const auto groupId = getGroup(it->rate);
	if (groupId >= _g){
		std::cout << "Error, impossible group"<<std::endl;
		return nullptr;
	}

	auto pointer = groups[groupId]->addElement(it);
	bins[groups[groupId]->lastBinId].value += it->rate;
	if (bins[groups[groupId]->lastBinId].value > _B){
		double diff = bins[groups[groupId]->lastBinId].value - _B;
		bins[groups[groupId]->lastBinId].value = _B;
		bins.emplace_back(groupId,groups[groupId]->lastBinId,0,false,true,diff);
		bins[groups[groupId]->lastBinId].nextBin = bins.size() - 1;
		bins[groups[groupId]->lastBinId].last = false;
		groups[groupId]->lastBinId = bins.size() - 1;
	}

	N += 1;
	totalRate += it->rate;


	return pointer;
}


element2ARB *TwoLevelARB::sampleElement() {
	while (true) {  //repeats until an event is returned
		double rand = randExt->operator()(rng) * (double)bins.size();
		int id = (int) std::floor(rand);
		rand = (rand - (double)id) * _B;
		if (bins[id].value > rand) return groups[bins[id].groupId]->sampleElement();
	}
}

void TwoLevelARB::updateElement(element2ARB *i, double newRate) {
	totalRate += newRate - i->rate;

	if (i->rate == 0.){
		i->rate = newRate;
		addElement(i);

		return;
	}


	if (i->rate > 0.){
		unsigned long const currGroup = i->groupId;
		if (currGroup >= _g){
			std::cout << "Error, impossible group"<<std::endl;
		}

		if (newRate == 0){
			groups[currGroup]->deleteElement(i);
			bins[groups[currGroup]->lastBinId].value -= i->rate;
			if (bins[groups[currGroup]->lastBinId].value < 0){
				unsigned long curr = groups[currGroup]->lastBinId;
				unsigned long prev = bins[groups[currGroup]->lastBinId].prevBin;
				bins[prev].value += bins[groups[currGroup]->lastBinId].value;
				bins[prev].last = true;
				groups[currGroup]->lastBinId = prev;

				if (curr < bins.size() - 1){
					bins.pop_back();
				}

				if (bins[prev].value < 0) std::cout << "Err, bin changed by more than B"<<std::endl;


			}
		}

	}


}

unsigned long TwoLevelARB::getGroup(double r) const {
	if (r == _min) return 0;
	if (r == _max) return _g - 1;
	return (unsigned long)std::ceil(std::log(r/_min)*_divBase);
}
