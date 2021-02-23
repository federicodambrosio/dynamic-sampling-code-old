//
// Created by federicodambrosio on 9-3-17.
//

#include "AcceptanceRejection.h"

elementAR* AcceptanceRejection::addMove(double rate) {
	if (rate<=0) return nullptr;
	//an event with payload = size of the data structure is created
	auto * aa = new elementAR(items.size(), items.size(),rate);
	items.push_back(aa);
	if (rate > maxR) maxR = rate;
	return aa;
}

elementAR * AcceptanceRejection::getMove() {
	while (true) {  //repeats until an event is returned
		double rand = randExt->operator()(rng) * (double)items.size();
		int id = (int) std::floor(rand);
		rand = (rand - (double)id) * maxR;
		if (items[id]->rate>rand) return items[id];
	}
}


void AcceptanceRejection::updateMove(elementAR * it, double newRate) {
	if (newRate == 0)  {                    //if the event becomes impossible
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

		return;
	}
	//if it's larger than the largest encountered, updates it
	if (newRate > maxR) maxR = newRate;

	//if an event was impossible and becomes possible
	if (it->position == -1) {
		it->rate = newRate;
		addMove(it);            //added with the new rate
		return;
	}
	//otherwise just updates the rate
	it->rate = newRate;
}

elementAR* AcceptanceRejection::addMove(elementAR *it) {
	items.push_back(it);
	it->position = items.size();
	return it;
}

elementAR *AcceptanceRejection::getMove(int &counter, int & randNumbers) {
	while (true) {
		counter++;
		double rand = randExt->operator()(rng);
		counter++;
		randNumbers++;
		rand = rand * (double) items.size();
		counter++;
		int id = (int) std::floor(rand);
		counter++;
		rand = (rand - (double)id);
		counter++;
		rand = rand* maxR;
		counter++;
		counter++; //for the next instruction
		if (items[id]->rate>rand) {
			return items[id];
		}
	}
}



void AcceptanceRejection::updateMove(elementAR *it, double newRate, int &counter) {
	counter++; //for the next instruction
	if (newRate == 0)  {
		counter++; //for the next instruction
		if (it->position==-1) {
			it->rate = 0;
			counter++;
			return;
		}

		items[it->position] = items[items.size()-1];
		counter++;
		items.pop_back();
		counter++;
		items[it->position]->position = it->position;
		counter++;
		it->rate = 0;
		counter++;
		it->position = -1;
		counter++;

		return;
	}
	if (newRate > maxR) maxR = newRate;
	counter++;
	counter++; //for the next instruction
	if (it->position == -1) {
		it->rate = newRate;
		counter++;
		addMove(it, counter);
		counter++;
		return;
	}

	it->rate = newRate;
	counter++;
}

elementAR* AcceptanceRejection::addMove(elementAR *it, int &counter) {
	items.push_back(it);
	counter++;
	it->position = items.size();
	counter++;
	if (it->rate > maxR) maxR = it->rate;
	counter++;
	return it;

}



