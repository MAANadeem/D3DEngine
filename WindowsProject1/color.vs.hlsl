///Globals///
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

///Typedefs///
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float color : COLOR;
};

///Vertex Shader///
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
	
	//4 unit pos vector (xyzw) for proper matrix cals
	input.position.w = 1.0f;
	
	//find vertex pos against all 3 matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
	//store input color
	output.color = input.color;
	
	return output;
}