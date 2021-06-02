#include "Mesh.h"
#include <cassert>

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.

	cout << "====== " << filename << " =====" << endl;
	ifstream input(filename);
	string buf;
	while (getline(input, buf) ){
		// cout << buf << endl;
		stringstream ss(buf);
		string flag;
		float x, y, z;
		unsigned xx, yy, zz;
		
		ss >> flag;
		if (flag == "v"){
			ss >> x >> y >> z;
			bindVertices.push_back( Vector3f(x,y,z) );
		}
		else if ( flag == "f" ){
			ss >> xx >> yy >> zz;
			faces.push_back( Tuple3u(xx, yy, zz) );
		}
	}
	
	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".

	cout << " start draw mesh ... " << endl;
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < faces.size(); i++){
		Vector3f v1 =  currentVertices[ faces[i][0]-1 ];
		Vector3f v2 =  currentVertices[ faces[i][1]-1 ];
		Vector3f v3 =  currentVertices[ faces[i][2]-1 ];

		Vector3f n = Vector3f::cross ( v1 - v3, v2 - v1 ).normalized();

		glNormal3d(n[0],n[1],n[2]);
		glVertex3d(v1[0],v1[1],v1[2]);
		// glNormal3d(n[0],n[1],n[2]);
		glVertex3d(v2[0],v2[1],v2[2]);
		// glNormal3d(n[0],n[1],n[2]);
		glVertex3d(v3[0],v3[1],v3[2]);
	}
	glEnd();	

}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
	cout << "====== " << filename << " =====" << endl;
	ifstream input(filename);
	string buf;
	while (getline(input, buf) ){	
		stringstream ss(buf);
		float f;
		vector<float> vf;
		int i = 0;
		while ( ss >> f ){
			vf.push_back(f);
			i++;
		}
		assert(i == numJoints-1);

		attachments.push_back(vf);
	}
	
	cout << "====== " << filename << "  DONE ===" << endl;
}
