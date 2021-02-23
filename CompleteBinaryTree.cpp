//
// Created by federicodambrosio on 20-12-16.
//

#include "CompleteBinaryTree.h"

CompleteBinaryTree::CompleteBinaryTree() {
	rng.seed(time(nullptr));
	randExt = new std::uniform_real_distribution<double>(0, 1);
	root = nullptr;
}

CompleteBinaryTree::~CompleteBinaryTree() {
	delete root;  //recursively deletes all the nodes
}

void CompleteBinaryTree::addLeaf(CBTNode *node) {
	Nleafs++;
	if (root == nullptr) {             //if there is no root, it creates one
		root = new CBTNode();
		root->parent = nullptr;        //the root has no parents
		root->mass = 0;
		nodes.push_back(root);
	}

	nodes.push_back(node);

	//starting from root, we go down until we find the correct spot where to put the new node
	CBTNode *workingNode = root;

	while (true) {
		if (workingNode->left == nullptr) {    //if there is no left node, new node is right node
			workingNode->left = node;
			node->parent = workingNode;
			touched.push_back(node);
			updateTree();

			return;
		} else if (workingNode->right == nullptr) {    //if there is no right node, new node is right node
			workingNode->right = node;
			node->parent = workingNode;
			touched.push_back(node);
			updateTree();
			return;
		} else if (workingNode->left->leaf) {       //if the left node is a leaf, adds a new internal node and puts both
			//under it
			CBTNode *leftLeaf = workingNode->left;
			workingNode->left = new CBTNode();
			workingNode->left->parent = leftLeaf->parent;
			nodes.push_back(workingNode->left);
			workingNode->left->left = leftLeaf;
			leftLeaf->parent = workingNode->left;
			workingNode->left->right = node;
			node->parent = workingNode->left;
			touched.push_back(node);
			touched.push_back(leftLeaf);
			updateTree();

			return;
		} else if (workingNode->right->leaf) {      //as before, but with right node
			CBTNode *leftLeaf = workingNode->right;
			workingNode->right = new CBTNode();
			workingNode->right->parent = leftLeaf->parent;
			nodes.push_back(workingNode->right);
			workingNode->right->left = leftLeaf;
			leftLeaf->parent = workingNode->right;
			workingNode->right->right = node;
			node->parent = workingNode->right;
			touched.push_back(node);
			touched.push_back(leftLeaf);
			updateTree();

			return;
		} else if (workingNode->left->mass >
		           workingNode->right->mass) {//otherwise selects the direction with less "mass" and
			//repeats cycle
			workingNode = workingNode->right;
		} else {
			workingNode = workingNode->left;
		}

	}

}


CBTNode *CompleteBinaryTree::sampleLeaf(double random) {
	random = random * root->rate;              //random is now between 0 and totalrate
	while (true) {                                      //repeats until it extract a node
		CBTNode *workingNode = root;

		while (!(workingNode->leaf)) {                  //repeat until we get to a leaf
			//if the random rate is < left rate, goes there
			if (random < workingNode->left->rate) workingNode = workingNode->left;
				//otherwise, goes right and subtracts the left rate
			else {
				random = random - workingNode->left->rate;
				workingNode = workingNode->right;
			}
		}
		//once we get a leaf, it is sampled
		return workingNode;
	}
}

void CompleteBinaryTree::updateTree() {
	int toUpdate = touched.size();

	while (toUpdate > 0) {  //if there are still nodes to update

		toUpdate = 0;

		for (int i = 0; i < touched.size(); i++) {
			CBTNode *node = touched[i];
			updateNode(node);                           //update rate upwards
			if (node != root) {                         //if the node is not root, we subs it with parent
				touched[i] = node->parent;              //and count one more node to update
				toUpdate++;
			} else touched[i] = root;

		}
	}
	touched.clear();                                    //empty the touched vector from leftovers
}

void CompleteBinaryTree::updateNode(CBTNode *node) {       //this updates upward
	if (node->leaf) {
		node->mass = 1;     //if it's a leaf, mass is 1
	} else {                //otherwise mass = left.mass+right.mass, rate = left.rate + right.rate
		double rate = 0;
		int mass = 0;
		if (node->left != nullptr) {
			rate += node->left->rate;
			mass += node->left->mass;
		}
		if (node->right != nullptr) {
			rate += node->right->rate;
			mass += node->right->mass;
		}
		node->rate = rate;
		node->mass = mass;
	}
}

CBTNode *CompleteBinaryTree::addLeaf(int pl, double r) {
	auto *node = new CBTNode(pl, r);
	addLeaf(node);
	return node;
}

void CompleteBinaryTree::updateLeaf(CBTNode *leaf, double r) {
	leaf->rate = r;
	touched.push_back(leaf->parent);
}

std::vector<double> CompleteBinaryTree::getAllRates() {
	std::vector<double> rates(Nleafs, 0.);
	for (auto &node : nodes) {
		if (node->leaf) {
			rates[node->payload] = node->rate;
		}
	}

	return rates;
}

void CompleteBinaryTree::updateLeaf(CBTNode *leaf, double r, int &counter) {
	leaf->rate = r;
	counter++;
	touched.push_back(leaf->parent);
	counter++;
}

void CompleteBinaryTree::updateTree(int &counter) {
	int toUpdate = touched.size();       //an arbitrary value just to start
	counter++;

	while (toUpdate > 0) {  //if there are still node to update

		toUpdate = 0;
		counter++;

		for (int i = 0; i < touched.size(); i++) {
			CBTNode *node = touched[i];
			counter++;
			updateNode(node);                           //update rate upwards
			counter++;
			if (node != root) {                         //if the node is not root, we subs with parent
				touched[i] = node->parent;              //and count one more node to update
				counter++;
				toUpdate++;
				counter++;
			} else {
				touched[i] = root;
				counter++;
			}

		}
	}
	touched.clear();
	counter++;
}

CBTNode *CompleteBinaryTree::sampleLeaf(double random, int &counter) {
	random = random * root->rate;              //random is now between 0 and totalrate
	counter++;
	CBTNode *workingNode = root;                    //starts from root
	counter++;

	while (!(workingNode->leaf)) {                  //repeat until we get to a leaf
		//if the random rate is < left rate, goes there
		if (random < workingNode->left->rate) {
			workingNode = workingNode->left;
			counter++;
		}
		//otherwise, goes right and subtracts the left rate
		else {
			random = random - workingNode->left->rate;
			counter++;
			workingNode = workingNode->right;
			counter++;
		}
	}
	//once we get a leaf, it is extracted
	return workingNode;
}

CBTNode *CompleteBinaryTree::sampleLeaf() {
	double random = randExt->operator()(rng);
	random = random * root->rate;              //random is now between 0 and totalrate

	CBTNode *workingNode = root;                    //starts from root

	while (!(workingNode->leaf)) {                  //repeat until we get to a leaf
		//if the random rate is < left rate, goes there
		if (random < workingNode->left->rate) workingNode = workingNode->left;
			//otherwise, goes right and subtracts the left rate
		else {
			random = random - workingNode->left->rate;
			workingNode = workingNode->right;
		}
	}
	//once we get a leaf, if the rate is > 0, it is extracted
	return workingNode;
}

CBTNode *CompleteBinaryTree::sampleLeaf(int &counter) {
	double random = randExt->operator()(rng);
	counter++;
	random = random * root->rate;              //random is now between 0 and totalrate
	counter++;
	CBTNode *workingNode = root;                    //starts from root
	counter++;

	while (!(workingNode->leaf)) {                  //repeat until we get to a leaf
		counter++;
		//if the random rate is < left rate, goes there
		if (random < workingNode->left->rate) {
			counter++;
			workingNode = workingNode->left;
			counter++;
		}
			//otherwise, goes right and subtracts the left rate
		else {
			counter++;
			random = random - workingNode->left->rate;
			counter++;
			workingNode = workingNode->right;
			counter++;

		}
	}
	//once we get a leaf, it is extracted
	return workingNode;
}

void CompleteBinaryTree::print() {
	auto *next = new std::vector<CBTNode *>;
	next->push_back(root);
	int level = 0;
	while (!next->empty()) {
		auto *future = new std::vector<CBTNode *>;
		std::cout << "Level " << level << ": ";
		for (auto & i : *next) {
			if (i->leaf) {
				std::cout << "L\t";
			}
			if (!i->leaf) {
				std::cout << i->mass << "\t";
				future->push_back(i->left);
				future->push_back(i->right);
			}
		}
		std::cout << std::endl;
		level++;
		delete next;
		next = future;
	}
}
