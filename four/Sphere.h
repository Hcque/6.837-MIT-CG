#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere(){ 
		//unit ball at the center
		_center = Vector3f(0.0, 0.0, 0.0);
		_radius = 1.0;
	}

	Sphere( Vector3f center , float radius , Material* material ):Object3D(material){
		_center = center;
		_radius = radius;
	}
	

	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin);

protected:
	Vector3f _center;
	float _radius;
};


#endif
