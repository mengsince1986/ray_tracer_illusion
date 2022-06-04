/*----------------------------------------------------------
*  COSC363  Assignment 2
*
*  Author: MENG ZHZNAG
*  The Cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/
#include "Cylinder.h"
#include <math.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Plane.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include "TextureBMP.h"
#include <GL/freeglut.h>

/**
* Cylinder's intersection method. The input is a ray.
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - center; // vdif[0] = (x0-xc), vdif[2] = (z0-zc)
    float a = dir.x * dir.x + dir.z * dir.z;
    float b = 2 * (dir.x * vdif.x + dir.z * vdif.z);
    float c = vdif.x *  vdif.x + vdif.z * vdif.z - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (a == 0.0 || discriminant < 0.0) return -1.0;

    float t1 = (-b - sqrt(discriminant)) / (2 * a); // intersection 1
    float t2 = (-b + sqrt(discriminant)) / (2 * a); // intersection 2

    if (t1 < 0.0 && t2 < 0.0) return -1.0;

    // intersection points
    glm::vec3 p1 = p0 + t1 * dir;
    glm::vec3 p2 = p0 + t2 * dir;
    float heightY = center.y + height;

    if (t1 > 0.0 && p1.y <= heightY && p1.y >= center.y) { // p1 on cylinder
        return t1;
    } else if (p2.y <= heightY && p2.y >= center.y) { // only p2 on cylinder
        return t2;
    } else {
        return -1.0;
    }

}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Cylinder.
* (un-normalized) n = (x - xc, 0, z-zc)
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3 (p[0]-center[0], 0, p[2]-center[2]);
    n = glm::normalize(n);
    return n;
}
