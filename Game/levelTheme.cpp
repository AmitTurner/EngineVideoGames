
#include "levelTheme.h"
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <random>

float scale1[] = { 0.004f, 0.0007f };
float scale2[] = { 0.004f, 0.001f };
float scale3[] = { 0.003f, 0.002f };
float scale4[] = { 0.003f, 0.004f };
float scale5[] = { 0.008f, 0.002f };
float *scales[] = { scale1, scale2, scale3, scale4, scale5 };
float ThemeHolder::getScale(int type) {
	return scales[current][type];
}

static const char *TexImg[] = { "floor.jpg", "wall.jpg", "tree.png", "fruit.jpg", "snake.jpg" };
static const int TexImgSize = sizeof(TexImg) / sizeof(char*);
static const char *sharedObjNames[] = { "../res/objs/cave.obj", "../res/objs/snake_female.obj" };
static const int sharedSize = sizeof(sharedObjNames) / sizeof(char*);
static const char *objNames[] = { "tree.obj", "fruit.obj" };
static const char *path = "../res/objs-ordered/levels/%s/%s";
static const int subNameSize = 256;
struct theme *ThemeHolder::genTheme(char *name) {
	struct theme *out = new theme();
	out->themeName = (char *) malloc(subNameSize);
	strcpy_s(out->themeName, subNameSize, name);

	out->filepath = (char **)calloc(sizeof(void *), size);

	int i = 0;
	for (; i < sharedSize; i++) {
		out->filepath[i] = (char *)malloc(subNameSize);
		strcpy_s(out->filepath[i], subNameSize, sharedObjNames[i]);
	}
	for (;  i < size; i++) {
		out->filepath[i] = (char *) malloc(subNameSize);
		sprintf_s(out->filepath[i], subNameSize, path, name, objNames[i - sharedSize]);
	}

	//out->wallTex		 = -1;
	//out->floorTex		 = -1;
	out->uploadedObj	 = (void **) calloc(sizeof(void *), size);
	out->computedKDtrees = (void **) calloc(sizeof(void *), size);
	return out;
}

char *themeFilePath = "../res/objs-ordered/levels/themeList";
static const int maxWidth = 1024;
char buf[maxWidth];
int ThemeHolder::readInitFile() {
	std::ifstream myfile(themeFilePath);
	if (myfile.is_open())
	{
		while (myfile.getline(buf, maxWidth, '\n'))
			if (buf[0] != 0) {
				printf("%s\n", buf);
				struct theme *t = genTheme(buf);
				themes.push_back(t);
			}

		myfile.close();
		return 0;
	}
	printf("Unable to open file <%s>\n", themeFilePath);
	return -1;
}

void ThemeHolder::loadTex()
{
	if (sharedSize < (signed)themes[current]->texNo.size())
		return;
	int texId = scn->textures.size();
	//for (int i = 0; i < (signed)themes.size(); i++)
	for (int j = 0; j < TexImgSize; j++) {
		sprintf_s(buf, maxWidth, path, themes[current]->themeName, TexImg[j]);
		scn->AddTexture(buf);
		themes[current]->texNo.push_back(texId++);
	}	
}

ThemeHolder::ThemeHolder(Scene *scn, int size, int firstTheme) {
	current = firstTheme;
	this->scn = scn;
	this->size = size;
	readInitFile();
	loadTex();

	printf("%d themes loaded\n", themes.size());
}

void ThemeHolder::swapThemes(int next) {
	for (int i = 0; i < sharedSize; i++) {
		themes[next]->uploadedObj[i]	 = themes[current]->uploadedObj[i];
		themes[next]->computedKDtrees[i] = themes[current]->computedKDtrees[i];
	}
	current = next;
	loadTex();
}

ThemeHolder::~ThemeHolder(){

	for (int i = 0; i < sharedSize; i++) {
		if (themes[0]->computedKDtrees[i] != nullptr) {
			Kdtree::kill((Node *)themes[0]->computedKDtrees[i]);
			delete ((IndexedModel *)themes[0]->uploadedObj[i]);
		}
	}
	for (int i = 0; i < (signed) themes.size(); i++) {
		delete themes[i]->themeName;
		for (int j = 0; j < size; j++) {
			delete themes[i]->filepath[j];
			if ((themes[i]->computedKDtrees[j] != nullptr) & (j >= sharedSize)) {
				Kdtree::kill((Node *)themes[i]->computedKDtrees[j]);
				delete ((IndexedModel *)themes[i]->uploadedObj[j]);
			}
		}
		delete themes[i];
	}
	themes.clear();
}