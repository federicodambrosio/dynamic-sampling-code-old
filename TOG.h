//
// This implements the Tree of Groups method from the paper.
//

#ifndef TREE_OF_GROUPS_H
#define TREE_OF_GROUPS_H

#include "CompleteBinaryTree.h"
#include <random>
#include <vector>
#include <ctime>
#include "RunningStats.hpp"

struct elementTOG { //a single event in the Tree of Groups datastructure
	int payload=0;
	double rate;

	int elementID;      //id of the element inside a group
	int groupId;        //id of the group

	//creates an "empty" elementTOG with out-of-range indexes (that we can look for)
	elementTOG() : elementID(-1), groupId(-1), payload(0), rate(0.){}

	//creates a elementTOG, given all the parameters
	explicit elementTOG(int _position, double _rate = 0, int _elementId=0, int _groupId=-1){
		rate=_rate;
		elementID=_elementId;
		groupId=_groupId;
		payload = _position;
	}
};

struct groupTOG {
	const int groupId;  //the id of the group
	const double maxR;  //max rate of a elementTOG in the group
	const double minR;  //min rate of a elementTOG in the group
	const double k; // = maxR/minR
	double totalR = 0;  //sum of the rates of all the elements inside the group
	std::vector<elementTOG*> elements;    //array of elements
	int totalN = 0;     //number of elements inside the group
	CBTNode *node = nullptr;   //node in the Complete Binary Tree for this group

	groupTOG(int i, double max, double min, double _k) : groupId(i), maxR(max), minR(min), k(_k){}

	~groupTOG() {
		for (auto & element : elements) delete element;
	}

	void addElement(elementTOG* m) {
		//update total rate of the group
		totalR = totalR + m->rate;

		//update Ids in the elementTOG
		m->elementID = totalN;
		m->groupId = groupId;

		//add the elementTOG to data structure and update total N
		elements.push_back(m);
		totalN++;
	}

	void deleteElement(elementTOG* m) {
		//if it was the only elementTOG in the structure
		if (totalN == 1 && m->elementID == 0){
			totalN=0;
			totalR=0;
			elements.pop_back();
			return;
		}

		int elementId = m->elementID; //get the Id

		//update rates
		totalR = totalR - m->rate;

		//delete elementTOG and subs in its place the last in the structure
		m->rate = 0;
		if (elementId != totalN - 1) {
			elements[elementId] = elements[totalN-1];
			elements[elementId]->elementID = elementId;
		}

		elements.pop_back();

		totalN--;   //update N
	}


	void updateElement(elementTOG *m, double r_) {
		totalR = totalR + r_ - m->rate;   //update total rate
		m->rate = r_;                     //update rate of the elementTOG
	}

	//debug function
	bool checkGroup(){
		double sum=0;
		double err_ratio = 0;
		for (int i=0;i<totalN;i++) {
			if (!isInside(elements[i])) {
				std::cout<<"MOVE OUT OF RANGE"<<std::endl;
			}
			sum+=elements[i]->rate;
		}
		if (sum!=totalR){
			err_ratio =(totalR-sum)/sum;
			if (std::abs(err_ratio)>0.001) {
				std::cout<<err_ratio<<std::endl;
			}
			totalR = sum;
		}
		return (std::abs(err_ratio)<0.001);
	}

	//checks if a rate is inside this group
	bool isInside (double rate){
		if (rate > maxR or rate < minR) {
			return false;
		}
		else return true;
	}

	//check if a elementTOG is inside this group
	bool isInside(elementTOG *m){
		if (m->rate > maxR or m->rate < minR) {
			return false;
		}
		else return true;
	}

};

class TOG {                                 //Tree of Groups, Section 3.4
	std::vector<groupTOG*> groups;
	const double maxR;                      //maximum possible rate in the whole structure
	const double minR;                      //minimum possible rate " " "
	int nGroups=0;                          //how many groups
	const double k;                         //grouping constant, i.e. group i is form rmax/k^(i-1) to rmax/k^i
	const double logBase;                         // 1/log2(k)
	int totalN=0;                           //how many elements

	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;
	CompleteBinaryTree tree;                                            //CBT to pick a group
public:


	TOG(double pmax, double pmin, double _k =2 );

	~TOG(){
		for (auto & group : groups) delete group;
	}

	elementTOG * sampleElement();

	void updateElement(elementTOG * m, double newRate);

	void update();                                                  //if groups have changed tot. rates, update tree

	void addElement(elementTOG * m);

	elementTOG * addElement(int position, double rate);             //same but generates the element and returns * to it
	
	int NGroup() const{return nGroups;}

	double totalRate(){
		checkGroups();
		return tree.getRate();}
		
	bool checkGroups();

	void printGroups();
};


#endif //TREE_OF_GROUPS_H
