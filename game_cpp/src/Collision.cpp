#include "Collision.h"
#include <algorithm>
using namespace std;

bool Collision::CheckCollision_Sphere(glm::vec3 SphereCenter,float radius,glm::vec3 aabbCenter,glm::vec3 aabbSize){
    // extracting the edges
    glm::vec3 halfExtents = aabbSize/2.0f;
    glm::vec3 aabbMin = aabbCenter - halfExtents;
    glm::vec3 aabbMax = aabbCenter + halfExtents;

    float closestX = max(aabbMin.x,min(SphereCenter.x,aabbMax.x));
    float closestY = max(aabbMin.y,min(SphereCenter.y,aabbMax.y));
    float closestZ = max(aabbMin.z,min(SphereCenter.z,aabbMax.z));

    glm::vec3 closestPoint = glm::vec3(closestX,closestY,closestZ);
    float dist = glm::distance(SphereCenter,closestPoint);

    return dist < radius;
}