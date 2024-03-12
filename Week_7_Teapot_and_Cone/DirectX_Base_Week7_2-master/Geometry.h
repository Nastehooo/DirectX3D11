#pragma once

#define ShaderFileName		L"shader.hlsl"
#define VertexShaderName	"VS"
#define PixelShaderName		"PS"




// Format of the constant buffer. This must match the format of the
// cbuffer structure in the shader

struct CBuffer
{
	Matrix		WorldViewProjection;
	Matrix		World;
	Vector4		MaterialColour;
	Vector4		AmbientLightColour;
	Vector4		DirectionalLightColour;
	Vector4		DirectionalLightVector;
};

// Structure of a single vertex.  This must match the
// structure of the input vertex in the shader




// The description of the vertex that is passed to CreateInputLayout.  This must
// match the format of the vertex above and the format of the input vertex in the shader

D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// This example uses hard-coded vertices and indices for a cube. Usually, you will load the verticesa and indices from a model file. 
// We will see this later in the module. 



