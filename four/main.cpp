#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

inline float get_shade_d (Vector3f &L, Vector3f N){
  float ans = Vector3f::dot(L, N);
  if (ans < 0) ans = 0;
  return ans;
}

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
  // Fill in your implementation here.
  char* input_file;
  char* output_file;
  int size[2];

  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  for( int argNum = 2; argNum < argc; ++argNum )
    {
      std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
      if (strcmp(argv[argNum-1], "-input" ) == 0) input_file = argv[argNum];
      if (strcmp(argv[argNum-1], "-size" ) == 0) size[0] = atoi( argv[argNum] );
      if (strcmp(argv[argNum-2], "-size" ) == 0) size[1] = atoi( argv[argNum] );
      if (strcmp(argv[argNum-1], "-output" ) == 0) output_file = argv[argNum];
    }
	
  // First, parse the scene using SceneParser.
  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.
  cout << "parse " << input_file << endl;
  
  SceneParser sp = SceneParser(input_file);
  Camera *camera = sp.getCamera();
  Group *group = sp.getGroup();
  Vector3f bg_color = sp.getBackgroundColor();
  Vector3f ab_light = sp.getAmbientLight();
  int num_light = sp.getNumLights();
  vector<Light*> lights;
  for (int i = 0; i < num_light; i++)
    lights.push_back(sp.getLight(i));

  Image image(size[0], size[1]);
  cout << "Construct Image "  << endl;

  for (int x = 0; x < size[0]; x++){
    for (int y = 0; y < size[1]; y++){
      Hit h = Hit( FLT_MAX, NULL, Vector3f(0.0, 0.0, 0.0));
      float x_nom = -1 + 2.0f*x / (size[0]-1);
      float y_nom = -1 + 2.0f*y / (size[1]-1);
      Ray ray = camera->generateRay(Vector2f(x_nom, y_nom));
      // cout << "ray:" << 
      // ray.getDirection().x() << " | " <<
      // ray.getDirection().y() << " | " <<
      // ray.getDirection().z() << " | " <<
      // endl; 
      bool has_inters = group->intersect(ray, h, camera->getTMin());
      if (has_inters){
        // cout << "h.material:" << h.getMaterial() << endl;
        cout << "T:" << h.getT() << endl;
        assert(h.getMaterial() != 0);
        assert(h.getT() >= camera->getTMin());
        
        Vector3f point = ray.pointAtParameter(h.getT() );
        // cout << "hit:" << h << endl;
        // set color of this pixel
        Vector3f c_pixel = ab_light ;
        // cout << "num_light: " << num_light << endl;
        for (int i = 0; i < num_light; i++){
          Vector3f dir, col;
          float distanceToLight = -1;
          lights[i]->getIllumination(point, dir, col, distanceToLight);
          // cout << "col: " << col << "normal: " << h.getNormal() << endl;
          // cout << "col: " << col << endl;
          c_pixel = c_pixel + get_shade_d(dir, h.getNormal() ) * col * h.getMaterial()->getDiffuseColor();

          // get specttale shader
          Vector3f d = ray.getDirection();
          Vector3f N = h.getNormal();
          Vector3f R = (d + (-2.0f) * Vector3f::dot(d, N) * N) .normalized(); 
          // c_pixel = c_pixel + pow(get_shade_d(dir, R ) * col * h.getMaterial()->getSpecularColor();
        }

        // float tt = h.getT();
        image.SetPixel(x, y, c_pixel);
        // cout << "color pixel:" << tt << endl;
      }
      else {
        image.SetPixel(x, y, bg_color); 
        // cout << "bg pixel:" << bg_color << endl;
      }

    }
  }
  image.SaveImage(output_file);
  cout << "Image save " << output_file  << endl;
  return 0;

  /*
  ///TODO: below demonstrates how to use the provided Image class
  ///Should be removed when you start
  Vector3f pixelColor (1.0f,0,0);
  //width and height
  Image image( 10 , 15 );
  image.SetPixel( 5,5, pixelColor );
  image.SaveImage("demo.bmp");
  return 0;
  */
}

