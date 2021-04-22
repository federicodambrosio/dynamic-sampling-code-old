//
// The class CompleteBinaryTree implements the Complete Binary Tree method from the paper.
// It is referenced by the TOG (Tree of Groups), TwoLevelARB, CascadeOfGroups classes
//

#ifndef ACCEPTANCE_REJECTION
#define ACCEPTANCE_REJECTION

#include <random>

struct elementAR {
	unsigned long payload;                //unique event id
	unsigned long position;               //position in the event vector, useful for updates
	double rate;                //rate of the event


	elementAR() : position(-1), payload(-1), rate(0){}
	elementAR(int pl, int ps, double r) : payload(pl), position(ps), rate(r){}
	elementAR(int pl, int ps, double r, int & counter) : payload(pl), position(ps), rate(r){counter+=3;}
};

class AcceptanceRejection {                                
	std::mt19937 rng;
	double maxR=0;                                          //largest rate encountered by the data structure
	std::vector<elementAR*> items;                          //vector of events
	std::uniform_real_distribution<double> * randExt;

public:
	double currentRate = 0;                                 //current total rate in AR

	//init random number generation methods
	AcceptanceRejection(){
		rng.seed(time(nullptr));
		randExt = new std::uniform_real_distribution<double>(0,1);
	};
	//cleans up the events before deleting the structure
	~AcceptanceRejection(){
		for (auto & item : items) delete item;
	}
	//adds an event to the data structure
	elementAR* addElement(double rate);                        //payload = order of creation
	elementAR* addElement(elementAR * it);                     //adds a pre-created event

	elementAR * sampleElement();                                  //returns an event according to rate
															//randNumbers by the number of random numbers generated
	void updateElement(elementAR* i, double newRate);                  //updates the rate of an event

	void deleteElement(elementAR* i);                          //does not delete the object

};


#endif //ACCEPTANCE_REJECTION
