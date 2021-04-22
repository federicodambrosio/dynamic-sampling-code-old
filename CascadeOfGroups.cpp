#include "CascadeOfGroups.h"

CascadeOfGroups::CascadeOfGroups(bool rev, double minRate, double maxRate, double ratio):
_maxRate(maxRate),_minRate(minRate),_c(ratio),_divBase(1. / std::log(ratio)),_reversed(rev),
	_g((unsigned long) std::ceil(std::log(_maxRate/_minRate) / std::log(ratio))){
	rng.seed(time(nullptr));
	randExt = new std::uniform_real_distribution<double>(0,1);

	if (_reversed){
		for (unsigned long i = 0; i < _g; i++) {
			double groupMin = _minRate * pow(_c, _g-i-1);
			double groupMax = _minRate * pow(_c, _g-i);
			_groups.push_back(new singleGroup(groupMin, groupMax, i));
		}
	}
	else {
		for (unsigned long i = 0; i < _g; i++) {
			double groupMin = _minRate * pow(_c, i);
			double groupMax = _minRate * pow(_c, i + 1);
			_groups.push_back(new singleGroup(groupMin, groupMax, i));
		}
	}
	_totalRate = 0.;
	N = 0;
}

elementCoG *CascadeOfGroups::updateElement(elementCoG *it, double newRate) {
	if (newRate == it->rate) return it;                                                 //no change

	_totalRate += newRate - it->rate;                                                   //updates total rate

	if (newRate == 0) {                                                                 //event becomes impossible
		_groups[it->groupId]->deleteElement(it);
		it->groupId = -1;
		return it;
	}
	if (it->rate == 0) {                                                                //event was impossible
		it->rate = newRate;
		const auto groupId = getGroup(newRate);
		_groups[groupId]->addElement(it);
		return it;
	}

	const auto groupId = getGroup(newRate);

	if (groupId == it->groupId) {
		_groups[groupId]->updateElement(it, newRate);
		return it;
	} else {
		_groups[it->groupId]->deleteElement(it);
		it->rate = newRate;
		_groups[groupId]->addElement(it);
		return it;
	}
}


elementCoG* CascadeOfGroups::addElement(int payload, double rate) {
	if (rate <= 0) return nullptr;

	if (rate > _maxRate or rate < _minRate) {
		std::cout << "OOB RATE" << rate << "\t" << _maxRate << "\t" << _minRate << std::endl;
		return nullptr;
	}

	elementCoG *it = new elementCoG(payload, rate);
	auto const groupId = getGroup(rate);
	_groups[groupId]->addElement(it);
	_totalRate += rate;
	N++;
	return it;

}

elementCoG *CascadeOfGroups::sampleElement() {
	double randomRate = _totalRate * randExt->operator()(rng);
	int i = 0;

	while (true) {
		if (_groups[i]->totalRate > randomRate) return _groups[i]->sampleElement();

		if (i == _groups.size() - 1) {
			std::cout << _groups[i]->totalRate - randomRate << std::endl;
			return _groups[i]->sampleElement();
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			i++;
		}
	}
}


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


elementCoG *singleGroup::sampleElement() {
	while (true) {
		double random = randExt->operator()(rng) * N;
		int elementId = std::floor(random);
		random = (random - (double) elementId) * max;
		if (random <= elements[elementId]->rate) return elements[elementId];
	}
}


elementCoG *singleGroup::addElement(elementCoG *it) {
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

void singleGroup::updateElement(elementCoG *it, double newRate) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}
	double diff = newRate - it->rate;
	it->rate = newRate;
	totalRate = totalRate + diff;

}

void singleGroup::deleteElement(elementCoG *it) {
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

std::vector<double> CascadeOfGroups::rates(){
	std::vector<double> rates;
	
	for (auto g : _groups){
		rates.push_back(g->totalRate);
	}
	
	return rates;
}

