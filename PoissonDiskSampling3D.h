#pragma once
#include <vector>
#include <glm/glm.hpp>

class PoissonDiskSampling3D {
public:
    PoissonDiskSampling3D(glm::vec3 areaMin, glm::vec3 areaMax, float minDistance, int numPoints);

    // Public functions to generate and retrieve points
    void generatePoints();
    std::vector<glm::vec3> getPoints();

private:
    glm::vec3 areaMin;
    glm::vec3 areaMax;
    float minDistance;
    int numPoints;
    std::vector<glm::vec3> points;

    // Private helper functions
    bool isWithinArea(const glm::vec3& point);
    bool isPointNearby(const glm::vec3& point);
};
