#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"
#include <vecmath.h>

#define EPSILON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming)
{
  return ( incoming - 2* Vector3f::dot(incoming, normal) * normal ).normalized();
}

bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming, 
        float index_n, float index_nt, 
        Vector3f& transmitted)
{
  float nr = index_n / index_nt;
  Vector3f N = normal.normalized();
  Vector3f I = (-1.0f)*incoming.normalized();

  float to_root = 1 - nr*nr * (1 - pow(Vector3f::dot(N, I), 2));
  if (to_root < 0)
    return false;
  
  transmitted = ((nr * (Vector3f::dot(N,I)) - sqrt(to_root)) * N - nr * I) .normalized();
  return true;
}

RayTracer::RayTracer( SceneParser * scene, int max_bounces, bool _shadow
  //more arguments if you need...
                      ) :
  m_scene(scene), m_maxBounces(max_bounces), shadow(_shadow)
{
}

RayTracer::~RayTracer()
{
}

bool test_shadow(Ray &ray, SceneParser * m_scene, float tmin, int which_light){

  Hit hit_shaw = Hit();
  Group *group = m_scene->getGroup();

  bool hasHit = group->intersect(ray, hit_shaw, tmin + 0.1) ;
  Vector3f point = ray.pointAtParameter(hit_shaw.getT() );
  Vector3f dir, col;
  float distanceToLight = -1;
  m_scene->getLight(which_light)->getIllumination(point, dir, col, distanceToLight);
  //check shadows
  assert(distanceToLight >= 0);
  float distToHit = (ray.pointAtParameter(hit_shaw.getT()) - ray.getOrigin()).abs();
  if (!hasHit) return true;
  if (hasHit && abs(distToHit - distanceToLight)  < 0.1 ) return true;
  return false;
}

Vector3f RayTracer::traceRay( Ray& ray, float tmin, int bounces,
        float refr_index, Hit& hit ) const
{
    Vector3f ans(0.0, 0.0, 0.0);
    // loop each object
    Group *group = this->m_scene->getGroup();
    int num_light = this->m_scene->getNumLights();

    if (group->intersect(ray, hit, tmin) ){
      Vector3f point = ray.pointAtParameter(hit.getT() );
      // cout << "num_light: " << num_light << endl;
      for (int i = 0; i < num_light; i++){
        Vector3f dir, col;
        float distanceToLight = -1;
        this->m_scene->getLight(i)->getIllumination(point, dir, col, distanceToLight);
        //check shadows
        Ray ray_shaw = Ray( point, dir);
        if (this->shadow && test_shadow(ray_shaw, m_scene, tmin, i)){
          // assert(distanceToLight >= 0);
          ans = ans + hit.getMaterial()->Shade(ray, hit, dir, col);
        }
        if (! this->shadow)
          ans = ans + hit.getMaterial()->Shade(ray, hit, dir, col);
      }
    }
    else {
      return m_scene->getBackgroundColor(ray.getDirection().normalized()); // not hit any objects
    }

    if (bounces == 0 || bounces > m_maxBounces){
      // base case
      return ans;
    }

    Vector3f point_hit = ray.pointAtParameter(hit.getT());
    Vector3f transmitted;
    bool trans = false;
    float R = 1;
    float nt = hit.getMaterial()->getRefractionIndex();
    if ( nt > 0 && transmittedDirection( hit.getNormal(), ray.getDirection(), refr_index, hit.getMaterial()->getRefractionIndex() , transmitted)){

      //cal contribution
      Vector3f d = ray.getDirection().normalized();
      Vector3f N = hit.getNormal().normalized();
      float ni = refr_index;
      float R0 = pow(((nt - ni) / (nt + ni)), 2);
      float c = (ni <= nt) ? (Vector3f::dot(d, N)) : (Vector3f::dot(transmitted, N)); 
      if (c < 0) c = -c;
      R = R0 + (1-R0)*pow(1-c,5);

      // refraction
      Ray refrat_r = Ray(point_hit,  transmitted);
      Hit hit1 = Hit( FLT_MAX, NULL, Vector3f(0.0, 0.0, 0.0));
      ans = ans + (1-R)*traceRay( refrat_r, tmin, bounces-1, hit.getMaterial()->getRefractionIndex(), hit1);
    }
    // cout << "R" << R << endl;
    assert(R >= 0 && R <= 1);

    // reflect
    Ray reflect_r = Ray(point_hit,  mirrorDirection( hit.getNormal(), ray.getDirection()) );
    Hit hit2 = Hit( FLT_MAX, NULL, Vector3f(0.0, 0.0, 0.0));
    ans = ans + R * traceRay(reflect_r, tmin, bounces-1, hit.getMaterial()->getRefractionIndex(), hit2);

    return ans;
}
