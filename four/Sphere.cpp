#include "Sphere.h"


bool Sphere::intersect( const Ray& r , Hit& h , float tmin){
		// cout << "intersect sphere" << endl;
    Vector3f Ro = r.getOrigin() - this->_center;  // for ball not in the 0,0,0
    Vector3f Rd = r.getDirection();

    float a = Vector3f::dot( Rd, Rd);
    float b = 2* Vector3f::dot(Ro, Rd);
    float c = Vector3f::dot(Ro, Ro) - _radius*_radius;
    float t1;
    //  t2;
    float discrim = b*b-4.0*a*c;
    // cout << "calculate two roots t1: " << t1 << endl;
    // cout << "hit: " << h << endl;
    // cout << "dis: " << discrim << endl;
    if (discrim < 0) {
        // h.set(h.getT(), this->material, Vector3f(0.0, 0.0, 0.0));
        return false;
    }
    else if (discrim > 0){
        t1 = ((-1.0)*b - sqrt(discrim) ) / (2*a);
        // t2 = ((-1.0)*b + sqrt(discrim) ) / (2*a);
        // if (t2 < t1) t1 = t2;
    }
    else{
        t1 = ((-1.0)*b ) / (2*a);
    }

    if (t1 >= tmin && t1 <= h.getT()){
        // cout << t1 << endl;
        Vector3f n = (r.getOrigin() + t1 * Rd - this->_center).normalized() ;
        // cout << "sphere normal:" << 
        // n[0] << "|" <<
        // n[1] << "|" <<
        // n[2] << "|" 
        // << endl;
        h.set(t1, this->material, n);
        // h.setDepth()
        return true;
    }
    else return false;
}