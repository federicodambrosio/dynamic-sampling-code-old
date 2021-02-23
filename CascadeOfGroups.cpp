//
// Created by federicodambrosio on 17-3-17.
//

#include "CascadeOfGroups.h"

item *NestedGroupsLarge::updateItem(item *it, double newRate) {
	if (newRate == it->rate) return it;

	_totalRate += newRate - it->rate;

	if (newRate == 0) {
		_groups[it->groupId]->deleteItem(it);
		it->groupId = -1;
		return it;
	}
	if (it->rate == 0) {
		it->rate = newRate;
		int groupId = (int) std::floor(std::log(_maxRate / newRate) * _divBase);
		_groups[groupId]->addItem(it);
		return it;
	}

	int groupId = (int) std::floor(std::log(_maxRate / newRate) * _divBase);

	if (groupId == it->groupId) {
		_groups[groupId]->updateItem(it, newRate);
		return it;
	} else {
		_groups[it->groupId]->deleteItem(it);
		it->rate = newRate;
		_groups[groupId]->addItem(it);
		return it;
	}
}

item *NestedGroupsLarge::extractItem(double random) {
	double totRate = _totalRate;
	double randomRate = totRate * random;
	int i = 0;

	while (true) {
		if (_groups[i]->totalRate > randomRate) return _groups[i]->extractItem();
		if (i == _groups.size() - 1) {
			std::cout << _groups[i]->totalRate - randomRate << std::endl;
			return _groups[i]->extractItem();
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			i++;

		}
	}
}

item* NestedGroupsLarge::addItem(int payload, double rate) {
	if (rate <= 0) return NULL;

	if (rate > _maxRate or rate < _minRate) {
		std::cout << "OOB RATE" << std::endl;
		return NULL;
	}

	item *it = new item(payload, rate);
	int groupId = (int) std::floor(std::log(_maxRate / rate) * _divBase);
	_groups[groupId]->addItem(it);
	_totalRate += rate;
	_N++;
	return it;

}

NestedGroupsLarge::NestedGroupsLarge(double minRate, double maxRate, int N, double ratio) {
	rng.seed(time(0));
	randExt = new std::uniform_real_distribution<double>(0,1);
	_maxRate = maxRate;
	_minRate = minRate;
	_ratio = ratio;
	_divBase = 1. / std::log(_ratio);
	int numberOfGroups = (int) std::floor(std::log(_maxRate / _minRate) * _divBase) + 1;
	for (int i = 0; i < numberOfGroups; i++) {
		double groupMax = _maxRate / pow(ratio, i);
		double groupMin = _maxRate / pow(ratio, i + 1);
		_groups.push_back(new singleGroupLarge(groupMin, groupMax, i, N));
	}
	_totalRate = 0;
	_N = 0;

}

item *NestedGroupsLarge::extractItem(double random, RunningStats &deep, RunningStats &iter) {
	double totRate = _totalRate;
	double randomRate = totRate * random;
	int i = 0;

	while (true) {
		if (_groups[i]->totalRate > randomRate) {
			deep.Push(i);
			return _groups[i]->extractItem(iter);
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			i++;
		}
	}
}

item* NestedGroupsLarge::addItem(int payload, double rate, int &counter) {
/*	counter++;
	if (rate <= 0) return NULL;

	counter++;
	if (rate > _maxRate or rate < _minRate) {
		std::cout << "OOB RATE" << std::endl;
		return NULL;
	}*/

	item *it = new item(payload, rate);
	counter++;
	int groupId = (int) std::floor(std::log(_maxRate / rate) * _divBase);
	counter+=3;
	_groups[groupId]->addItem(it, counter);
	counter++;
	_totalRate += rate;
	counter++;
	_N++;
	counter++;
	return it;
}

item *NestedGroupsLarge::extractItem(double random, int &counter, int & randNumbers) {
	double totRate = _totalRate;
	counter++;
	double randomRate = totRate * random;
	counter++;
	int i = 0;
	counter++;

	counter++;
	while (true) {
		counter++;
		if (_groups[i]->totalRate > randomRate) return _groups[i]->extractItem(counter, randNumbers);

		counter++;
		if (i == _groups.size() - 1) {
			return _groups[i]->extractItem(counter, randNumbers);
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			counter++;
			i++;
			counter++;
		}
	}
}


item *NestedGroupsLarge::updateItem(item *it, double newRate, int &counter) {
	counter++;
	if (newRate == it->rate) return it;

	_totalRate += newRate - it->rate;
	counter++;

	counter++;
	if (newRate == 0) {
		_groups[it->groupId]->deleteItem(it, counter);
		counter++;
		it->groupId = -1;
		counter++;
		return it;
	}
	counter++;
	if (it->rate == 0) {
		it->rate = newRate;
		counter++;
		int groupId = (int) std::floor(std::log(_maxRate / newRate) * _divBase);
		counter+=3;
		_groups[groupId]->addItem(it, counter);
		counter++;
		return it;
	}

	int groupId = (int) std::floor(std::log(_maxRate / newRate) * _divBase);
	counter+=3;

	counter++;
	if (groupId == it->groupId) {
		_groups[groupId]->updateItem(it, newRate, counter);
		counter++;
		return it;
	} else {
		_groups[it->groupId]->deleteItem(it, counter);
		counter++;
		it->rate = newRate;
		counter++;
		_groups[groupId]->addItem(it, counter);
		counter++;
		return it;
	}
}

item *NestedGroupsLarge::extractItem() {
	double totRate = _totalRate;
	double randomRate = totRate * randExt->operator()(rng);
	int i = 0;

	while (true) {
		if (_groups[i]->totalRate > randomRate) return _groups[i]->extractItem();

		if (i == _groups.size() - 1) {
			std::cout << _groups[i]->totalRate - randomRate << std::endl;
			return _groups[i]->extractItem();
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			i++;
		}
	}
}

item *NestedGroupsLarge::extractItem(int &counter, int & randNumbers) {
	double totRate = _totalRate;
	counter++;
	double randomRate = totRate * randExt->operator()(rng);
	randNumbers++;
	counter+=2;
	int i = 0;
	counter++;

	counter++;
	while (true) {
		counter++;
		if (_groups[i]->totalRate > randomRate) {
			counter++;
			return _groups[i]->extractItem(counter, randNumbers);
		}

		counter++;
		if (i == _groups.size() - 1) {
			counter++;
			return _groups[i]->extractItem(counter, randNumbers);
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			counter++;
			i++;
			counter++;
		}
	}
}




item *singleGroupLarge::extractItem() {
	while (true) {
		double random = randExt->operator()(rng) * N;
		int itemId = std::floor(random);
		random = (random - (double) itemId) * max;
		if (random <= items[itemId]->k) return items[itemId];
	}
}


item *singleGroupLarge::addItem(item *it) {
	if (it->rate > max or it->rate < min) {
		std::cout << "WRONG GROUP (add)" << std::endl;
		return NULL;
	}
	items.push_back(it);
	it->moveId = items.size() - 1;
	it->groupId = groupId;
	totalRate += it->rate;
	N++;
	return it;
}

void singleGroupLarge::updateItem(item *it, double newRate) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}
	double diff = newRate - it->rate;
	it->rate = newRate;
	totalRate = totalRate + diff;

}

void singleGroupLarge::deleteItem(item *it) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (delete)" << std::endl;
		return;
	}
	if (N == 1) {
		items.clear();
		totalRate = 0;
		N = 0;
		return;
	}

	N--;
	totalRate -= it->rate;
	it->groupId = -1;
	items[it->moveId] = items[items.size() - 1];
	items[it->moveId]->elementID = it->moveId;
	items.pop_back();

}

item *singleGroupLarge::extractItem(RunningStats &rs) {
	double iter = 1;
	while (true) {
		double random = randExt->operator()(rng) * N;
		int itemId = std::floor(random);
		random = (random - (double) itemId) * max;
		if (random <= items[itemId]->k) {
			rs.Push(iter);
			return items[itemId];
		}
		iter++;
	}
}

item *singleGroupLarge::addItem(item *it, int &counter) {
	/*if (it->rate > max or it->rate < min) {
		std::cout << "WRONG GROUP (add)" << std::endl;
		return NULL;
	}*/
	items.push_back(it);
	counter++;
	it->moveId = items.size() - 1;
	counter++;
	it->groupId = groupId;
	counter++;
	totalRate += it->rate;
	counter++;
	N++;
	counter++;

	return it;
}

item *singleGroupLarge::extractItem(int &counter, int & randNumbers) {
	while (true) {
		double random = randExt->operator()(rng) * N;
		randNumbers++;
		int itemId = std::floor(random);
		counter++;
		random = (random - (double) itemId) * max;
		counter++;
		if (random <= items[itemId]->k) return items[itemId];
	}
}

void singleGroupLarge::updateItem(item *it, double newRate, int &counter) {
	/*if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}*/
	double diff = newRate - it->rate;
	counter++;
	it->rate = newRate;
	counter++;
	totalRate = totalRate + diff;
	counter++;
}

void singleGroupLarge::deleteItem(item *it, int &counter) {
	/*if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (delete)" << std::endl;
		return;
	}*/
	counter++;
	if (N == 1) {
		items.clear();
		counter++;
		totalRate = 0;
		counter++;
		N = 0;
		counter++;
		return;
	}

	N--;
	counter++;
	totalRate -= it->rate;
	counter++;
	it->groupId = -1;
	counter++;
	items[it->moveId] = items[items.size() - 1];
	counter++;
	items[it->moveId]->elementID = it->moveId;
	counter++;
	items.pop_back();
	counter++;
}


item *CascadeOfGroups::updateItem(item *it, double newRate) {
	if (newRate == it->rate) return it;

	_totalRate += newRate - it->rate;

	if (newRate == 0) {
		_groups[it->groupId]->deleteItem(it);
		it->groupId = -1;
		return it;
	}
	if (it->rate == 0) {
		it->rate = newRate;
		int groupId = (int) std::floor(std::log(_maxRate / newRate) * _divBase);
		_groups[groupId]->addItem(it);
		return it;
	}

	int groupId = (int) std::floor(std::log(_maxRate / newRate) * _divBase);

	if (groupId == it->groupId) {
		_groups[groupId]->updateItem(it, newRate);
		return it;
	} else {
		_groups[it->groupId]->deleteItem(it);
		it->rate = newRate;
		_groups[groupId]->addItem(it);
		return it;
	}
}

item *CascadeOfGroups::extractItem(double random) {
	double totRate = _totalRate;
	double randomRate = totRate * random;
	int i = 0;

	while (true) {
		if (_groups[i]->totalRate > randomRate) return _groups[i]->extractItem();
		if (i == _groups.size() - 1) {
			std::cout << _groups[i]->totalRate - randomRate << std::endl;
			return _groups[i]->extractItem();
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			i++;

		}
	}
}

item* CascadeOfGroups::addItem(int payload, double rate) {
	if (rate <= 0) return NULL;

	if (rate > _maxRate or rate < _minRate) {
		std::cout << "OOB RATE" << std::endl;
		return NULL;
	}

	item *it = new item(payload, rate);
	int groupId = (int) std::floor(std::log(_maxRate / rate) * _divBase);
	_groups[groupId]->addItem(it);
	_totalRate += rate;
	_N++;
	return it;

}

CascadeOfGroups::CascadeOfGroups(double minRate, double maxRate, int N, double ratio) {
	rng.seed(time(0));
	randExt = new std::uniform_real_distribution<double>(0,1);
	_maxRate = maxRate;
	_minRate = minRate;
	_ratio = ratio;
	_divBase = 1. / std::log(_ratio);
	int numberOfGroups = (int) std::floor(std::log(_maxRate / _minRate) * _divBase) + 1;
	for (int i = 0; i < numberOfGroups; i++) {
		double groupMax = _maxRate / pow(ratio, i);
		double groupMin = _maxRate / pow(ratio, i + 1);
		_groups.push_back(new singleGroup(groupMin, groupMax, i, N));
	}
	_totalRate = 0;
	_N = 0;

}

item *CascadeOfGroups::extractItem(double random, RunningStats &deep, RunningStats &iter) {
	double totRate = _totalRate;
	double randomRate = totRate * random;
	int i = 0;

	while (true) {
		if (_groups[i]->totalRate > randomRate) {
			deep.Push(i);
			return _groups[i]->extractItem(iter);
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			i++;
		}
	}
}

item* CascadeOfGroups::addItem(int payload, double rate, int &counter) {
	counter++;
	if (rate <= 0) return NULL;

	/*if (rate > _maxRate or rate < _minRate) {
		std::cout << "OOB RATE" << std::endl;
		return NULL;
	}*/

	item *it = new item(payload, rate);
	counter++;
	int groupId = (int) std::floor(std::log(_maxRate / rate) * _divBase);
	counter+=3;
	_groups[groupId]->addItem(it, counter);
	counter++;
	_totalRate += rate;
	counter++;
	_N++;
	counter++;
	return it;
}

item *CascadeOfGroups::extractItem(double random, int &counter, int & randNumbers) {
	double totRate = _totalRate;
	counter++;
	double randomRate = totRate * random;
	counter++;
	int i = 0;
	counter++;

	while (true) {
		counter++;
		if (_groups[i]->totalRate > randomRate){
			counter++;
			return _groups[i]->extractItem(counter, randNumbers);
		}
		counter++;
		if (i == _groups.size() - 1) {
			counter++;
			return _groups[i]->extractItem(counter, randNumbers);
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			counter++;
			i++;
			counter++;
		}
	}
}


item *CascadeOfGroups::updateItem(item *it, double newRate, int &counter) {
	counter++;
	if (newRate == it->rate) return it;

	_totalRate += newRate - it->rate;
	counter++;

	counter++;
	if (newRate == 0) {
		_groups[it->groupId]->deleteItem(it, counter);
		counter++;
		it->groupId = -1;
		counter++;
		return it;
	}
	counter++;
	if (it->rate == 0) {
		it->rate = newRate;
		counter++;
		int groupId = (int) std::floor(std::log(_maxRate / newRate) * _divBase);
		counter+=3;
		_groups[groupId]->addItem(it, counter);
		counter++;
		return it;
	}

	int groupId = (int) std::floor(std::log(_maxRate / newRate) * _divBase);
	counter+=3;

	counter++;
	if (groupId == it->groupId) {
		counter++;
		_groups[groupId]->updateItem(it, newRate, counter);
		return it;
	} else {
		_groups[it->groupId]->deleteItem(it, counter);
		counter++;
		it->rate = newRate;
		counter++;
		_groups[groupId]->addItem(it, counter);
		counter++;
		return it;
	}
}

item *CascadeOfGroups::extractItem() {
	double totRate = _totalRate;
	double randomRate = totRate * randExt->operator()(rng);
	int i = 0;

	while (true) {
		if (_groups[i]->totalRate > randomRate) return _groups[i]->extractItem();

		if (i == _groups.size() - 1) {
			std::cout << _groups[i]->totalRate - randomRate << std::endl;
			return _groups[i]->extractItem();
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			i++;
		}
	}
}

item *CascadeOfGroups::extractItem(int &counter, int & randNumbers) {
	double totRate = _totalRate;
	counter++;
	double randomRate = totRate * randExt->operator()(rng);
	randNumbers++;
	counter++;
	int i = 0;
	counter++;

	while (true) {
		counter++;
		counter++;
		if (_groups[i]->totalRate > randomRate) {
			counter++;
			return _groups[i]->extractItem(counter, randNumbers);
		}

		counter++;
		if (i == _groups.size() - 1) {
			counter++;
			//std::cout << _groups[i]->totalRate - randomRate << std::endl;
			counter++;
			return _groups[i]->extractItem(counter, randNumbers);
		} else {
			randomRate = randomRate - _groups[i]->totalRate;
			counter++;
			i++;
			counter++;
		}
	}
}




item *singleGroup::extractItem() {
	while (true) {
		double random = randExt->operator()(rng) * N;
		int itemId = std::floor(random);
		random = (random - (double) itemId) * max;
		if (random <= items[itemId]->rate) return items[itemId];
	}
}


item *singleGroup::addItem(item *it) {
	if (it->rate > max or it->rate < min) {
		std::cout << "WRONG GROUP (add)" << std::endl;
		return NULL;
	}
	items.push_back(it);
	it->moveId = items.size() - 1;
	it->groupId = groupId;
	totalRate += it->rate;
	N++;
	return it;
}

void singleGroup::updateItem(item *it, double newRate) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}
	double diff = newRate - it->rate;
	it->rate = newRate;
	totalRate = totalRate + diff;

}

void singleGroup::deleteItem(item *it) {
	if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (delete)" << std::endl;
		return;
	}
	if (N == 1) {
		items.clear();
		totalRate = 0;
		N = 0;
		return;
	}

	N--;
	totalRate -= it->rate;
	it->groupId = -1;
	items[it->moveId] = items[items.size() - 1];
	items[it->moveId]->moveId = it->moveId;
	items.pop_back();

}

item *singleGroup::extractItem(RunningStats &rs) {
	double iter = 1;
	while (true) {
		double random = randExt->operator()(rng) * N;
		int itemId = std::floor(random);
		random = (random - (double) itemId) * max;
		if (random <= items[itemId]->rate) {
			rs.Push(iter);
			return items[itemId];
		}
		iter++;
	}
}

item *singleGroup::addItem(item *it, int &counter) {
	/*if (it->rate > max or it->rate < min) {
		std::cout << "WRONG GROUP (add)" << std::endl;
		return NULL;
	}*/
	items.push_back(it);
	counter++;
	it->moveId = items.size() - 1;
	counter++;
	it->groupId = groupId;
	counter++;
	totalRate += it->rate;
	counter++;
	N++;
	counter++;

	return it;
}

item *singleGroup::extractItem(int &counter, int & randNumbers) {
	while (true) {
		counter++;
		double random = randExt->operator()(rng) * N;
		counter++;
		randNumbers++;
		int itemId = std::floor(random);
		counter++;
		random = (random - (double) itemId) * max;
		counter+=2;
		counter++;
		if (random <= items[itemId]->rate) return items[itemId];
	}
}

void singleGroup::updateItem(item *it, double newRate, int &counter) {
	/*if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (update)" << std::endl;
		return;
	}*/
	double diff = newRate - it->rate;
	counter++;
	it->rate = newRate;
	counter++;
	totalRate = totalRate + diff;
	counter++;
}

void singleGroup::deleteItem(item *it, int &counter) {
	/*if (it->groupId != groupId) {
		std::cout << "WRONG GROUP (delete)" << std::endl;
		return;
	}*/
	counter++;
	if (N == 1) {
		items.clear();
		counter++;
		totalRate = 0;
		counter++;
		N = 0;
		counter++;
		return;
	}

	N--;
	counter++;
	totalRate -= it->rate;
	counter++;
	it->groupId = -1;
	counter++;
	items[it->moveId] = items[items.size() - 1];
	counter++;
	items[it->moveId]->moveId = it->moveId;
	counter++;
	items.pop_back();
	counter++;
}