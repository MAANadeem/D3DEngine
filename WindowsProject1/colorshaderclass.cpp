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
	error = wcscpy_s(vsFile, 128, L"color.vs.hlsl");
	if (error != 0) return false;
	error = wcscpy_s(psFile, 128, L"color.ps.hlsl");
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
	result = D3DCompileFromFile(psFile, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
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

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) return false;

	// Release the vertex shader buffer and pixel shader buffer
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;

	return true;
}

void ColorShaderClass::ShutdownShader() {
	// Release the matrix constant buffer.
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	// Release the layout.
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}
	// Release the pixel shader.
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}
	// Release the vertex shader.
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
	char* compileErrors;
	unsigned long long bufferSize, i;
	std::ofstream fout;
	//get pointer to error text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	//get its length
	bufferSize = errorMessage->GetBufferSize();
	//open file to write error to
	fout.open("shader-error.txt");
	//write error
	for (i = 0; i < bufferSize; ++i) {
		fout << compileErrors[i];
	}
	//close file
	fout.close();
	//release
	errorMessage->Release(); errorMessage = 0;
	//show message box for compile errors
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* context, XMMATRIX world , XMMATRIX view, XMMATRIX proj) {
	HRESULT result; 
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	
	//DX11 demands matrices to be transposed before sending to shader
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	proj = XMMatrixTranspose(proj);

	//lock matrix buffer, set matrices, then unlock
	result = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = proj;
	context->Unmap(m_matrixBuffer, 0);
	
	//set updated matrix buffer in HLSL vs
	bufferNumber = 0;
	context->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* context, int indexCount) {
	//set vertex input layout
	context->IASetInputLayout(m_layout);
	//set vs and ps used to render
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);
	//render
	context->DrawIndexed(indexCount, 0, 0);
}