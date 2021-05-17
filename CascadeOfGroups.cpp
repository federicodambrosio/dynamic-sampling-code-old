#include "CascadeOfGroups.h"

CascadeOfGroups::CascadeOfGroups(bool rev, double minRate, double maxRate, double ratio):
_maxRate(maxRate),_minRate(minRate),_c(ratio),
_divBase(1. / std::log(ratio)),_reversed(rev),
	_g((unsigned long) std::ceil(std::log(_maxRate/_minRate) / std::log(ratio))){
	rng.seed(time(nullptr));

	//creates groups according to grouping rule
	if (_reversed){		//if reversed cascade
		for (unsigned long i = 0; i < _g; i++) {
			double groupMin = _minRate * pow(_c, _g-i-1);
			double groupMax = _minRate * pow(_c, _g-i);
			if (groupMax > _maxRate) groupMax = _maxRate;
			_groups.push_back(new singleGroup(groupMin, groupMax, i));
		}
	}
	else {				//if cascade
		for (unsigned long i = 0; i < _g; i++) {
			double groupMin = _minRate * pow(_c, i);
			double groupMax = _minRate * pow(_c, i + 1);
			if (groupMax > _maxRate) groupMax = _maxRate;
			_groups.push_back(new singleGroup(groupMin, groupMax, i));
		}
	}
	R = 0.;
	N = 0;
}

eventCoG *CascadeOfGroups::updateEvent(eventCoG *it, double newRate) {
	//if rate is unchanged, do nothing
	if (newRate == it->rate) return it;                                                 
	
	//updates total rate
	R += newRate - it->rate;                                                  
	
	//if event becomes impossible, deletes it
	if (newRate == 0) {                                                                 
		_groups[it->groupId]->deleteEvent(it);
		it->groupId = -1;
		return it;
	}
	
	//if event was impossible, adds it
	if (it->rate == 0) {                                                                
		it->rate = newRate;
		const auto groupId = getGroup(newRate);
		_groups[groupId]->addEvent(it);
		return it;
	}
	//what's the correct group for the newRate?
	const auto groupId = getGroup(newRate);
	
	//if it stays in the same group
	if (groupId == it->groupId) {
		_groups[groupId]->updateEvent(it, newRate);
		return it;
	} else {
	//if it changes group
		_groups[it->groupId]->deleteEvent(it);
		it->rate = newRate;
		_groups[groupId]->addEvent(it);
		return it;
	}
}


eventCoG* CascadeOfGroups::addEvent(unsigned long payload, double rate) {
	//sanity check
	if (rate > _maxRate or rate < _minRate) {
		std::cout << "OOB RATE" << rate << "\t" << _maxRate << "\t" << _minRate << std::endl;
		return nullptr;
	}
	//creates event
	eventCoG *it = new eventCoG(payload, rate);
	//puts it in the right group
	auto const groupId = getGroup(rate);
	_groups[groupId]->addEvent(it);
	
	R += rate;
	N++;
	return it;

}

eventCoG *CascadeOfGroups::sampleEvent() {
	//random number between zero and R
	double randomRate = R * randExt(rng);
	int i = 0;

	while (true) {
		//if the totalrate of the group i is bigger than randomRate
		if (_groups[i]->R > randomRate) return _groups[i]->sampleEvent();
		
		randomRate = randomRate - _groups[i]->R;
		i++;
	}
}

//grouping law
unsigned long CascadeOfGroups::getGroup(const double r) const {
	if (_reversed) {
		if (r == _minRate) return _g - 1;
		if (r == _maxRate) return 0;
		return (_g - (unsigned long)std::ceil(std::log(r/_minRate)*_divBase));
	}
	else {
		if (r == _minRate) return 0;
		if (r == _maxRate) return _g - 1;
		const auto id = (unsigned long)std::floor(std::log(r/_minRate)*_divBase);
		return id;
	}
}


eventCoG *singleGroup::sampleEvent() {
	while (true) {
		double random = randExt(rng) * N;
		int eventId = std::floor(random);
		random = (random - (double) eventId) * max;
		if (random <= events[eventId]->rate) return events[eventId];
	}
}


eventCoG *singleGroup::addEvent(eventCoG *it) {
	if (it->rate > max or it->rate < min) {
		//sanity check
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

void singleGroup::updateEvent(eventCoG *it, double newRate) {
	if (it->groupId != groupId) {			//sanity check
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}
	R += newRate - it->rate;
	it->rate = newRate;

}

void singleGroup::deleteEvent(eventCoG *it) {
	if (it->groupId != groupId) {			//sanity check
		std::cout << "WRONG GROUP (delete)" << std::endl;
		return;
	}
	//if there's only an event, we clear the group
	if (N == 1) {
		events.clear();
		R = 0;
		N = 0;
		return;
	}

	N--;
	R -= it->rate;
	it->groupId = -1;
	//overwrites the event with the last one in the group
	events[it->position] = events[events.size() - 1];
	events[it->position]->position = it->position;
	events.pop_back();

}


