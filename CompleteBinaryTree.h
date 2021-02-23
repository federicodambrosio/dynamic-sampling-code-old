//
// Created by federicodambrosio on 20-12-16.
//

#ifndef CUADATOM_DYNAMICFLATTREE_H
#define CUADATOM_DYNAMICFLATTREE_H

#include <iostream>
#include <random>
#include <vector>

//each node in the tree is a CBTNode object
struct CBTNode {
	int payload;

	double rate;

	bool leaf;          //is this a leaf?

	//tree structure
	CBTNode *parent = nullptr;
	CBTNode *left = nullptr;
	CBTNode *right = nullptr;
	int mass;           //how many leaves under this node

	CBTNode() {         //generates internal node (payload doesn't matter for them)
		rate = 0;
		payload = -1;
		leaf = false;
		mass = 0;
	};

	CBTNode(int pl, double r) { //generates leaf
		rate = r;
		payload = pl;
		leaf = true;
		mass = 1;
	}

	~CBTNode() {                //recursively deletes the tree
		delete left;
		delete right;
	}
};


class CompleteBinaryTree {          //Section 3.1
	std::vector<CBTNode*>  nodes;   //here the pointers to all of the nodes are stored for debug and easy access
									//but not normally accessed
	CBTNode *root;                  //the root of the tree
	std::vector<CBTNode*> touched; //nodes "touched" since the last tree update

	static static void updateNode(CBTNode *node); //private method for update a node

	int Nleafs=0;                   //number of leafs

	std::mt19937 rng;
	std::uniform_real_distribution<double> * randExt;

public:
	CompleteBinaryTree();

	~CompleteBinaryTree();

	CBTNode *addLeaf(int pl, double r);  //adds a new leaf, with given payload and rate

	void addLeaf(CBTNode *node);        //adds a previously created leaf

	void updateLeaf(CBTNode *leaf, double r);   //update a leaf, requires updateTree

	void updateLeaf(CBTNode *leaf, double r, int & counter);   //adds to counter # of operations required

	CBTNode *sampleLeaf(double random);                    //samples with a given random number (b.w. 0 and 1)

	CBTNode *sampleLeaf(double random, int & counter);    //same but updates counter w/ # of operations required

	CBTNode *sampleLeaf();                                //samples leaf

	CBTNode *sampleLeaf(int & counter);                   //same but updates counter w/ # of operations required

	void updateTree();                  //update the whole tree. has to be invoked after updating leafs before extracting

	void updateTree(int & counter);     //same but updates counter w/ # of operations required

	double getRate() { return root->rate; };   //outputs sum of all rates, which is, by definition, the rate of the root

	std::vector<double> getAllRates();         //outputs all rates

	int getMass() { return root->mass;}         //outputs all mass

	void print();                               //prints the tree to stdout
};

#endif //CUADATOM_DYNAMICFLATTREE_H
