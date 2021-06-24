#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>
#include <cassert>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
public:
	Triangle();
        ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
          hasTex = false;
		  _a = a;
		  _b = b;
		  _c = c;
	}

	virtual bool intersect( const Ray& ray,  Hit& hit , float tmin){
		// hit.set();
		// cout << "Triangle " << endl;
		
		Vector3f Rd = ray.getDirection();
		Vector3f Ro = ray.getOrigin();
		Matrix3f A(
			_a.x()-_b.x(), _a.x()-_c.x(), Rd.x(),
			_a.y()-_b.y(), _a.y()-_c.y(), Rd.y(),
			_a.z()-_b.z(), _a.z()-_c.z(), Rd.z()
		);

		Matrix3f A_t(
			_a.x()-_b.x(), _a.x()-_c.x(), _a.x()-Ro.x(),
			_a.y()-_b.y(), _a.y()-_c.y(), _a.y()-Ro.y(),
			_a.z()-_b.z(), _a.z()-_c.z(), _a.z()-Ro.z()
		);
		
		Matrix3f A_beta(
			_a.x()-Ro.x(), _a.x()-_c.x(), Rd.x(),
			_a.y()-Ro.y(), _a.y()-_c.y(), Rd.y(),
			_a.z()-Ro.z(), _a.z()-_c.z(), Rd.z()
		);
		Matrix3f A_gamma(
			_a.x()-_b.x(), _a.x()-Ro.x(), Rd.x(),
			_a.y()-_b.y(), _a.y()-Ro.y(), Rd.y(),
			_a.z()-_b.z(), _a.z()-Ro.z(), Rd.z()
		);

		float t = A_t.determinant() / A.determinant();
		float beta = A_beta.determinant() / A.determinant();
		float gamma = A_gamma.determinant() / A.determinant();

		if ( ! (beta >= 0 && gamma >= 0 && beta+gamma<=1 && t >= tmin && t <= hit.getT()) ) 
			return false;
		// cout << "t in triangle" << t 
		// 	<< "beta: " << beta 
		// 	<< "gamma: " << gamma << endl;

		Vector3f n = (1-beta-gamma)*normals[0] + beta*normals[1] + gamma*normals[2];
		hit.set(t, this->material, n);
			assert (hit.getMaterial() != 0);
		return true;
	}
	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];
protected:
	Vector3f _a, _b, _c;

};

#endif //TRIANGLE_H
