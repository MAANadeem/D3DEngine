#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}
ColorShaderClass::ColorShaderClass(const ColorShaderClass& other) {}
ColorShaderClass::~ColorShaderClass() {}

bool ColorShaderClass::Init(ID3D11Device* device, HWND hwnd) {
	bool result;
	wchar_t vsFile[128];
	wchar_t psFile[128];
	int error;

	//set filenames for vs and ps
	error = wcscpy_s(vsFile, 128, L"../Shader Files/color.vs.hlsl");
	if (error != 0) return false;
	error = wcscpy_s(vsFile, 128, L"../Shader Files/color.ps.hlsl");
	if (error != 0) return false;

	//Init shaders
	result = InitShader(device, hwnd, vsFile, psFile);
	
	return result;
}

void ColorShaderClass::Shutdown() {
	//shutdown shaders and related objects
	ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* context, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX proj) {
	bool result;

	//set shader parameters
	result = SetShaderParameters(context, world, view, proj);
	if (!result) return false;

	//Render prepared buffers with shader
	RenderShader(context, indexCount);
	return true;
}

bool ColorShaderClass::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFile, WCHAR* psFile) {
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//Compile vs code
	result = D3DCompileFromFile(vsFile, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		//failed compilation condition
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, vsFile);
		//shader file not found condition
		else MessageBox(hwnd, vsFile, L"Mising Shader File", MB_OK);
		return false;
	}
	result = D3DCompileFromFile(psFile, NULL, NULL, "ColorPixelShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		//failed compilation condition
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, psFile);
		//shader file not found condition
		else MessageBox(hwnd, psFile, L"Mising Shader File", MB_OK);
		return false;
	}

	//creating shader objects from buffers
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) return false;
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) return false;


}
