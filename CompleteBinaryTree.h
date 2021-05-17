//
// The class CompleteBinaryTree implements the Complete Binary Tree method from the paper.
// It is referenced by the TOG (Tree of Groups) class
//

#ifndef COMPLETE_BINARY_TREE_H
#define COMPLETE_BINARY_TREE_H

#include <iostream>
#include <random>
#include <vector>
#include <queue>

//each node in the tree is a CBTNode object
struct CBTNode {
	long payload;

	double rate;

	bool leaf;          //true if leaf, false if internal node

	//tree structure
	CBTNode *parent = nullptr;
	CBTNode *left = nullptr;
	CBTNode *right = nullptr;
	unsigned long mass;           //how many leaves under this node

	//generates internal node (payload doesn't matter for them)
	CBTNode() {         
		rate = 0;
		payload = -1;
		leaf = false;
		mass = 0;
	};
	
	//generates leaf (needs a payload and a rate)
	CBTNode(unsigned long pl, double r) { 
		rate = r;
		payload = pl;
		leaf = true;
		mass = 1;
	}
	
	//deletes the node and its children (no orphans)
	~CBTNode() {                
		delete left;
		delete right;
	}
};


class CompleteBinaryTree {
	//all nodes are stored in this vector for debug but not normally accessed from it          
	std::vector<CBTNode*>  nodes;   
									
	CBTNode *root;                  //the root of the tree
	
	//nodes that have been edited since last updateTree 
	std::queue<CBTNode*> touched; 

	//updates mass and rate of a node for internal consistency
	static void updateNode(CBTNode *node);

	unsigned long Nleafs=0;                   //number of leafs (== events)

	//random number generator + uniform real distribution between 0 and 1
	std::mt19937 rng;			
	std::uniform_real_distribution<double> randExt;
	
	
public:	
	//creates empty tree with only root
	CompleteBinaryTree();
	
	//deletes the tree and all of its nodes
	~CompleteBinaryTree();
	
	//adds a new leaf, with given payload and rate. 
	CBTNode *addLeaf(unsigned long pl, double r);  

	//adds a CBTNode as a leaf. Requires an updateTree.
	void addLeaf(CBTNode *node);        
	
	//updates the rate of a leaf to r. Requires an updateTree.
	void updateLeaf(CBTNode *leaf, double r);   
	
	//samples a leaf according to its rate 
	CBTNode *sampleLeaf();                                                   

	//updates mass and rate from the "touched" nodes to the root
	void updateTree();                 

	//returns sum of all rates, which is, by definition, the rate of the root
	double getRate() { return root->rate; };   
	
	//returns a vector of the rates of the leaves
	std::vector<double> getAllRates();                
	
	//returns number of nodes in tree
	unsigned long numberNodes(){return nodes.size();}
	
	//returns number of events in structure
	unsigned long N(){return Nleafs;}
};

#endif //COMPLETE_BINARY_TREE_H
