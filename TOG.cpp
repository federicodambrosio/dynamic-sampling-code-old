//
// Created by federicodambrosio on 6-2-17.
//

#include "TOG.h"
#include "RunningStats.hpp"

TOG::TOG(double pmax, double pmin, double _ratio) : maxR(pmax), minR(pmin), k(_ratio), logBase(1. / log2(_ratio)) {
	rng.seed(time(nullptr));
	randExt = new std::uniform_real_distribution<double>(0,1);

	int index = 0;
	double min = minR;
	while (min < maxR) {
		double max = k * min;
		if (max > maxR) max = maxR;
		auto * g = new groupTOG(index, max, min, k);       //new group created
		g->node = tree.addLeaf(index, 0);               //gets its leaf
		groups.push_back(g);                               //added to group list

		min = max;                                         //next group starts where this ends
		index++;
		nGroups++;
	}
}

void TOG::updateElement(elementTOG * m, double newRate) {
	if (newRate == m->rate) return;             //if the rates is unchanged, do nothing

	int groupId = m->groupId;                   //what's its current group

	if (groupId == -1) {                        //previously impossible elementTOG, it had no group
		if (newRate==0) return;                 //still impossible, do nothing
		m->rate = newRate;
		addElement(m);                          //added to the structure with its new rate
		return;
	}

	if (newRate == 0) {                         //previously possible element becomes impossible
		groups[groupId]->deleteElement(m);      //removed from group
		totalN--;
		m->groupId = - 1;                       //no group
		m->elementID = -1;
		m->rate = 0;

		return;
	}

	if (groups[groupId]->isInside(newRate)) {       //rate changed, but it's still in the same group
		groups[groupId]->updateElement(m,newRate);  //updates the element inside the group and that's it
		return;
	}

	if (!groups[groupId]->isInside(newRate)) {      //rate changed, different group now
		int newGroupId = (int) (floor (log2(newRate / minR) * logBase));
		groups[groupId]->deleteElement(m);          //delete from previous group
		m->rate = newRate;
		groups[newGroupId]->addElement(m);          //add to new group

		return;
	}
}

elementTOG *TOG::sampleElement() {
	double r2 = randExt->operator()(rng);
	CBTNode * extractedNode = tree.sampleLeaf(r2);      //samples group from CBT
	int groupId = extractedNode->payload;
	int iteration=0;
	while (true){                                       //Acceptance Rejection on that group
		double r3 = randExt->operator()(rng) * (groups[groupId]->totalN-1);
		int elementId =(int)std::floor(r3);
		double randElement = (r3 - (double)elementId) * groups[groupId]->maxR;
		iteration++;
		if (groups[groupId]->elements[elementId]->rate >= randElement ) {
			return groups[groupId]->elements[elementId];
		}
	}
}

void TOG::update() {            //updates the tree connected to the structure
	for (auto & group : groups) {
		tree.updateLeaf(group->node, group->totalR);
	}
	tree.updateTree();
}


void TOG::addElement(elementTOG * m) {
	if (m->rate == 0) {
		m->groupId = -1;
		return;
	}
	int groupId = (int) floor ((log2(m->rate / minR))*logBase); //which group it's going to end in
	groups[groupId]->addElement(m);
	totalN++;
}

bool TOG::checkGroups(){        //debug
	bool check=true;
	for (int i=0; i<groups.size();i++) {
		if (!groups[i]->checkGroup()){
			std::cout<<"Problem with group "<<i<<std::endl;
			check = false;
		}
	};
	std::vector<double> rrr = tree.getAllRates();
	for (int i=0; i<groups.size();i++) {
		if(groups[i]->totalR != rrr[groups[i]->groupId]){
			if ((groups[i]->totalR - rrr[groups[i]->groupId]) / groups[i]->totalR > 1e-6) {
				std::cout<<i<<"\t"<<groups[i]->totalR<<"\t"<<groups[i]->totalR - rrr[groups[i]->groupId]<<std::endl;
				check = false;
			}
		}
	}

	return check;
}

void TOG::printGroups() {                               //print groups to stdout
	for (int g=0; g<groups.size();g++){
		std::cout<<"Group "<<g<<": "<<std::endl;
		for (int m=0;m<groups[g]->totalN;m++)
			std::cout << groups[g]->elements[m]->payload << "\t";
		std::cout<<std::endl;
	}
}

elementTOG * TOG::sampleElement(RunningStats & it) {          //as above, but stores # of iterations of A-R
	std::uniform_real_distribution<double> random(0,1); //in a RunningStats structure
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
			return groups[groupId]->elements[elementId];
		}
	}

}

elementTOG* TOG::addElement(int position, double rate) {
	auto * m= new elementTOG(position, rate);
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
	for (auto & group : groups) {
		tree.updateLeaf(group->node, group->totalR, counter);
		counter++;
	}
	tree.updateTree(counter);
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
}
