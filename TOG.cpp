//
// Created by federicodambrosio on 6-2-17.
//

#include "TOG.h"
#include "RunningStats.hpp"

TOG::TOG(double pmax, double pmin, double _ratio) : maxR(pmax), minR(pmin), k(_ratio) {
	rng.seed(time(nullptr));
	randExt = new std::uniform_real_distribution<double>(0,1);

	logBase = 1. / log2(k);

	int index = 0;
	double min = minR;
	while (min < maxR) {
		double max = k * min;
		if (max > maxR) max = maxR;
		auto * g = new groupTOG(index, max, min, k);
		//std::cout<<index<<"\t"<<min<<"\t"<<max<<"\t"<<std::endl;
		g->node = tree.addLeaf(index, 0);
		groups.push_back(g);

		min = max;
		index++;
		nGroups++;
	}
}

elementTOG * TOG::sampleElement(RunningStats & it) {
	std::uniform_real_distribution<double> random(0,1);
	double r2 = random(rng);
	CBTNode * extractedNode = tree.sampleLeaf(r2);
	int groupId = extractedNode->payload;
	int iteration=0;
	while (true){
		double r3 = random(rng);
		double r4 = random(rng);
		int elementId =(int)std::floor(r3 * (groups[groupId]->totalN-1));
		r4 = r4 * groups[groupId]->maxR;
		iteration++;
		if (groups[groupId]->elements[elementId]->rate >= r4 ) {
			it.Push(iteration);
			/*if (iteration > 35) {
				std::cout<<"Iterations: "<<iteration<<std::endl;
			}*/
			return groups[groupId]->elements[elementId];
		}
	}

}

void TOG::updateElement(elementTOG * m, double newRate) {
	/*if (!checkGroups()) {
		std::cout<<"Something wrong"<<std::endl;
	}*/

	//checkGroups();

	if (newRate == m->rate) return;

	int groupId = m->groupId;

	if (groupId == -1) {    //previously impossible elementTOG, no group assigned
		if (newRate==0) return;
		m->rate = newRate;
		addElement(m);
		return;
	}

	if (newRate == 0) {     //if the elementTOG is impossible, copies the last over it and shrink by one
		//int oldGroup = m->groupId;
		//int oldElement = m->elementID;
		groups[groupId]->deleteElement(m);
		totalN--;
		m->groupId = - 1; //empty so no group
		m->elementID = -1;
		m->rate = 0;

		/*if (m->isEqual(groups[oldGroup].elements[oldElement]) && groups[oldGroup].elements[oldElement]->rate>0){
			std::cout<<"SOMETHING WRONG"<<std::endl;
		}*/

		return;

	}


	if (groups[groupId]->isInside(newRate)) {
		groups[groupId]->updateElement(m,newRate);
		return;
	}

	if (!groups[groupId]->isInside(newRate)) {
		int newGroupId = (int) (floor (log2(newRate / minR) * logBase));
		groups[groupId]->deleteElement(m);
		m->rate = newRate;
		groups[newGroupId]->addElement(m);

		//debug code
		if (!groups[newGroupId]->isInside(m)) {
			std::cout<<"DID NOT MOVE TO CORRECT GROUP"<<std::endl;
		}
		//check(m,newRate);
		return;
	}


}

void TOG::update() {
	for (int i=0;i<groups.size();i++) {
		tree.updateLeaf(groups[i]->node, groups[i]->totalR);
	}
	//checkGroups();
	//checkGroups();
	tree.updateTree();
	//checkGroups();
}


void TOG::addElement(elementTOG * m) {
	if (m->rate == 0) {
		m->groupId = -1;
		return;
	}
	int groupId = (int) floor ((log2(m->rate / minR))*logBase);
	groups[groupId]->addElement(m);
	totalN++;
	return;
}



/*bool TOG::check(elementTOG m, double newRate) {
	if (newRate == 0) return true;

	if (groups[m.groupId].elements[m.elementID]->rate != newRate){
		std::cout<<"SOMETHING WRONG. Should: "<<newRate<<". Is "<<groups[m.groupId].elements[m.elementID]->rate<<std::endl;
		return false;
	}
	return true;
}

bool TOG::check(elementTOG * m, double newRate) {
    if (newRate == 0) return true;

    if (groups[m->groupId].elements[m->elementID]->rate != newRate){
        std::cout<<"SOMETHING WRONG. Should: "<<newRate<<". Is "<<groups[m->groupId].elements[m->elementID]->rate<<std::endl;
        return false;
    }
    return true;
}
*/
bool TOG::checkGroups(){
	bool check=true;
	for (int i=0; i<groups.size();i++) {
		if (!groups[i]->checkGroup()){
			std::cout<<"Problem with group "<<i<<std::endl;
			check = false;
		}
	};
	std::vector<double> rrr = tree.getAllRates();
	for (int i=0; i<groups.size();i++) {
		if(!(groups[i]->totalR == rrr[groups[i]->groupId])){
			if ((groups[i]->totalR - rrr[groups[i]->groupId]) / groups[i]->totalR > 1e-6) {
				std::cout<<i<<"\t"<<groups[i]->totalR<<"\t"<<groups[i]->totalR - rrr[groups[i]->groupId]<<std::endl;
				check = false;
			}
		}
	}

	return check;
}

/*std::vector<double> TOG::getAllRates() {
	std::vector<double> rates(totalN,0);

	for (int g=0; g<groups.size();g++) {
		for (int m = 0; m < groups[g]->totalN; m++) {
			if (rates[groups[g]->elements[m]->position] > 0) {
				std::cout << "Something wrong here!" << std::endl;
			}
			rates[groups[g]->elements[m]->position] = groups[g]->elements[m]->rate;
		}
	}
	return rates;
}*/

void TOG::printGroups() {
	for (int g=0; g<groups.size();g++){
		std::cout<<"Group "<<g<<": "<<std::endl;
		for (int m=0;m<groups[g]->totalN;m++)
			std::cout << groups[g]->elements[m]->payload << "\t";
		std::cout<<std::endl;
	}

}

elementTOG *TOG::sampleElement() {
	double r2 = randExt->operator()(rng);
	CBTNode * extractedNode = tree.sampleLeaf(r2);
	int groupId = extractedNode->payload;
	int iteration=0;
	while (true){
		double r3 = randExt->operator()(rng) * (groups[groupId]->totalN-1);
		int elementId =(int)std::floor(r3);
		double randElement = (r3 - (double)elementId) * groups[groupId]->maxR;
		iteration++;
		if (groups[groupId]->elements[elementId]->rate >= randElement ) {
			return groups[groupId]->elements[elementId];
		}
	}
}

elementTOG* TOG::addElement(int position, double rate) {
	elementTOG * m= new elementTOG(position, rate);
	addElement(m);
	return m;
}

elementTOG *TOG::sampleElement(int & counter, int & randNumbers) {
	double r2 = randExt->operator()(rng);
	randNumbers++;
	CBTNode * extractedNode = tree.sampleLeaf(r2, counter);
	counter++;
	int groupId = extractedNode->payload;
	counter++;
	//int iteration=0;
	counter++;
	while (true){
		double r3 = randExt->operator()(rng) * (groups[groupId]->totalN-1);
		randNumbers++;
		int elementId =(int)std::floor(r3);
		counter++;
		double randElement = (r3 - (double)elementId) * groups[groupId]->maxR;
		counter++;
		//iteration++;
		if (groups[groupId]->elements[elementId]->rate >= randElement ) {
			return groups[groupId]->elements[elementId];
		}
	}
}

void TOG::updateElement(elementTOG *m, double newRate, int &counter) {
/*if (!checkGroups()) {
		std::cout<<"Something wrong"<<std::endl;
	}*/

	//checkGroups();

	if (newRate == m->rate) return;

	int groupId = m->groupId;
	counter++;

	if (groupId == -1) {    //previously impossible elementTOG, no group assigned
		if (newRate==0) return;
		m->rate = newRate;
		counter++;
		addElement(m,counter);
		counter++;
		totalN++;
		counter++;
		return;
	}

	if (newRate == 0) {     //if the elementTOG is impossible, copies the last over it and shrink by one
		//int oldGroup = m->groupId;
		//int oldElement = m->elementID;
		groups[groupId]->deleteElement(m,counter);
		m->groupId = - 1; //empty so no group
		counter++;
		m->elementID = -1;
		counter++;
		m->rate = 0;
		counter++;
		totalN--;
		counter++;

		/*if (m->isEqual(groups[oldGroup].elements[oldElement]) && groups[oldGroup].elements[oldElement]->rate>0){
			std::cout<<"SOMETHING WRONG"<<std::endl;
		}*/

		return;

	}


	if (groups[groupId]->isInside(newRate)) {
		groups[groupId]->updateElement(m,newRate,counter);
		return;
	}

	if (!groups[groupId]->isInside(newRate)) {
		int newGroupId = (int) (floor (log2(newRate / minR) * logBase));
		counter++;
		groups[groupId]->deleteElement(m,counter);
		m->rate = newRate;
		counter++;
		groups[newGroupId]->addElement(m, counter);

		//debug code
		if (!groups[newGroupId]->isInside(m)) {
			std::cout<<"DID NOT MOVE TO CORRECT GROUP"<<std::endl;
		}
		//check(m,newRate);
		return;
	}


}

void TOG::update(int &counter) {
	for (int i=0;i<groups.size();i++) {
		tree.updateLeaf(groups[i]->node, groups[i]->totalR, counter);
		counter++;
	}
	//checkGroups();
	//checkGroups();
	tree.updateTree(counter);
	//checkGroups();
}

void TOG::addElement(elementTOG *m, int &counter) {
	if (m->rate == 0) {
		m->groupId = -1;
		counter++;
		return;
	}
	int groupId = (int) floor ((log2(m->rate / minR))*logBase);
	counter++;
	groups[groupId]->addElement(m, counter);
	counter++;
	totalN++;
	return;
}

//large

SSACRLarge::SSACRLarge(double pmax, double pmin, double _ratio) : maxR(pmax), minR(pmin), ratio(_ratio) {
	rng.seed(time(0));
	randExt = new std::uniform_real_distribution<double>(0,1);

	logBase = 1. / log2(ratio);


	int index = 0;
	double min = minR;
	while (min < maxR) {
		double max = ratio * min;
		if (max > maxR) max = maxR;
		groupTOG * g = new groupTOG(index, max, min, ratio);
		//std::cout<<index<<"\t"<<min<<"\t"<<max<<"\t"<<std::endl;
		g->node = tree.addLeaf(index, 0);
		groups.push_back(g);

		min = max;
		index++;
		nGroups++;
	}
}

elementTOG * SSACRLarge::getElement(RunningStats & it) {
	std::uniform_real_distribution<double> random(0,1);
	double r2 = random(rng);
	CBTNode * extractedNode = tree.sampleLeaf(r2);
	int groupId = extractedNode->payload;
	int iteration=0;
	while (true){
		double r3 = random(rng);
		double r4 = random(rng);
		int elementId =(int)std::floor(r3 * (groups[groupId]->totalN-1));
		r4 = r4 * groups[groupId]->maxR;
		iteration++;
		if (groups[groupId]->elements[elementId]->rate >= r4 ) {
			it.Push(iteration);
			/*if (iteration > 35) {
				std::cout<<"Iterations: "<<iteration<<std::endl;
			}*/
			return groups[groupId]->elements[elementId];
		}
	}

}

void SSACRLarge::updateElement(elementTOG * m, double newRate) {
	/*if (!checkGroups()) {
		std::cout<<"Something wrong"<<std::endl;
	}*/

	//checkGroups();

	if (newRate == m->rate) return;

	int groupId = m->groupId;

	if (groupId == -1) {    //previously impossible elementTOG, no group assigned
		if (newRate==0) return;
		m->rate = newRate;
		addElement(m);
		return;
	}

	if (newRate == 0) {     //if the elementTOG is impossible, copies the last over it and shrink by one
		//int oldGroup = m->groupId;
		//int oldElement = m->elementID;
		groups[groupId]->deleteElement(m);
		totalN--;
		m->groupId = - 1; //empty so no group
		m->elementID = -1;
		m->rate = 0;

		/*if (m->isEqual(groups[oldGroup].elements[oldElement]) && groups[oldGroup].elements[oldElement]->rate>0){
			std::cout<<"SOMETHING WRONG"<<std::endl;
		}*/

		return;

	}


	if (groups[groupId]->isInside(newRate)) {
		groups[groupId]->updateElement(m,newRate);
		return;
	}

	if (!groups[groupId]->isInside(newRate)) {
		int newGroupId = (int) (floor (log2(newRate / minR) * logBase));
		groups[groupId]->deleteElement(m);
		m->rate = newRate;
		groups[newGroupId]->addElement(m);

		//debug code
		if (!groups[newGroupId]->isInside(m)) {
			std::cout<<"DID NOT MOVE TO CORRECT GROUP"<<std::endl;
		}
		//check(m,newRate);
		return;
	}


}

void SSACRLarge::update() {
	for (int i=0;i<groups.size();i++) {
		tree.updateLeaf(groups[i]->node, groups[i]->totalR);
	}
	//checkGroups();
	//checkGroups();
	tree.updateTree();
	//checkGroups();
}


void SSACRLarge::addElement(elementTOG * m) {
	if (m->rate == 0) {
		m->groupId = -1;
		return;
	}
	int groupId = (int) floor ((log2(m->rate / minR))*logBase);
	groups[groupId]->addElement(m);
	totalN++;
	return;
}



/*bool SSACRLarge::check(elementTOG m, double newRate) {
	if (newRate == 0) return true;

	if (groups[m.groupId].elements[m.elementID]->rate != newRate){
		std::cout<<"SOMETHING WRONG. Should: "<<newRate<<". Is "<<groups[m.groupId].elements[m.elementID]->rate<<std::endl;
		return false;
	}
	return true;
}

bool SSACRLarge::check(elementTOG * m, double newRate) {
    if (newRate == 0) return true;

    if (groups[m->groupId].elements[m->elementID]->rate != newRate){
        std::cout<<"SOMETHING WRONG. Should: "<<newRate<<". Is "<<groups[m->groupId].elements[m->elementID]->rate<<std::endl;
        return false;
    }
    return true;
}
*/
bool SSACRLarge::checkGroups(){
	bool check=true;
	for (int i=0; i<groups.size();i++) {
		if (!groups[i]->checkGroup()){
			std::cout<<"Problem with group "<<i<<std::endl;
			check = false;
		}
	};
	std::vector<double> rrr = tree.getAllRates();
	for (int i=0; i<groups.size();i++) {
		if(!(groups[i]->totalR == rrr[groups[i]->groupId])){
			if ((groups[i]->totalR - rrr[groups[i]->groupId]) / groups[i]->totalR > 1e-6) {
				std::cout<<i<<"\t"<<groups[i]->totalR<<"\t"<<groups[i]->totalR - rrr[groups[i]->groupId]<<std::endl;
				check = false;
			}
		}
	}

	return check;
}

/*std::vector<double> SSACRLarge::getAllRates() {
	std::vector<double> rates(totalN,0);

	for (int g=0; g<groups.size();g++) {
		for (int m = 0; m < groups[g]->totalN; m++) {
			if (rates[groups[g]->elements[m]->position] > 0) {
				std::cout << "Something wrong here!" << std::endl;
			}
			rates[groups[g]->elements[m]->position] = groups[g]->elements[m]->rate;
		}
	}
	return rates;
}*/

void SSACRLarge::printGroups() {
	for (int g=0; g<groups.size();g++){
		std::cout<<"Group "<<g<<": "<<std::endl;
		for (int m=0;m<groups[g]->totalN;m++)
			std::cout << groups[g]->elements[m]->payload << "\t";
		std::cout<<std::endl;
	}

}

elementTOG *SSACRLarge::getElement() {
	double r2 = randExt->operator()(rng);
	CBTNode * extractedNode = tree.sampleLeaf(r2);
	int groupId = extractedNode->payload;
	int iteration=0;
	while (true){
		double r3 = randExt->operator()(rng) * (groups[groupId]->totalN-1);
		int elementId =(int)std::floor(r3);
		double randElement = (r3 - (double)elementId) * groups[groupId]->maxR;
		iteration++;
		if (groups[groupId]->elements[elementId]->rate >= randElement ) {
			return groups[groupId]->elements[elementId];
		}
	}
}

elementTOG* SSACRLarge::addElement(int position, double rate) {
	elementTOG * m= new elementTOG(position, rate);
	addElement(m);
	return m;
}

elementTOG *SSACRLarge::getElement(int & counter, int & randNumbers) {
	double r2 = randExt->operator()(rng);
	randNumbers++;
	CBTNode * extractedNode = tree.sampleLeaf(r2, counter);
	counter++;
	int groupId = extractedNode->payload;
	counter++;
	//int iteration=0;
	counter++;
	while (true){
		double r3 = randExt->operator()(rng) * (groups[groupId]->totalN-1);
		randNumbers++;
		int elementId =(int)std::floor(r3);
		counter++;
		double randElement = (r3 - (double)elementId) * groups[groupId]->maxR;
		counter++;
		//iteration++;
		if (groups[groupId]->elements[elementId]->rate >= randElement ) {
			return groups[groupId]->elements[elementId];
		}
	}
}

void SSACRLarge::updateElement(elementTOG *m, double newRate, int &counter) {
/*if (!checkGroups()) {
		std::cout<<"Something wrong"<<std::endl;
	}*/

	//checkGroups();

	if (newRate == m->rate) return;

	int groupId = m->groupId;
	counter++;

	if (groupId == -1) {    //previously impossible elementTOG, no group assigned
		if (newRate==0) return;
		m->rate = newRate;
		counter++;
		addElement(m,counter);
		counter++;
		totalN++;
		counter++;
		return;
	}

	if (newRate == 0) {     //if the elementTOG is impossible, copies the last over it and shrink by one
		//int oldGroup = m->groupId;
		//int oldElement = m->elementID;
		groups[groupId]->deleteElement(m,counter);
		m->groupId = - 1; //empty so no group
		counter++;
		m->elementID = -1;
		counter++;
		m->rate = 0;
		counter++;
		totalN--;
		counter++;

		/*if (m->isEqual(groups[oldGroup].elements[oldElement]) && groups[oldGroup].elements[oldElement]->rate>0){
			std::cout<<"SOMETHING WRONG"<<std::endl;
		}*/

		return;

	}


	if (groups[groupId]->isInside(newRate)) {
		groups[groupId]->updateElement(m,newRate,counter);
		return;
	}

	if (!groups[groupId]->isInside(newRate)) {
		int newGroupId = (int) (floor (log2(newRate / minR) * logBase));
		counter++;
		groups[groupId]->deleteElement(m,counter);
		m->rate = newRate;
		counter++;
		groups[newGroupId]->addElement(m, counter);

		//debug code
		if (!groups[newGroupId]->isInside(m)) {
			std::cout<<"DID NOT MOVE TO CORRECT GROUP"<<std::endl;
		}
		//check(m,newRate);
		return;
	}


}

void SSACRLarge::update(int &counter) {
	for (int i=0;i<groups.size();i++) {
		tree.updateLeaf(groups[i]->node, groups[i]->totalR, counter);
		counter++;
	}
	//checkGroups();
	//checkGroups();
	tree.updateTree(counter);
	//checkGroups();
}

void SSACRLarge::addElement(elementTOG *m, int &counter) {
	if (m->rate == 0) {
		m->groupId = -1;
		counter++;
		return;
	}
	int groupId = (int) floor ((log2(m->rate / minR))*logBase);
	counter++;
	groups[groupId]->addElement(m, counter);
	counter++;
	totalN++;
	return;
}
