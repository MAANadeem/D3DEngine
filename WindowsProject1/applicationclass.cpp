#include "applicationclass.h"

ApplicationClass::ApplicationClass(){
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}
ApplicationClass::ApplicationClass(const ApplicationClass& other) {}
ApplicationClass::~ApplicationClass() {}

bool ApplicationClass::Init(int screenWidth, int screenHeight, HWND hwnd){
	bool result;

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;
	result = m_Direct3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//Create and init camera, model, and color shader objects
	m_Camera = new CameraClass;
	m_Model = new ModelClass;
	m_ColorShader = new ColorShaderClass;
	
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	result = m_Model->Init(m_Direct3D->GetDevice());
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	result = m_ColorShader->Init(m_Direct3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
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
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model; m_Model = 0;
	}
	if (m_ColorShader) {
		m_ColorShader->Shutdown();
		delete m_ColorShader; m_ColorShader = 0;
	}
	if (m_Camera) {
		delete m_Camera; m_Camera = 0;
	}
}

bool ApplicationClass::Frame(){
	static float spin = 0.0f;
	bool result;

	spin -= 0.0174532925f * 0.25f;
	if (spin < 0.0f) spin += 360.0f;
	
	result = Render(spin);

	return result;
}

bool ApplicationClass::Render(float rotation){
	XMMATRIX world, view, proj;
	bool result;

	//Clear buffers to begin scene
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Generate view matrix via camera's position
	m_Camera->Render();
	//copy matrices
	m_Direct3D->GetWorldMatrix(world);
	m_Camera->GetViewMatrix(view);
	m_Direct3D->GetProjectionMatrix(proj);

	//rotate world matrix with rotation value
	//world = XMMatrixRotationY(rotation);

	//put model vertex and index buffers on graphics pipeline
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//Render model with color shader
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), world, view, proj);
	if (!result) return false;

	//Present rendered scene to screen
	m_Direct3D->EndScene();

	return true;
}
