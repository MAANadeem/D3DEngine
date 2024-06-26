#include "cameraclass.h"

CameraClass::CameraClass() {
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}
CameraClass::CameraClass(const CameraClass& other) {}
CameraClass::~CameraClass() {}
void CameraClass::SetPosition(float x, float y, float z) {
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}
void CameraClass::SetRotation(float x, float y, float z) {
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}
XMFLOAT3 CameraClass::GetPosition() {
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}
XMFLOAT3 CameraClass::GetRotation() {
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix) {
	//assigns our view matrix to the reference passed in
	viewMatrix = m_viewMatrix;
}

void CameraClass::Render() {
	XMFLOAT3 up, pos, lookAt;
	XMVECTOR upVec, posVec, lookAtVec;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//setup vector to point up and load it
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVec = XMLoadFloat3(&up);
	//setup pos vector and load it
	pos.x = m_positionX;
	pos.y = m_positionY;
	pos.z = m_positionZ;
	posVec = XMLoadFloat3(&pos);
	//setup camera's default view and load it
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVec = XMLoadFloat3(&lookAt);
	//set pitch (x), yaw (y) and roll (z) rotations in radians
	pitch = m_rotationX * oneRad;
	yaw = m_rotationY * oneRad;
	roll = m_rotationZ * oneRad;
	//make rotation matrix with above values
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	//transform lookAt and up vectors by rotation matrix
	//so view is correctly rotated at the origin
	lookAtVec = XMVector3Transform(lookAtVec, rotationMatrix);
	upVec = XMVector3Transform(upVec, rotationMatrix);
	// Translate the rotated camera position to the location of the viewer.
	lookAtVec = XMVectorAdd(posVec, lookAtVec);
	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(posVec, lookAtVec, upVec);
}