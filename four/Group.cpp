#include "Group.h"

Group::Group( int num_objects ){
    for (int i = 0; i < num_objects; i++)
      _container.push_back(nullptr);
  }

bool Group::intersect( const Ray& r , Hit& h , float tmin ) {
    bool ans = false;
    for (auto i: _container){
        // cout << "group intersect" << i << endl;
        if( i->intersect(r, h, tmin)) {
            assert(h.getMaterial() != 0);
            assert (h.getT() >= tmin);
            ans = true;
        }
    }
    return ans;
}

void Group::addObject( int index , Object3D* obj ){
    assert(index < _container.size());
    _container[index] = obj;
}

int Group::getGroupSize(){ 
    return _container.size();
}

