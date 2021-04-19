//
// Created by Federico D'Ambrosio on 31/03/2021.
//

#include "TwoLevelAR.h"
#include <iostream>

TwoLevelAR::TwoLevelAR(double min, double max, double c) : _max(max), _min(min), _c(c), _divBase(1./std::log(c)),
                                                           _g ((unsigned long) std::ceil(std::log(max/min) / std::log(c))){

	rng.seed(time(nullptr));
	randExt = new std::uniform_real_distribution<double>(0,1);

	for (unsigned long i = 0; i < _g; i++){
		const double gMin = _min * std::pow(c,i);
		const double gMax = _min * std::pow(c,i+1);
		groups.push_back(new group2AR(gMin,gMax,i));
	}

}

element2AR *TwoLevelAR::addElement(double rate) {
	const auto groupId = getGroup(rate);
	if (groupId >= _g){
		std::cout << "Error, impossible group"<<std::endl;
		return nullptr;
	}

	auto pointer = groups[groupId]->addElement(new element2AR(N,rate));

	N += 1;
	totalRate += rate;

	if (groups[groupId]->totalRate > currMax) currMax = groups[groupId]->totalRate;
	else updateMax();

	return pointer;
}

element2AR *TwoLevelAR::addElement(element2AR *it) {
	const auto groupId = getGroup(it->rate);
	if (groupId >= _g){
		std::cout << "Error, impossible group"<<std::endl;
		return nullptr;
	}

	auto pointer = groups[groupId]->addElement(it);

	N += 1;
	totalRate += it->rate;

	if (groups[groupId]->totalRate > currMax) currMax = groups[groupId]->totalRate;
	else updateMax();

	return pointer;
}

double TwoLevelAR::updateMax() {
	if (updateCounter > _g){
		updateCounter = 0;
		currMax = 0;
		for (auto g : groups){
			if (g->totalRate > currMax) currMax = g->totalRate;
		}
		return currMax;
	}
	updateCounter++;

	return (double)(updateCounter - _g);
}

element2AR *TwoLevelAR::sampleElement() {
	while (true) {  //repeats until an event is returned
		double rand = randExt->operator()(rng) * (double)groups.size();
		int id = (int) std::floor(rand);
		rand = (rand - (double)id) * currMax;
		if (groups[id]->totalRate>rand) return groups[id]->sampleElement();
	}
}


void TwoLevelAR::updateElement(element2AR *i, double newRate) {
	totalRate += newRate - i->rate;

	if (i->rate == 0.){
		i->rate = newRate;
		addElement(i);

		return;
	}


	if (i->rate > 0.){
		unsigned long const currGroup = getGroup(i->rate);
		if (currGroup >= _g){
			std::cout << "Error, impossible group"<<std::endl;
		}

		if (newRate == 0){
			groups[currGroup]->deleteElement(i);
			updateMax();
		}

		unsigned long const newGroup = getGroup(newRate);

		if (newGroup == currGroup){
			groups[currGroup]->updateElement(i,newRate);
			if (groups[currGroup]->totalRate > currMax) currMax = groups[currGroup]->totalRate;
			else updateMax();
		} else {
			groups[currGroup]->deleteElement(i);
			i->rate = newRate;
			groups[newGroup]->addElement(i);
			if (groups[newGroup]->totalRate > currMax) currMax = groups[newGroup]->totalRate;
			updateMax();
		}
	}


}

unsigned long TwoLevelAR::getGroup(double r) const {
	if (r == _min) return 0;
	if (r == _max) return _g - 1;
	return (unsigned long)std::ceil(std::log(r/_min)*_divBase);
}

element2AR *group2AR::sampleElement() {
	while (true) {
		double random = randExt->operator()(rng) * (double)N;
		int elementId = std::floor(random);
		random = (random - (double) elementId) * max;
		if (random <= elements[elementId]->rate) return elements[elementId];
	}
}


element2AR *group2AR::addElement(element2AR *it) {
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

void group2AR::updateElement(element2AR *it, double newRate) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}
	double diff = newRate - it->rate;
	it->rate = newRate;
	totalRate = totalRate + diff;
}

void group2AR::deleteElement(element2AR *it) {
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
