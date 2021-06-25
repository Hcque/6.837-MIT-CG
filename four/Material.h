#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:
	
 Material( const Vector3f& d_color ,const Vector3f& s_color=Vector3f::ZERO, float s=0):
  diffuseColor( d_color),specularColor(s_color), shininess(s)
  {
        	
  }

  virtual ~Material()
    {

    }

  virtual Vector3f getDiffuseColor() const 
  { 
    return  diffuseColor;
  }

  virtual Vector3f getSpecularColor() const 
  { 
    return  specularColor;
  }

  virtual float getShine() const 
  { 
    return  shininess;
  }
    
  virtual Texture getTexture() const 
  { 
    return  t;
  }

inline float get_shade_d (const Vector3f &L, Vector3f N){
  float ans = Vector3f::dot(L, N);
  if (ans < 0) ans = 0;
  return ans;
}

  Vector3f Shade( const Ray& ray, const Hit& hit,
                  const Vector3f& dirToLight, const Vector3f& lightColor ) {


    Vector3f Kd = hit.getMaterial()->getDiffuseColor();

    // add texture if possiable
    if (t.valid() && hit.hasTex){
      Vector3f color = t(hit.texCoord[0], hit.texCoord[1]);
      Kd = color;
    }

    Vector3f c_pixel = get_shade_d(dirToLight, hit.getNormal() ) * lightColor * Kd;
    if (hit.getMaterial()->getShine() != 0){
        Vector3f d = ray.getDirection().normalized();
        Vector3f N = hit.getNormal().normalized();
        Vector3f R = (d + (-2.0f) * Vector3f::dot(d, N) * N) .normalized(); 
        
        float shine = hit.getMaterial()->getShine();
        c_pixel = c_pixel + pow(get_shade_d(dirToLight, R), shine) * lightColor * hit.getMaterial()->getSpecularColor();
    }

    return c_pixel; 
  }

  void loadTexture(const char * filename){
    t.load(filename);
  }
 protected:
  Vector3f diffuseColor;
  Vector3f specularColor;
  float shininess;
  Texture t;
};



#endif // MATERIAL_H
