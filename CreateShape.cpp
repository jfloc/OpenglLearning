#include "CreateShape.h"
#include <iostream>
#include <string>


CreateShape::CreateShape(const char shapeName[32], int shapeSize) {
    strncpy_s(this->shapeName, shapeName, sizeof(this->shapeName) - 1);
    this->shapeName[sizeof(this->shapeName) - 1] = '\0';// Ensure null-termination
    this->shapeSize = shapeSize;
}

// Function definition
void CreateShape::displayInfo() const {
    // Using this-> to access member variables
    std::cout << "Shape Name: " << this->shapeName << std::endl;
    std::cout << "Shape Size: " << this->shapeSize << std::endl;
}

std::vector <float> CreateShape::createVertices(char *typeOfShape) {
    typeOfShape = this->shapeName;

    std::cout << typeOfShape;

    return std::vector<float> {1, 2, 3, 4};
}

char* CreateShape::getName(){
    return this->shapeName;
}