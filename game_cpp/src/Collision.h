#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>

class Collision{
    public:
        static bool CheckCollision_Sphere(glm::vec3 SphereCenter,float radius,glm::vec3 aabbCenter,glm::vec3 aabbSize);
};

#endif