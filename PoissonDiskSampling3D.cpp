#include "PoissonDiskSampling3D.h"
#include <random>
#include <iostream>

PoissonDiskSampling3D::PoissonDiskSampling3D(glm::vec3 areaMin, glm::vec3 areaMax, float minDistance, int numPoints)
    : areaMin(areaMin), areaMax(areaMax), minDistance(minDistance), numPoints(numPoints) {
    
}

void PoissonDiskSampling3D::generatePoints() {
    // Implement point generation logic here
    points.clear(); 
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> xDist(areaMin.x, areaMax.x);
    std::uniform_real_distribution<float> yDist(areaMin.y, 0.1);
    std::uniform_real_distribution<float> zDist(areaMin.z, areaMax.z);

    while (points.size() < numPoints) {
        glm::vec3 newPoint(xDist(rng), yDist(rng), zDist(rng));
        if (isWithinArea(newPoint) && !isPointNearby(newPoint)) {
            points.push_back(newPoint);
        }
    }
}

std::vector<glm::vec3> PoissonDiskSampling3D::getPoints() {
    return points;
}

bool PoissonDiskSampling3D::isWithinArea(const glm::vec3& point) {
    return point.x >= areaMin.x && point.x <= areaMax.x &&
        point.y >= areaMin.y && point.y <= areaMax.y &&
        point.z >= areaMin.z && point.z <= areaMax.z;
}

bool PoissonDiskSampling3D::isPointNearby(const glm::vec3& point) {
    for (const glm::vec3& existingPoint : points) {
        float distance = glm::length(existingPoint - point);
        if (distance < minDistance) {
            return true; // Point is too close to an existing point
        }
    }
    return false; // Point is not too close to any existing points
}
