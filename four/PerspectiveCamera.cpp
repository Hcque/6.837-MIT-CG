#include "Camera.h"
#include <cassert>

PerspectiveCamera::PerspectiveCamera(const Vector3f& center, const Vector3f& direction,const Vector3f& up , float angle) 
	{
		this->center = center;
		this->direction = direction;
		this->up = up;
		this->angle = angle;
		this->horizontal = Vector3f::cross(direction, up);

        //coordiantes
        this->w = direction.normalized();
        // this->u = this->horizontal.normalized();
        assert(up.abs() == 1.0 );
        this->u = Vector3f::cross(w, up).normalized();
        this->v = Vector3f::cross(u, w).normalized();
        // this->verticle = Vector3f::cross(up , u).normalized();
	}

Ray PerspectiveCamera::generateRay( const Vector2f& point){
    float D = 1.0f / tan(angle/2.0f);
    assert(D > 0);
    Vector3f ray_dir =  this->u * point[0]  + this->v * point[1]+ this->w * D;
    return Ray(this->center, ray_dir.normalized());
}

// float PerspectiveCamera::getTMin() const { 
//     return 1 / tan(angle/2);
// }

