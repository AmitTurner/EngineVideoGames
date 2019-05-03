#include "intersect.h"
#include <stdlib.h>
#include <list>


//default constructor
intersect::intersect(std::vector<glm::vec3> shape) {
	boundbox = findthightbox(shape);
	boundboxvec = bound_vec_to_boundbox(boundbox);
	kd.makeTree(shape);
}

//default constructor
IndexedModel intersect::getBoundingBox() {
	return boxVertexesToIndexModel(boundboxvec);
}

glm::mat4 *currentTransMe;
glm::mat4 *currentTransOther;

/*
*   Return the tree root node
*/
std::vector<IndexedModel> intersect::isIntersect(glm::mat4 *transMe, glm::mat4 *transOther, intersect other) {

	std::vector<glm::vec3> boundboxvec1 = bound_vec_to_boundbox(boundbox);
	std::vector<glm::vec3> boundboxvec2 = bound_vec_to_boundbox(other.boundbox);

	currentTransMe    = transMe;
	currentTransOther = transOther;

	int res = isThereSeparatingPanel(boundboxvec1, boundboxvec2);
	//printf("res %d\n", res);
	if (res > 0)
		return std::vector<IndexedModel>();

	std::vector<std::vector<glm::vec3>> intersect_boxes;
	rec_is_intersect(kd.getRoot(), other.kd.getRoot(), &boundbox, &other.boundbox, 0, &intersect_boxes);
	//intersect_boxes.pop_back(); intersect_boxes.pop_back();
	return makeBoxesIndexModels(intersect_boxes);
}

std::vector<float> intersect::findthightbox(std::vector<glm::vec3> positions) {
	std::vector<float> critical_point;
	critical_point.push_back(positions[0].x);	critical_point.push_back(positions[0].x);
	critical_point.push_back(positions[0].y);	critical_point.push_back(positions[0].y);
	critical_point.push_back(positions[0].z);	critical_point.push_back(positions[0].z);

	for (unsigned int i = 1; i < positions.size(); i++) {
		if (critical_point[0] > positions[i].x)
			critical_point[0] = positions[i].x;
		if (critical_point[1] < positions[i].x)
			critical_point[1] = positions[i].x;
		if (critical_point[2] > positions[i].y)
			critical_point[2] = positions[i].y;
		if (critical_point[3] < positions[i].y)
			critical_point[3] = positions[i].y;
		if (critical_point[4] > positions[i].z)
			critical_point[4] = positions[i].z;
		if (critical_point[5] < positions[i].z)
			critical_point[5] = positions[i].z;
	}

	return critical_point;
}

int intersect::isThereSeparatingPanel(std::vector<glm::vec3> box1, std::vector<glm::vec3> box2) {
	glm::vec3 frontleftdown = v4to3(*currentTransMe * v3to4(box1[0]));
	glm::vec3 backleftdown  = v4to3(*currentTransMe * v3to4(box1[1]));
	glm::vec3 backrightdown = v4to3(*currentTransMe * v3to4(box1[2]));
	glm::vec3 backleftup    = v4to3(*currentTransMe * v3to4(box1[5]));
	glm::vec3 frontupright  = v4to3(*currentTransMe * v3to4(box1[7]));
	glm::vec3 PA = (backleftdown + frontupright) / 2.0f;//coordinate position of the center of A
	glm::vec3 Ax = backrightdown - backleftdown;//unit vector representing the x - axis of A
	glm::vec3 Ay = backleftup    - backleftdown;// unit vector representing the y - axis of A
	glm::vec3 Az = frontleftdown - backleftdown;// unit vector representing the z - axis of A
	float WA = glm::length(Ax);// half width of A(corresponds with the local x - axis of A)
	Ax = Ax / WA;//normalize
	WA /= 2.0f;
	float HA = glm::length(Ay);//half height of A(corresponds with the local y - axis of A)
	Ay = Ay / HA;
	HA /= 2.0f;
	float DA = glm::length(Az);//half depth of A(corresponds with the local z - axis of A)
	Az = Az / DA;
	DA /= 2.0f;

	frontleftdown= v4to3(*currentTransOther * v3to4(box2[0]));
	backleftdown = v4to3(*currentTransOther * v3to4(box2[1]));
	backrightdown= v4to3(*currentTransOther * v3to4(box2[2]));
	backleftup   = v4to3(*currentTransOther * v3to4(box2[5]));
	frontupright = v4to3(*currentTransOther * v3to4(box2[7]));
	glm::vec3 PB = (backleftdown + frontupright) / 2.0f;//coordinate position of the center of B
	glm::vec3 Bx = backrightdown - backleftdown;//unit vector representing the x - axis of B
	glm::vec3 By = backleftup - backleftdown;//unit vector representing the y - axis of B
	glm::vec3 Bz = frontleftdown - backleftdown;//unit vector representing the z - axis of B
	float WB = glm::length(Bx);// half width of B(corresponds with the local x - axis of B)
	Bx = Bx / WB;//normalize
	WB /= 2.0f;
	float HB = glm::length(By);// half height of B(corresponds with the local y - axis of B)
	By = By / HB;
	HB /= 2.0f;
	float DB = glm::length(Bz);// half depth of B(corresponds with the local z - axis of B)
	Bz = Bz / DB;
	DB /= 2.0f;
	glm::vec3 T = PB - PA;
	float cond, res;
	// L = Ax
	cond = abs(glm::dot(T, Ax));
	res = WA + WB * abs(glm::dot(Ax, Bx)) + HB * abs(glm::dot(Ax, By)) + DB * abs(glm::dot(Ax, Bz));
	if (cond > res)
		return 1;
	// L = Ay
	cond = abs(glm::dot(T, Ay));
	res = HA + WB * abs(glm::dot(Ay, Bx)) + HB * abs(glm::dot(Ay, By)) + DB * abs(glm::dot(Ay, Bz));
	if (cond > res)
		return 2;
	// L = Az
	cond = abs(glm::dot(T, Az));
	res = DA + WB * abs(glm::dot(Az, Bx)) + HB * abs(glm::dot(Az, By)) + DB * abs(glm::dot(Az, Bz));
	if (cond > res)
		return 3;
	// L = Bx
	cond = abs(glm::dot(T, Bx));
	res = WB + WA * abs(glm::dot(Ax, Bx)) + HA * abs(glm::dot(Ay, Bx)) + DA * abs(glm::dot(Az, Bx));
	if (cond > res)
		return 4;
	// L = By
	cond = abs(glm::dot(T, Ay));
	res = HB + WA * abs(glm::dot(Ax, By)) + HA * abs(glm::dot(Ay, By)) + DA * abs(glm::dot(Az, By));
	if (cond > res)
		return 5;
	// L = Bz
	cond = abs(glm::dot(T, Az));
	res = DB + WA * abs(glm::dot(Ax, Bz)) + HA * abs(glm::dot(Ay, Bz)) + DA * abs(glm::dot(Az, Bz));
	if (cond > res)
		return 6;
	// L = Ax x Bx
	glm::vec3 AxCrsBx = glm::cross(Ax, Bx);
	cond = abs(glm::dot(T, AxCrsBx));
	res  = WA * abs(glm::dot(Ax, AxCrsBx)) + HA * abs(glm::dot(Ay, AxCrsBx)) + DA * abs(glm::dot(Az, AxCrsBx)) +
		   WB * abs(glm::dot(Bx, AxCrsBx)) + HB * abs(glm::dot(By, AxCrsBx)) + DB * abs(glm::dot(Bz, AxCrsBx));
	if (cond > res)
		return 7;
	return 0;
}

//std::vector<glm::vec3> floats_to_vec(std::vector<float> boundbox) {
//	std::vector<glm::vec3> vec;
//	vec.push_back(vec3(boundbox[0], 0, 0));
//	vec.push_back(vec3(boundbox[1], 0, 0));
//	vec.push_back(vec3(0, boundbox[2], 0));
//	vec.push_back(vec3(0, boundbox[3], 0));
//	vec.push_back(vec3(0, 0, boundbox[4]));
//	vec.push_back(vec3(0, 0, boundbox[5]));
//	return vec;
//}

std::vector<glm::vec3> intersect::bound_vec_to_boundbox(std::vector<float> boundbox) {
	std::vector<glm::vec3> vec;
	vec.push_back(vec3(boundbox[0], boundbox[2], boundbox[5]));
	vec.push_back(vec3(boundbox[0], boundbox[2], boundbox[4]));
	vec.push_back(vec3(boundbox[1], boundbox[2], boundbox[4]));
	vec.push_back(vec3(boundbox[1], boundbox[2], boundbox[5]));
	vec.push_back(vec3(boundbox[0], boundbox[3], boundbox[5]));
	vec.push_back(vec3(boundbox[0], boundbox[3], boundbox[4]));
	vec.push_back(vec3(boundbox[1], boundbox[3], boundbox[4]));
	vec.push_back(vec3(boundbox[1], boundbox[3], boundbox[5]));
	return vec;
}

void intersect::merge(std::vector<std::vector<glm::vec3>> *a, std::vector<std::vector<glm::vec3>> *b) {
	for (unsigned int i = 0; i < b->size(); i++) 
		a->push_back((*b)[i]);
}

bool intersect::isEqual(std::vector<glm::vec3> &boxvec1, std::vector<glm::vec3> &boxvec2) {
	if (boxvec1.size() > boxvec2.size())
		return false;
	bool out = 1;
	for (unsigned int i = 0; i < boxvec1.size(); i++)
		out &= boxvec1[i] == boxvec2[i];
	return out;
}

void intersect::insert_box(std::vector<std::vector<glm::vec3>> *boxes, std::vector<glm::vec3> boxvec, glm::mat4 *transmat) {
	for (std::vector<glm::vec3> box : *boxes)
		if(intersect::isEqual(box, boxvec)) return;

	for (unsigned int i = 0; i < boxvec.size(); i++)
		boxvec[i] = v4to3(*transmat * v3to4(boxvec[i]));
	boxes->push_back(boxvec);
}

int intersect::intersectWithOther(Node * nextother, std::vector<float> &boundingboxcopyother, int axis, std::vector<float> &boundingboxcopy)
{
	if (nextother) {
		int res = isThereSeparatingPanel(bound_vec_to_boundbox(boundingboxcopy), bound_vec_to_boundbox(boundingboxcopyother));
		if (res == 0) {
			if (boundingboxcopyother[axis * 2] == boundingboxcopyother[axis * 2 + 1])
				return 64;//we do not want to keep recursion on a plane
		}
		return res;
	}
	return 65;
}

void intersect::nodesIntersectValitate(Node * next, std::vector<float> &boundingboxcopy, int axis, Node * other, std::vector<float> &boundingboxcopyother,
	int depth, std::vector<std::vector<glm::vec3>> * output, std::vector<glm::vec3> &boxvec, std::vector<glm::vec3> &boxvec2)
{
	bool intersect_with = 0;// 1;
	if (next) {
		if (boundingboxcopy[axis * 2] == boundingboxcopy[axis * 2 + 1])
			intersect_with = 1;//we do not want to keep recursion on a plane	
		else {
			boundingboxcopyother[axis * 2    ] = other->min;
			boundingboxcopyother[axis * 2 + 1] = other->data[axis];
			int res1 = intersectWithOther(other->left, boundingboxcopyother, axis, boundingboxcopy);
			if (res1 == 0)
				rec_is_intersect(next, other->left, &boundingboxcopy, &boundingboxcopyother, depth + 1, output);
			boundingboxcopyother[axis * 2    ] = other->data[axis];
			boundingboxcopyother[axis * 2 + 1] = other->max;
			int res2 = intersectWithOther(other->right, boundingboxcopyother, axis, boundingboxcopy);
			if (res2 == 0)
				rec_is_intersect(next, other->right, &boundingboxcopy, &boundingboxcopyother, depth + 1, output);
			intersect_with = res1 & res2;
		}
	}
	if (intersect_with && ((unsigned)depth == kd.max_depth-1)) {//none of the children are intersecting - add perants
		insert_box(output, boxvec, currentTransMe);
		insert_box(output, boxvec2, currentTransOther);
	}
}

//in iteration level the boxes are intersecting we will assure it happen in the children too
void intersect::rec_is_intersect(Node *current, Node *other,
			std::vector<float> *boundingbox, std::vector<float> *boundingboxother,
			int depth, std::vector<std::vector<glm::vec3>> *output) {
	int axis = depth % 3;
	std::vector<glm::vec3> boxvec  = bound_vec_to_boundbox(*boundingbox);
	std::vector<glm::vec3> boxvec2 = bound_vec_to_boundbox(*boundingboxother);
	std::vector<float> boundingboxcopy      = *boundingbox;
	std::vector<float> boundingboxcopyother = *boundingboxother;

	boundingboxcopy[axis * 2    ] = current->min;
	boundingboxcopy[axis * 2 + 1] = current->data[axis];
	nodesIntersectValitate(current->left,  boundingboxcopy, axis, other, boundingboxcopyother, depth, output, boxvec, boxvec2);

	boundingboxcopy[axis * 2    ] = current->data[axis];
	boundingboxcopy[axis * 2 + 1] = current->max;
	nodesIntersectValitate(current->right, boundingboxcopy, axis, other, boundingboxcopyother, depth, output, boxvec, boxvec2);
}

IndexedModel intersect::boxVertexesToIndexModel (std::vector<glm::vec3> intesect_box){
	Vertex vertices[] =
	{
		Vertex(intesect_box[0], glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)),
		Vertex(intesect_box[1], glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)),
		Vertex(intesect_box[2], glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)),
		Vertex(intesect_box[3], glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)),
		Vertex(intesect_box[4], glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)),
		Vertex(intesect_box[5], glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)),
		Vertex(intesect_box[6], glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0)),
		Vertex(intesect_box[7], glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 0))
	};
	
	/*for (unsigned int i = 0; i<intesect_box.size(); i++) {
		if (i % 2 == 0) printf("\n");
		printf("%f %f %f\t\t", intesect_box[i][0], intesect_box[i][1], intesect_box[i][2]);
	}*/

	unsigned int indices[] = { 
		0, 1, 2,
		0, 3, 2,

		4, 5, 6,
		4, 7, 6,

		0, 4, 5,
		0, 1, 5,

		1, 2, 6,
		1, 5, 6,

		2, 3, 7,
		2, 6, 7,

		3, 0, 4,
		3, 7, 4,
	};

	IndexedModel model;

	for (unsigned int i = 0; i < 8; i++)
	{
		model.positions.push_back(*vertices[i].GetPos());
		model.colors.push_back(*vertices[i].GetColor());
		model.normals.push_back(*vertices[i].GetNormal());
		model.texCoords.push_back(*vertices[i].GetTexCoord());
	}
	for (unsigned int i = 0; i < 6*2*3; i++)
		model.indices.push_back(indices[i]);

	return model;
}

std::vector<IndexedModel> intersect::makeBoxesIndexModels(std::vector<std::vector<glm::vec3>> intersect_boxes) {
	std::vector<IndexedModel> models;
	for (std::vector<glm::vec3> intesect_box : intersect_boxes) {
		models.push_back(boxVertexesToIndexModel(intesect_box));
	}
	return models;
}