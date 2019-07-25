#include "snakeMoveTracker.H"

snakeMoveTracker::snakeMoveTracker(int size) {
	nodesLen = size-2;
	for (int i = 0; i < size-1; i++)
		firstVec.push_back(nullptr);
}

listNode<motionTracker> *getLast(listNode<motionTracker> *node) {
	listNode<motionTracker> *prev = node;
	while (node) {
		prev = node;
		node = node->next;
	}
	return prev;
}

inline listNode<motionTracker> *genNode(int ticks, float angleTurn) {
	listNode<motionTracker> *mTnode = new listNode<motionTracker>();
	mTnode->value.ticks = ticks;
	mTnode->value.angleTurn = angleTurn;
	mTnode->next = nullptr;
	return mTnode;
}

//primitive in bound check
bool inBound(float x1, float y1, float z1, float x2, float y2, float z2) {
	float diff = abs(x2 - x1);
	diff += abs(y2 - y1);
	diff += abs(z2 - z1);
	return diff < mTepsilon ? true : false;
}

void snakeMoveTracker::add(int Node, listNode<motionTracker> *newNode) {
	listNode<motionTracker> *mTnode = firstVec[Node];
	newNode->next = nullptr;
	if (mTnode) {
		listNode<motionTracker> *last = getLast(mTnode);
		last->next = newNode;
		newNode->value.ticks = 0;
	}
	else {
		firstVec[Node] = newNode;
		newNode->value.ticks = baseTicks;
	}
}

void snakeMoveTracker::add(float angleTurn) {
	add(nodesLen, genNode(baseTicks, angleTurn));
}

void snakeMoveTracker::printDS() {
	for (int i = 0; i < (signed)firstVec.size(); i++) {
		listNode<motionTracker> *node = firstVec[i];
		printf("%d\n", i);
		while (node) {
			printf("%d %f\n", node->value.ticks, node->value.angleTurn);
			node = node->next;
		}
	}
}

// assume it has pointer to valid 
void snakeMoveTracker::transferToNextNode(int Node) {
	listNode<motionTracker> *mTnode = firstVec[Node];
	firstVec[Node] = mTnode->next;
	if (Node > 0)
		add(Node - 1, mTnode);
	else 
		delete mTnode;	
}

float snakeMoveTracker::getSumOfAllAngles(int Node) {
	listNode<motionTracker> *mTnode = firstVec[Node];
	float outangle = 0;
	while (mTnode) {
		motionTracker *mTval = (motionTracker *)mTnode;
		mTnode = mTnode->next;
		outangle += mTval->angleTurn;
		transferToNextNode(Node);		
	}

	return outangle;
}

float snakeMoveTracker::getAngle(int Node) {
	listNode<motionTracker> *mTnode = firstVec[Node];
	float outangle = 0;
	if (mTnode) {
		motionTracker *mTval = (motionTracker *) mTnode;

		if (mTval->ticks-- <= 0) {
			outangle = mTval->angleTurn;
			transferToNextNode(Node);
		}
	}

	return outangle;
}

snakeMoveTracker::~snakeMoveTracker(void) {
	for (int i = 0; i < (signed)firstVec.size(); i++) {
		listNode<motionTracker> *next;
		listNode<motionTracker> *node = firstVec[i];
		while (node) {
			next = node->next;
			delete node;
			node = next;
		}
	}
	firstVec.clear();	firstVec.~vector();
}

// todo check ds
// whenever user not pressing you should move the head back
// with the same func of user moving!