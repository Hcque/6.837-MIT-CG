#include "SkeletalModel.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}


void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.

	vector<Joint*> jointList;
	vector<int> pie; // index of parents

	cout << "============" << endl;
	cout <<  "read skel file: " << filename << endl;
	ifstream input;
	input.open(filename);
	string buf;

	while ( getline(input, buf) ) {
		cout << buf << endl;
		stringstream ss(buf);
		float x, y, z;
		int i;
		ss >> x >> y >> z >> i;

		Joint *j = new Joint;
		j->transform = Matrix4f::translation(x, y, z);

		jointList.push_back(j);
		pie.push_back(i);
		// if (i == 0)
		// 	m_rootJoint = j;
	}
	for (int i = 0; i < pie.size(); i++){
		if (  pie[i] != -1) {
			cout << "parent: " << pie[i]  << " chi:" << i << endl;
			jointList[ pie[i] ]->children.push_back( jointList[i] );
		}
	}
	m_joints = jointList;
	m_rootJoint = jointList.front();
}

// HELPER FOR DRAW JOINTS
void recDrawJoints(Joint *j, MatrixStack m_matrixStack) {
	if (j == nullptr)
		return;
	
	m_matrixStack.push( j->transform );
	glLoadMatrixf(m_matrixStack.top());
	glutSolidSphere(0.055f, 12, 12);
	// cout << "draw..."  << j << endl;

	for (Joint *child : j->children){
		recDrawJoints(child, m_matrixStack);
	}
	m_matrixStack.pop();
}


void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
	recDrawJoints(m_rootJoint, m_matrixStack);
}

void dfsDrawSkeleton(Joint *j, MatrixStack m_matrixStack, Joint *m_root){
	if (j == nullptr)
		return;
	m_matrixStack.push(j->transform);
	for (Joint *c: j->children){
		dfsDrawSkeleton(c, m_matrixStack, m_root);
	}
	m_matrixStack.pop();
	if (j == m_root) 
		return;

	Matrix4f transM = Matrix4f::translation(0.0,0.0,0.5);
	float dist = j->transform.getCol(3).xyz().abs();

	Matrix4f distM = Matrix4f( 0.05f, 0.0, 0.0, 0.0,
								0.0, 0.05f, 0.0, 0.0,
								0.0, 0.0, dist, 0.0,
								0.0, 0.0, 0.0, 1.0);
	Vector3f zz = j->transform.getCol(3).xyz().normalized();
	Vector3f rnd = Vector3f(0.0, 0.0, 1.0);
	Vector3f yy  = Vector3f::cross(zz, rnd).normalized();
	Vector3f xx  = Vector3f::cross(yy, zz).normalized();
	Matrix4f corrM = Matrix4f(xx.x(), yy.x(), zz.x(), 0.0,
							 xx.y(), yy.y(), zz.y(), 0.0,
							 xx.z(), yy.z(), zz.z(), 0.0,
							 0.0, 0.0, 0.0, 1.0);
	m_matrixStack.push(corrM);
	m_matrixStack.push(distM);
	m_matrixStack.push(transM);
	glLoadMatrixf(m_matrixStack.top());

	glutSolidCube( 1.0f );
	m_matrixStack.pop();
	m_matrixStack.pop();
	m_matrixStack.pop();

}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
	dfsDrawSkeleton(m_rootJoint, m_matrixStack, m_rootJoint);

}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
	Matrix4f rxM = Matrix4f::rotateX(rX); 
	Matrix4f ryM = Matrix4f::rotateY(rY); 
	Matrix4f rzM = Matrix4f::rotateZ(rZ); 
	Matrix4f rota = rxM * ryM * rzM;
	m_joints[jointIndex]->transform.setSubmatrix3x3( 0, 0, rota.getSubmatrix3x3(0, 0) );
}

void dfsWorldToJoint(Joint *j, MatrixStack m_stack){
	if (j == nullptr) return;

	m_stack.push(j->transform);
	j->currentJointToWorldTransform = m_stack.top();
	for (Joint *c : j->children){
		dfsWorldToJoint(c, m_stack);
	}
	m_stack.pop();
}

void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	m_matrixStack.clear();
	dfsWorldToJoint(m_rootJoint, m_matrixStack);
}

void dfsCurToWorld(Joint *j, MatrixStack m_stack){
	if (j == nullptr) return;

	m_stack.push(j->transform);
	j->currentJointToWorldTransform = m_stack.top().inverse();
	for (Joint *c : j->children){
		dfsCurToWorld(c, m_stack);
	}
	m_stack.pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	m_matrixStack.clear();
	dfsCurToWorld(m_rootJoint, m_matrixStack);
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.

	cout << "update Mesh " << endl;
	vector<Vector3f> newV ;// = new vector<Vector3f>();
	for (int i = 0; i < m_mesh.currentVertices.size(); i++){
		Vector4f sum_(0.0, 0.0, 0.0, 0.0);
		for (int j = 0; j < m_mesh.attachments[i].size(); j++){
			Joint *joint = m_joints[j+1];
			Vector3f v3 = m_mesh.bindVertices[i];
			sum_ = sum_ +  m_mesh.attachments[i][j] * 
			(joint->currentJointToWorldTransform * joint->bindWorldToJointTransform 
			* Vector4f(v3, 1.0f) );
		}
		newV.push_back(sum_.xyz());
	}
	m_mesh.currentVertices = newV;
}

