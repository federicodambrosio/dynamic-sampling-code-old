//
// Created by federicodambrosio on 20-12-16.
//

#include "CompleteBinaryTree.h"

CompleteBinaryTree::CompleteBinaryTree() {
	rng.seed(time(nullptr));
	root = nullptr;
}

CompleteBinaryTree::~CompleteBinaryTree() {
	delete root;  //recursively deletes all the nodes
}

void CompleteBinaryTree::addLeaf(CBTNode *node) {
	if (!node->leaf){
		std::cout << "Err. Not a leaf."<<std::endl;
		return;
	}
	Nleafs++;
	nodes.push_back(node);

	//if there is no root, it places the node as root
	if (root == nullptr){
		root = node;                //no update necessary
		return;
	}

	CBTNode * current = root;
	bool lastMoveLeft;              //is current a left or right child?

	while (!current->leaf){
		//if there is more mass on the left, go right
		if (current->left->mass > current->right->mass) {
			current = current->right;
			lastMoveLeft = false;
		}
		else {
			current = current->left;
			lastMoveLeft = true;
		}
	}

	//the leaf is replaced with an internal node with children current and node
	CBTNode * internal = new CBTNode();
	//replaces internal as child of the parent of current (root has no parents)
	if (current != root){
		internal->parent = current->parent;
		if (lastMoveLeft) internal->parent->left = internal;
		else internal->parent->right = internal;
	} else {
		//if the internal node replaces root, becomes root
		root = internal;
	}
	//sets the internal node as parent of current and node
	current->parent = internal;
	node->parent = internal;
	internal->left = current;
	internal->right = node;

	//internal and node have common parent, we only need to update from their parent (update does not affect leaves)
	touched.push(internal);
	//update performed immediately
	updateTree();
}


void CompleteBinaryTree::updateTree() {
	while (!touched.empty()){
		auto currNode = touched.front();
		updateNode(currNode);
		touched.pop();
		if (currNode != root){              //root has no parent
			touched.push(currNode->parent);
		}
	}
}

void CompleteBinaryTree::updateNode(CBTNode *node) {
	//only internal nodes require update for consistency
	if (!node->leaf){
		node->rate = node->right->rate + node->left->rate;
		node->mass = node->right->mass + node->left->mass;
	}
}

CBTNode *CompleteBinaryTree::addLeaf(unsigned long pl, double r) {
	auto *node = new CBTNode(pl, r);
	addLeaf(node);
	return node;
}

void CompleteBinaryTree::updateLeaf(CBTNode *leaf, double r) {
	leaf->rate = r;
	touched.push(leaf->parent);
}

CBTNode *CompleteBinaryTree::sampleLeaf() {
	double random = randExt(rng) * root->rate;  //random number between 0 and total rate
	unsigned long depth = 0;

	CBTNode *workingNode = root;                    //starts from root

	while (!(workingNode->leaf)) {                  //repeat until we get to a leaf
		//if the random rate is < left rate, goes there
		if (random < workingNode->left->rate) workingNode = workingNode->left;
			//otherwise, goes right and subtracts the left rate
		else {
			random = random - workingNode->left->rate;
			workingNode = workingNode->right;
		}
		depth++;
	}
	//once we get a leaf, it is sampled	
	return workingNode;
}


