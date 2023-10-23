#pragma once
#include <vector>

class CreateShape {
private:
	char shapeName[32];
	int shapeSize;
public:
	CreateShape(const char shapeName[32] = "polygon", int shapeSize = 1);

	void displayInfo() const;

	std::vector <float> createVertices(char *typeOfShape);

	char*  getName();
};