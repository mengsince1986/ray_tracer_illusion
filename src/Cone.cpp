/*----------------------------------------------------------
* COSC363  Assignment 2
*
*  The Cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray. 
* 
* source ray: P0(x0, y0, z0) with unite dir vector d(dx, dy, dz)
* intersection point: P(x, y, z), 
* 
* Equaction of cone with base at (xc, yc, zc), axis paralle to the y-axis, 
* radius R and height h:
* (x - xc)^2 + (z-zc)^2 = (R/h)^2 * (h - y + yc)^2
*
* Ray equation: x = x0 + dx*t; y = y0 + dy*t; z = z0 + dz*t; 
*
* Substitute into ray equation
* 
* 
*/
float Cone::intersect(glm::vec3 p0, glm::vec3 dir)
{
    return 0.0
}


/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    return 0.0
}
