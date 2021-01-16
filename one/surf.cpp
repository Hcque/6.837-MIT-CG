#include <cmath>
#include "surf.h"
#include "extra.h"
using namespace std;

const float PI = 3.14159;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

Matrix3f generateRotation(float theta) {

    Matrix3f rotation(cos(theta), 0, -sin(theta),
                        0, 1, 0,
                        sin(theta), 0, cos(theta)
                       );
    return rotation;

}


Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))    
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    // rotation matrix : transform (x, y, 0) to (t(x), t(z), y)
    // y now becomes the "height" in z-axis
    float theta = 2*PI / steps;
    
    // from start to end, each layer get its rotation
    // each layer has #steps of points in VV
    vector<Vector3f> VV;
    vector<Vector3f> VN;
    vector< Tup3u > VF;
    for (size_t i = 0; i < profile.size(); ++i) {
        for (size_t j = 0; j < steps; ++j) {
            VV.push_back( generateRotation(j*theta) * profile[i].V );
            VN.push_back( generateRotation(j*theta) * ((-1) * profile[i].N ));

            // link surfaces, leave the end
            if (j == steps - 1) {
                if ((i != profile.size()-1)) {
                    int pos = i*steps + j;
                    int posNext = (i+1)*steps;
                    Tup3u face1(pos, i*steps, (i+1)*steps);
                    Tup3u face2(pos, (i+1)*steps + j, (i+1)*steps);
                    VF.push_back(face1);
                    VF.push_back(face2);
                }
                
                continue;
            }
            int pos = i*steps + j;
            int posNext = (i+1)*steps + j;
            Tup3u face1(pos, pos + 1, posNext+1);
            Tup3u face2(pos, posNext, posNext+1);
            VF.push_back(face1);
            VF.push_back(face2);
        }
    }

    surface = {VV, VN, VF};
    return surface;
}

void constructFaces(vector<Tup3u> &VF, int NumOfOneLayer, int layer) {
    for (size_t i = 0; i < layer; ++i) {
        for (size_t j = 0; j < NumOfOneLayer; ++j) {

            // link surfaces, leave the end
            if (j == NumOfOneLayer - 1) {
                if ((i != layer-1)) {
                    int pos = i*layer + j;
                    int posNext = (i+1)*layer;
                    Tup3u face1(pos, i*layer, (i+1)*layer);
                    Tup3u face2(pos, (i+1)*layer + j, (i+1)*layer);
                    VF.push_back(face1);
                    VF.push_back(face2);
                }
                
                continue;
            }
            int pos = i*layer + j;
            int posNext = (i+1)*layer + j;
            Tup3u face1(pos, pos + 1, posNext+1);
            Tup3u face2(pos, posNext, posNext+1);
            VF.push_back(face1);
            VF.push_back(face2);
        }
    }

}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
    vector<Vector3f> VV, VN;
    for (unsigned i = 0; i < sweep.size(); ++i) {
        Vector3f x_axis = sweep[i].T;
        Vector3f y_axis = sweep[i].N;
        Vector3f z_axis = sweep[i].B;

        Matrix3f M(x_axis, y_axis, z_axis);
        Matrix3f M_inv = M.inverse();
        for (unsigned j = 0; j < profile.size(); ++j) {
            // float x = profile[j].V.x();
            // float y = profile[j].V.y();
            // float z = profile[j].V.z();
            // Vector3f newPoint = sweep[i].V + x_axis * x + y_axis * y + z_axis * z;
            Vector3f newPoint = M_inv * profile[j].V;
            VV.push_back(newPoint);

            // calculate coef after changing basis
            Vector3f newT = M.inverse() * profile[j].T;
            VN.push_back(newT);

        }
    }

    vector< Tup3u > VF;
    constructFaces(VF, profile.size(), sweep.size());

    surface = {VV, VN, VF};

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
