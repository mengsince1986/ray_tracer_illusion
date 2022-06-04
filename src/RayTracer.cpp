/*==================================================================================
* COSC 363 Assignment 2 (2022-S1)
*
* Author: Meng Zhang (mzh103)
* File: RayTracer.cpp
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Plane.h"
#include "SceneObject.h"
#include "Ray.h"
#include "TextureBMP.h"
#include <GL/freeglut.h>
using namespace std;

const float EDIST = 40.0;
const int NUMDIV = 600;
const int MAX_STEPS = 100;
const float XMIN = -20.0;
const float XMAX = 20.0;
const float YMIN = -20.0;
const float YMAX = 20.0;

vector<SceneObject*> sceneObjects;

TextureBMP texture;

float PI = 3.141592653589793;


//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
	glm::vec3 lightPos1(20, 30, -20);					//Light's position
	glm::vec3 lightPos2(-20, 30, -20);					//Extra light's position
	glm::vec3 color(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

    // 1. ADD CHEQUERED PATTERN FOR FLOOR PLANE OBJECT
    if (ray.index == 0)      // suppse the index of the floor plane is 4
    {
        int squareWidth = 5;
        glm::vec3 dark = glm::vec3(0.2, 0.2, 0.2);
        glm::vec3 light = glm::vec3(0.8, 0.8, 0.8);
        bool isKzSolid;
        bool isKxSolid;
        //  z-axis (iz is always negative)
        int iz = (ray.hit.z) / squareWidth;
        int kz = iz % 2;
        isKzSolid = kz == 0;
        //  x-axis (ix is neither positive or negative and can be 0)
        int ix = (ray.hit.x) / squareWidth;
        int kx = ix % 2;
        if (ray.hit.x >= 0) {
            isKxSolid =  kx == 0;
        } else {
            isKxSolid = kx != 0;
        }

        if ((isKzSolid && isKxSolid) || (!isKzSolid && !isKxSolid)) {
            color = dark;
        } else {
            color = light;
        }
        obj->setColor(color);

        //Add code for texture mapping
        /*
        float x1 = -15;  // based on fig.10 on page6 lab07 doc
        float x2 = 5;
        float z1 = -60;
        float z2 = -90;
        float texcoords = (ray.hit.x-x1)/(x2-x1);
        float texcoordt = (ray.hit.z-z1)/(z2-z1);
        if(texcoords > 0 && texcoords < 1 &&
           texcoordt > 0 && texcoordt < 1)
        {
            color=texture.getColorAt(texcoords, texcoordt);
            obj->setColor(color);
        }
        */
    }


    if (ray.index == 6) // texturing for sphere 1
    {
        
        glm::vec3 po = ray.hit;
        glm::vec3 centre = glm::vec3(-22.0, 2.0, -65.0); // the center of sphere 1
        glm::vec3 unitVec = glm::normalize(centre - po);        
        float texcoords = 0.5f + (atan2(unitVec[0], unitVec[2]) / (2.0 * PI));
        float texcoordt = 0.5f + (asin(unitVec[1]) / PI);
        color = texture.getColorAt(texcoords, texcoordt);
        obj->setColor(color);
    }


    //OBJECT'S COLOUR
	// color = obj->getColor();
    // SET LIGHTINGS
    glm::vec3 ambienTerm = glm::vec3 (0.2, 0.2, 0.2) * obj->getColor();

    if (ray.index != 1) {
        color = obj->lighting(lightPos1, -ray.dir, ray.hit) +
            obj->lighting(lightPos2, -ray.dir, ray.hit) - ambienTerm;
    } else {
        color = obj->getColor();
    }

    
    // ADD SHADOWS
    glm::vec3 lightVec1 = lightPos1 - ray.hit;
    glm::vec3 lightVec2 = lightPos2 - ray.hit;
    Ray shadowRay1(ray.hit, lightVec1);  // Create a shadow ray1
    Ray shadowRay2(ray.hit, lightVec2);  // Create a shadow ray2
    shadowRay1.closestPt(sceneObjects);  // find the closest point of intersect
    shadowRay2.closestPt(sceneObjects);  // find the closest point of intersect
    const float lightDist1 = glm::length(lightVec1);
    const float lightDist2 = glm::length(lightVec2);

    if ((shadowRay1.index > -1 && shadowRay1.dist < lightDist1) &&
        (shadowRay2.index > -1 && shadowRay2.dist < lightDist2)) {
        bool isTranspObj1 = sceneObjects[shadowRay1.index]->isTransparent();
        bool isRefractObj1 = sceneObjects[shadowRay1.index]->isRefractive();
        bool isTranspObj2 = sceneObjects[shadowRay2.index]->isTransparent();
        bool isRefractObj2 = sceneObjects[shadowRay2.index]->isRefractive();
        if ((isTranspObj1 || isRefractObj1) && (isTranspObj2 || isRefractObj2)) {
            color = 0.45f * obj->getColor();
        } else {
            //set color to be shadow. 0.2 = ambient scale factor
            color = ambienTerm;
        }
    } else if (shadowRay1.index > -1 && shadowRay1.dist < lightDist1) { // check if in shadow1
        // check if transparent/refractive object or selfShadowed
        bool isTranspObj1 = sceneObjects[shadowRay1.index]->isTransparent();
        bool isRefractObj1 = sceneObjects[shadowRay1.index]->isRefractive();
        //bool notSelfShadow = !(ray.index == shadowRay1.index);
        if (isTranspObj1 || isRefractObj1) {
            color = 0.6f * obj->getColor();
        } else {
            //set color to be shadow. 0.2 = ambient scale factor
            color = 0.34f * obj->getColor();
        }
    } else if (shadowRay2.index > -1 && shadowRay2.dist < lightDist2) { // check if in shadow2
        // check if transparent/refractive object or selfShadowed
        bool isTranspObj2 = sceneObjects[shadowRay2.index]->isTransparent();
        bool isRefractObj2 = sceneObjects[shadowRay2.index]->isRefractive();
        //bool notSelfShadow = !(ray.index == shadowRay1.index);
        if (isTranspObj2 || isRefractObj2) {
            color = 0.6f * obj->getColor();
        } else {
            //set color to be shadow. 0.2 = ambient scale factor
            color = 0.4f * obj->getColor();
        }
    }


    // REFLECTIVITY
    if (obj->isReflective() && step < MAX_STEPS)
    {
        float rho = obj->getReflectionCoeff();
        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
        Ray reflectedRay(ray.hit, reflectedDir);
        glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
        color = color + (rho * reflectedColor);
    }


    // TRANSPARENCY
    if (obj->isTransparent() && step < MAX_STEPS)
    {
        float transpCoeff = obj->getTransparencyCoeff();
        Ray insideRay(ray.hit, ray.dir);
        insideRay.closestPt(sceneObjects);
        Ray transpRay(insideRay.hit, ray.dir);
        glm::vec3 transpColor = trace(transpRay, step + 1);
        color = color + (transpCoeff * transpColor);
    }


    // REFRACTION
    if (obj->isRefractive() && step < MAX_STEPS)
    {
        float refracIndex = obj->getRefractiveIndex();
        float eta = 1.0 / refracIndex;
        float refracCoeff = obj->getRefractionCoeff();
        glm::vec3 normalVec1 = obj->normal(ray.hit); // normal vec n
        glm::vec3 refractedDir1 = glm::refract(ray.dir, normalVec1, eta); // g
        Ray refractedRay1(ray.hit, refractedDir1); // refrRay
        refractedRay1.closestPt(sceneObjects); 
        glm::vec3 normalVec2 = obj->normal(refractedRay1.hit);
        glm::vec3 refractedDir2 = glm::refract(refractedDir1, -normalVec2, 1.0f/eta);
        Ray refractedRay2(refractedRay1.hit, refractedDir2);
        refractedRay2.closestPt(sceneObjects);
        glm::vec3 refractedColor = trace(refractedRay2, step + 1);
        color = color + (refracCoeff * refractedColor);       
    }

    
	return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for(int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		// xp = XMIN + (i + 0.5) * cellX;
        xp = XMIN + i * cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			// yp = YMIN + (j + 0.5) * cellY;
            yp = YMIN + j * cellY;

		    // glm::vec3 dir(xp, yp, -EDIST);	//direction of the primary ray
            glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);

		    Ray ray = Ray(eye, dir);

		    glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
        }
    }

    glEnd();
    glFlush();
}



//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

    // 0:floor plane
    Plane *floor = new Plane (glm::vec3(-50., -15, 1),  // Point A
                              glm::vec3(50., -15, 1),  // Point B
                              glm::vec3(50., -15, -200),  // Point C
                              glm::vec3(-50., -15, -200));  // Point D
    floor->setColor(glm::vec3(0.8, 0.8, 0));
    floor->setSpecularity(false);  // turnoff specularity property
    sceneObjects.push_back(floor);

    

    // 1: ceiling
    Plane *ceiling = new Plane (glm::vec3(-50., 31, -200), // Point D
                                glm::vec3(50., 31, -200),  // Point C
                                glm::vec3(50., 31, 1),  // Point B
                                glm::vec3(-50., 31, 1));  // Point A
    ceiling->setColor(glm::vec3(0.5, 0.5, 0.6));
    ceiling->setSpecularity(false);  // turnoff specularity property
    sceneObjects.push_back(ceiling);

    // 2:left wall
    Plane *leftWall = new Plane (glm::vec3(-50., -15, 1),  // Point A
                                 glm::vec3(-50., -15, -200),  // Point B
                                 glm::vec3(-50., 85, -200),  // Point C
                                 glm::vec3(-50., 85, 1));  // Point D
    leftWall->setColor(glm::vec3(0.8, 0.5, 0.6));
    //leftWall->setColor(glm::vec3(0.0, 0.0, 0.0));        
    leftWall->setSpecularity(false);  // turnoff specularity property
    //leftWall->setReflectivity(true, 0.8);    
    sceneObjects.push_back(leftWall);

    // 3:right wall
    Plane *rightWall = new Plane (glm::vec3(50., -15, -200),  // Point B
                                  glm::vec3(50., -15, 1),   // Point A
                                  glm::vec3(50., 85, 1),    // Point D
                                  glm::vec3(50., 85, -200));  // Point C
    rightWall->setColor(glm::vec3(0.5, 0.8, 0.6));
    //rightWall->setColor(glm::vec3(0.0, 0.0, 0.0));    
    rightWall->setSpecularity(false);  // turnoff specularity property
    //rightWall->setReflectivity(true, 0.8);
    sceneObjects.push_back(rightWall);

    // 4:back wall
    Plane *backWall = new Plane (glm::vec3(-50., -15, -100),  // Point A
                                 glm::vec3(50., -15, -100),  // Point B
                                 glm::vec3(50., 85, -100),  // Point C
                                 glm::vec3(-50., 85, -100));  // Point D
    //backWall->setColor(glm::vec3(0.5, 0.5, 0.5));
    backWall->setColor(glm::vec3(0.0, 0.0, 0.0));    
    backWall->setSpecularity(false);  // turnoff specularity property
    backWall->setReflectivity(true, 0.9);
    sceneObjects.push_back(backWall);

    // 5:front wall
    Plane *frontWall = new Plane (glm::vec3(50., -15, 1),  // Point B
                                  glm::vec3(-50., -15, 1),  // Point A
                                  glm::vec3(-50., 85, 1),  // Point D                                 
                                  glm::vec3(50., 85, 1));  // Point C
    //frontWall->setColor(glm::vec3(0.5, 0.5, 0.9));
    frontWall->setColor(glm::vec3(0.0, 0.0, 0.0));    
    frontWall->setSpecularity(false);  // turnoff specularity property
    frontWall->setReflectivity(true, 0.9);    
    sceneObjects.push_back(frontWall);

    // 6:sphere 1 (texturing reflective)
    Sphere *sphere1 = new Sphere(glm::vec3(-22.0, 2.0, -65.0), 7.0);
	sphere1->setColor(glm::vec3(0, 0, 1));   //Set colour to blue
    // sphere1->setSpecularity(false);      // suppress reflections
    // sphere1->setShininess(5);              // set shininess
    // sphere1->setReflectivity(true, 0.8);   // set reflectivity
    sceneObjects.push_back(sphere1);		 //Add sphere to scene objects

    // texturing
    texture = TextureBMP("Earth2.bmp");

    // 1:sphere 2 (silver solid reflective)
    Sphere *sphere2 = new Sphere(glm::vec3(0.0, -7.0, -32.0), 3.8);
	sphere2->setColor(glm::vec3(0.1, 0.1, 0.));   //Set colour to red
    sphere2->setReflectivity(true, 0.9);
    sceneObjects.push_back(sphere2);		 //Add sphere to scene objects

    // 2:sphere 3 (green transparent reflective)
    Sphere *sphere3 = new Sphere(glm::vec3(-13.0, -7.0, -46.0), 5.0);
	sphere3->setColor(glm::vec3(0.3, 0.4, 0.3));   //Set colour to transparent
    sphere3->setReflectivity(true, 0.05);
    sphere3->setTransparency(true, 0.5);
	sceneObjects.push_back(sphere3);		 //Add sphere to scene objects
    
    // 3:sphere 4 (glass refractive reflective)
    Sphere *sphere4 = new Sphere(glm::vec3(0.0, 12.0, -60.0), 6.5);
	sphere4->setColor(glm::vec3(0.1, 0.1, 0.1));   //Set colour to Cyan
    sphere4->setReflectivity(true, 0.1);
    sphere4->setRefractivity(true, 0.8, 1.5);
	sceneObjects.push_back(sphere4);		 //Add sphere to scene objects    

    // sphere 5 (glass refractive reflective)
    Sphere *sphere5 = new Sphere(glm::vec3(13.0, -7.0, -46.0), 5.0);
	sphere5->setColor(glm::vec3(0.1, 0.1, 0.1));
    sphere5->setReflectivity(true, 0.1);
    sphere5->setRefractivity(true, 0.8, 1.01);
	sceneObjects.push_back(sphere5);	




    // 5,6,7,8,9:pyramid
    float pyraHeight = 10;
    float pyraSize = 11;
    glm::vec3 pyraColor = glm::vec3(0.1, 0.1, 0.5);
    glm::vec3 pyraColorAlt = glm::vec3(0.5, 0.1, 0.1);
    glm::vec3 pyraPos = glm::vec3 (22, -2, -61);
    glm::vec3 pyraTopV = glm::vec3(pyraPos[0],
                                   pyraPos[1]+pyraHeight,
                                   pyraPos[2]);
    // 5:pyramid base
    glm::vec3 pyraBaseA = glm::vec3(pyraPos[0] - 0.5 * pyraSize,
                                    pyraPos[1],
                                    pyraPos[2] + 0.5 * pyraSize);
    glm::vec3 pyraBaseB = glm::vec3(pyraPos[0] + 0.5 * pyraSize,
                                    pyraPos[1],
                                    pyraPos[2] + 0.5 * pyraSize);
    glm::vec3 pyraBaseC = glm::vec3(pyraPos[0] + 0.5 * pyraSize,
                                    pyraPos[1],
                                    pyraPos[2] - 0.5 * pyraSize);
    glm::vec3 pyraBaseD = glm::vec3(pyraPos[0] - 0.5 * pyraSize,
                                    pyraPos[1],
                                    pyraPos[2] - 0.5 * pyraSize);
    Plane *pyraBase = new Plane (pyraBaseA,
                                 pyraBaseB,
                                 pyraBaseC,
                                 pyraBaseD);
    pyraBase->setColor(pyraColor);
    pyraBase->setReflectivity(true, 0.5);
    sceneObjects.push_back(pyraBase);
    // 6:pyramid front plane
    Plane *pyraFront = new Plane(pyraBaseA,
                                 pyraBaseB,
                                 pyraTopV);
    pyraFront->setColor(pyraColor);
    pyraFront->setReflectivity(true, 0.5);
    sceneObjects.push_back(pyraFront);
    //7:pyramid left plane
    Plane *pyraLeft = new Plane(pyraBaseD,
                                pyraBaseA,
                                pyraTopV);
    pyraLeft->setColor(pyraColorAlt);
    pyraLeft->setReflectivity(true, 0.5);
    sceneObjects.push_back(pyraLeft);
    //8:pyramid right plane
    Plane *pyraRight = new Plane(pyraBaseB,
                                 pyraBaseC,
                                 pyraTopV);
    pyraRight->setColor(pyraColorAlt);
    pyraRight->setReflectivity(true, 0.5);
    sceneObjects.push_back(pyraRight);
    //9:pyramid back plane
    Plane *pyraBack = new Plane(pyraBaseC,
                                pyraBaseD,
                                pyraTopV);
    pyraBack->setColor(pyraColor);
    pyraBack->setReflectivity(true, 0.5);
    sceneObjects.push_back(pyraBack);


}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
