#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
  Transform(){}
 Transform( const Matrix4f& m, Object3D* obj ):o(obj){
   _m = m;
   o = obj;
  }
  ~Transform(){
    delete o;
    o = nullptr;
  }
  virtual bool intersect( const Ray& r , Hit& h , float tmin){

    // cout << "trans" << endl;
    Vector3f origin = (_m.inverse() * Vector4f( r.getOrigin(), 1.0)).xyz();
    Vector3f dir = (_m.inverse() * Vector4f( r.getDirection(), 1.0)).xyz();
    Ray trans_ray(origin, dir);
    bool hasinters = o->intersect(  trans_ray, h , tmin);
    if (! hasinters) 
      return false;
    // position, direction, normal
    Vector3f n = ( _m.inverse().transposed() * Vector4f(h.getNormal(), 1.0)).xyz();

    assert (h.getMaterial() != 0);
    h.set(h.getT(), h.getMaterial(), n);
    return true;
  }

 protected:
  Object3D* o; //un-transformed object	
  Matrix4f _m;
};

#endif //TRANSFORM_H
