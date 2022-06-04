/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Sphere.h"
#include <math.h>

/**
* Sphere's intersection method.  The input is a ray. 
*/
float Sphere::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - center;   //Vector s=p0-C (see Slide 28)
    float b = glm::dot(dir, vdif); // b -> s.d
    float len = glm::length(vdif); // len = sqrt(x^2 + y^2 + z^2)
    float c = len*len - radius*radius;  // len * len == s.s = x^2 + y^2 + z^2
    float delta = b*b - c; // b * b = (s.d)^2, c = (s.s) + r^2
   
	if(delta < 0.001) return -1.0;    //includes zero and negative values

    float t1 = -b - sqrt(delta); // intersection 1
    float t2 = -b + sqrt(delta); // intersection 2

	if (t1 < 0)
	{
		return (t2 > 0) ? t2 : -1;
	}
	else return t1;
}


/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Sphere::normal(glm::vec3 p)
{
    glm::vec3 n = p - center;
    n = glm::normalize(n);
    return n;
}
