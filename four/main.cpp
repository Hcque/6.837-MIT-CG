#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include "RayTracer.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);


#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
  // Fill in your implementation here.
  char* input_file;
  char* output_file;
  int size[2];
  int bounce;
  bool shadow;

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
      if (strcmp(argv[argNum-1], "-bounces" ) == 0) bounce = atoi(argv[argNum]);
      if (strcmp(argv[argNum], "-shadows" ) == 0) shadow = true;
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
  Vector3f bg_color = sp.getBackgroundColor(Vector3f(0.0, 0.0, 0.0));
  Vector3f ab_light = sp.getAmbientLight();
  int num_light = sp.getNumLights();
  vector<Light*> lights;
  for (int i = 0; i < num_light; i++)
    lights.push_back(sp.getLight(i));

  Image image(size[0], size[1]);
  Image image_nom(size[0], size[1]);
  cout << "Construct Image "  << endl;

  image.SetAllPixels(bg_color);
  image_nom.SetAllPixels(bg_color);
  float maxT = -1.0;
  vector<Hit> hits;

  for (int x = 0; x < size[0]; x++){
    for (int y = 0; y < size[1]; y++){
      Hit h = Hit( FLT_MAX, NULL, Vector3f(0.0, 0.0, 0.0));
      float x_nom = -1 + 2.0f*x / (size[0]-1);
      float y_nom = -1 + 2.0f*y / (size[1]-1);
      Ray ray = camera->generateRay(Vector2f(x_nom, y_nom));
  
      Vector3f c_pixel = ab_light;
      RayTracer rt = RayTracer(&sp, 10, shadow); // 10 is max bounces
      c_pixel = c_pixel +  rt.traceRay(ray, camera->getTMin() + 0.1, bounce, 1.0f , h);

      image.SetPixel(x, y, c_pixel);
      image_nom.SetPixel(x, y, h.getNormal());
      maxT = max(maxT, h.getT());
    }
  }
  image.SaveImage(output_file);
  image_nom.SaveImage("nom.bmp");
  cout << "Image save " << output_file  << endl;

/*
  Image imagedepth(size[0], size[1]);
  cout << "Construct Image "  << endl;
  // depth
  for (int x = 0; x < size[0]; x++){
    for (int y = 0; y < size[1]; y++){
      Hit h = hits[size[1]*x + y];
        if (h.getMaterial() != NULL){
          float cell = 1 - h.getT() / maxT;
          assert (cell >= 0 && cell <= 1.0f);
          Vector3f c_pixel(cell, cell, cell);
          imagedepth.SetPixel(x, y, c_pixel);
        }
        else 
          imagedepth.SetPixel(x, y, bg_color); 
    }
  }

  imagedepth.SaveImage("depth.bmp");
  */

  return 0;
}

