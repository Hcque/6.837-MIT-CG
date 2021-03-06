#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <cmath>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}


Matrix4f B_bezier(1,-3,3,-1,
        0,3,-6,3,
        0,0,3,-3,
        0,0,0,1);

Matrix4f B_bezier_inv = B_bezier.inverse();

double f = 1.f/6; 
Matrix4f B_bspline(1*f,-3*f,3*f,-1*f,
        4*f,0,-6*f,3*f,
        1*f,3*f,3*f,-3*f,
        0,0,0,1*f);

// concat vectors
Curve concat(Curve& left, Curve& right) {
	
	for (unsigned i = 0; i < right.size(); ++i) {

		left.push_back(right[i]);
	}
	return left;
}

// recursive imple
Curve deCast(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, int times) {

	if (times <= 0) return Curve();
	Vector3f p01 = ( p0 + p1 ) / 2;
	Vector3f p12 = ( p1 + p2 ) / 2;
	Vector3f p23 = ( p2 + p3 ) / 2;
	Vector3f e0 = ( p01 + p12 ) / 2;
	Vector3f e1 = ( p12 + p23 ) / 2;
	Vector3f mid = ( e0 + e1 ) / 2;

	Vector3f T = (p3 - p2) - (p1 - p0);
	T.normalize();

	Vector3f N = T / Vector3f::dot(T, T);
	Vector3f B = Vector3f::cross(T, N);

	CurvePoint midPoint = {
		mid,
		T,
		N,
		B
	};

	times--;

	Curve left = deCast(p0, p01, e0, mid, times);
	Curve right = deCast(mid, e1, p23, p3, times);
	left.push_back(midPoint);
	return concat(left, right);

}

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;

    Curve result;

    /*
    steps = steps / (P.size() / 3);
    for( unsigned i = 0; i < P.size()-3; ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
	
	// get 4 points
	Vector3f p0 = P[i+0];
	Vector3f p1 = P[i+1];
	Vector3f p2 = P[i+2];
	Vector3f p3 = P[i+3];

	// using de castjou
	Curve onepiece = deCast(p0, p1, p2, p3, steps);
	result = concat(result, onepiece);
	
	i += 3;

    }
    */


    int innerSteps = steps / (P.size() / 4);
    for( unsigned i = 0; i < P.size()-3; ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
	
        // get 4 points
        Vector4f p0(P[i+0], 1.0);
        Vector4f p1(P[i+1], 1.0);
        Vector4f p2(P[i+2], 1.0);
        Vector4f p3(P[i+3], 1.0);

        // direct compute based on monomial basis
        for (int p = 1; p < innerSteps; ++p) {
            float t = (float)p / innerSteps;
            Vector4f PowerBasis(1, t, t*t, pow(t, 3));
            Vector4f PowerDerivtive(0, 1, 2*t, 3*t*t);

            Matrix4f G(p0, p1, p2, p3);
            Matrix4f coef(G * B_bezier); // coef is the coeffients of monomial basis
            Vector3f pos3f = (coef * PowerBasis).xyz();
            /*
            cout << "out:" << endl;
            cout << pos3f.x() << " "
            << pos3f.y() << " "
            << pos3f.z()  << endl;
            */

            Vector3f T = (coef * PowerDerivtive).xyz();
            T.normalize();
            Vector3f N = T / Vector3f::dot(T, T);
            Vector3f B = Vector3f::cross(T, N);

            CurvePoint o = {
                pos3f, T, N, B
            };

            result.push_back(o);
        }
            
        // result = concat(result, onepiece);
        i += 3;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;
    // Right now this will just return this empty curve.
//    return Curve();
	
    return result;
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;

    Curve newCurve;
    for( unsigned i = 0; i < P.size()-3; ++i )
    {
        cerr << "\t>>> " << P[i] << endl;

        vector<Vector3f> newP;
        // 
        
        // get 4 control points
        Vector4f p0(P[i+0],1);
        Vector4f p1(P[i+1],1);
        Vector4f p2(P[i+2], 1);
        Vector4f p3(P[i+3], 1);
        
        // get the new control points (newG) : G * Bspline * Bezier_inverse
        Matrix4f G(p0,p1,p2,p3,true);
        Matrix4f newG = (G * B_bspline * B_bezier_inv);

        for (int j = 0; j < 4; ++j) {
            Vector3f pnewX = newG.getCol(j).xyz();
            newP.push_back(pnewX);
        }

        Curve onePiece = evalBezier(newP, steps);
        concat(newCurve, onePiece);
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning bsp curve." << endl;

    return newCurve;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

