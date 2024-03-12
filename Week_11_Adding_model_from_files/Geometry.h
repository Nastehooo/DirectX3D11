#pragma once

#define ShaderFileName		L"shader.hlsl"
#define TextureShaderFileName		L"TextureShader.hlsl"
#define VertexShaderName	"VS"
#define PixelShaderName		"PS"
#define TextureName			L"woodbox.bmp"

// Format of the constant buffer. This must match the format of the
// cbuffer structure in the shader
/*
struct CBuffer
{
	Matrix		WorldViewProjection;
	Matrix		World;
	Vector4		MaterialColour;
	Vector4		AmbientLightColour;
	Vector4		DirectionalLightColour;
	Vector4		DirectionalLightVector;
	Vector4		cameraPosition;
	Vector4		SpecularColour;
	float		Shininess;
	float		Opacity;
	float		Padding[2];

};
*/


// Structure of a single vertex.  This must match the
// structure of the input vertex in the shader




