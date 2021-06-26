#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cassert>
#include <vector>
#include "SceneParser.h"
#include "Ray.h"
#include "Hit.h"

class SceneParser;


class RayTracer
{
public:
  
  RayTracer()
  {
      assert( false );
  }

  RayTracer( SceneParser* scene, int max_bounces, bool shadow //more arguments as you need...
  );
  ~RayTracer();
  
  Vector3f traceRay( Ray& ray, float tmin, int bounces, 
                     float refr_index, Hit& hit ) const;
        
protected:
  bool shadow;
private:
  SceneParser* m_scene;

  int m_maxBounces;

};

#endif // RAY_TRACER_H
