#include "applicationclass.h"

ApplicationClass::ApplicationClass(){
	m_Direct3D = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other){
}


ApplicationClass::~ApplicationClass(){
}


bool ApplicationClass::Init(int screenWidth, int screenHeight, HWND hwnd){
	bool result;

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	return true;
}


void ApplicationClass::Shutdown(){
	if (m_Direct3D) {
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}


bool ApplicationClass::Frame(){
	bool result;
	result = Render();

	return result;
}


bool ApplicationClass::Render(){
	//Clear buffers to begin scene
	m_Direct3D->BeginScene(0.0f, 1.0f, 1.0f, 1.0f);

	//Present rendered scene to screen
	m_Direct3D->EndScene();

	return true;
}
