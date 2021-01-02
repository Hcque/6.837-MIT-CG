#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
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
    
// concat vectors
Curve concat(Curve& left, Curve& right) {
	
	for (int i = 0; i < right.size(); ++i) {

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
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    Curve curve;
    Vector3f Bi_1;
    unsigned short seed = 0;
    Bi_1 = Vector3f(erand48(&seed), 0.0, 1.0);
    Bi_1.normalize(); // it is likely that B0 will not parallel to T1
    Vector3f T1 = - 1.0/2 * P[0] + 1.0/2 * P[2];
    T1.normalize();
    if(1 - Vector3f::dot(T1, Bi_1) < 1e-4){
        Bi_1.y() = 1.0;
        Bi_1.normalize();
    }
	for(unsigned int i = 0; i < P.size() - 3; i += 1) {
		Vector3f P0, P1, P2, P3;
		if(i == P.size() -3) {
			P0 = P[i]; P1 = P[i+1]; P2 = P[i+2]; P3 = P[i+2];
		}
		else {
			P0 = P[i]; P1 = P[i+1]; P2 = P[i+2]; P3 = P[i+3];
		}
	    for(float t = 0; t < 1.0; t += 1.0/steps) {
    	    float _1_t = 1 - t;
	        CurvePoint point;
	        point.V = 1.0/6 *_1_t * _1_t * _1_t * P0
                    + 1.0/6 * (3 * t * t * t - 6 * t * t + 4) * P1
                    + 1.0/6 * (-3 * t * t * t + 3 * t * t + 3 * t + 1) * P2
                    + 1.0/6 * t * t * t * P3;

	        point.T = -1/2.0 * _1_t * _1_t * P0
                    + 1/2.0 * (3 * t * t - 4 * t) * P1
                    + 1/2.0 * (-3 * t * t + 2 * t + 1) * P2
                    + 1/2.0 * t * t * P3;
	        point.T.normalize();
	        point.N = Vector3f::cross(Bi_1 , point.T);
	        point.N.normalize();
	        point.B = Vector3f::cross(point.T, point.N);
	        point.B.normalize();
	        Bi_1 = point.B;
	        curve.push_back(point);
	    }
	}
	//curve.push_back(curve[0]);
    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning bsp curve." << endl;

	return curve;
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

