#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane(){}
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m){
		_normal = normal;
		_d = d;
	}
	~Plane(){}
	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		// cout << "intersect plane" << endl;
		Vector3f Ro = r.getOrigin();
		Vector3f Rd = r.getDirection();
		float t = (-1.0f) * (_d + Vector3f::dot(_normal, Ro) ) / (Vector3f::dot(_normal,Rd));

		if (t <= h.getT() && t >= tmin){
			h.set(t, this->material, _normal.normalized());
			assert (h.getMaterial() != 0);
			return true;
		}
		else return false;
	}

protected:
	Vector3f _normal;
	float _d;
};
#endif //PLANE_H
		

