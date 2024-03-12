#include "TeapotNode.h"
//#include "Geometry.h"
#include "GeometricObject.h"


#define ShaderFileName		L"shader.hlsl"
#define VertexShaderName	"VS"
#define PixelShaderName		"PS"

// Format of the constant buffer. This must match the format of the
// cbuffer structure in the shader

struct teaCBuffer
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

struct teapotVertex
{
	Vector3		Position;
	Vector3		Normal;
};

D3D11_INPUT_ELEMENT_DESC teapotvertexDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}

};



bool TeapotNode::Initialise()
{

	_device = DirectXFramework::GetDXFramework()->GetDevice();
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();
	if (_device.Get() == nullptr || _deviceContext.Get() == nullptr)
	{
		return false;
	}

	ComputeTeapot(vertices, indices, 1.0f);

	GenerateVertexNormals(vertices, indices);
	BuildGeometryBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();


	return true;
}



void TeapotNode::Render()
{
	// Calculate the world x view x projection transformation
	Matrix projectionTransformation = DirectXFramework::GetDXFramework()->GetProjectionTransformation();
	Matrix viewTransformation = DirectXFramework::GetDXFramework()->GetViewTransformation();



	//Matrix World = _worldTransformation;
	teaCBuffer constantBuffer;
	constantBuffer.World = _cumulativeWorldTransformation;

	constantBuffer.WorldViewProjection = _cumulativeWorldTransformation * viewTransformation * projectionTransformation;
	constantBuffer.MaterialColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	constantBuffer.AmbientLightColour = _ambientColour;

	constantBuffer.DirectionalLightVector = Vector4(-1.0f, -1.0f, 1.0f, 0.0f); // Direction of the light
	constantBuffer.DirectionalLightColour = Vector4(Colors::Linen); // Color of the light

	constantBuffer.SpecularColour = Vector4(0.1f, 0.1f, 0.1f, 0.1f);
	constantBuffer.Shininess = 1.0f;
	constantBuffer.Opacity = 1.0f;

	// Update the constant buffer. Note the layout of the constant buffer must match that in the shader
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBuffer, 0, 0);

	// Now render the cube
	// Specify the distance between vertices and the starting point in the vertex buffer
	UINT stride = sizeof(teapotVertex);
	UINT offset = 0;
	// Set the vertex buffer and index buffer we are going to use
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	_deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

	// Specify the layout of the polygons (it will rarely be different to this)
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Specify the layout of the input vertices.  This must match the layout of the input vertices in the shader
	_deviceContext->IASetInputLayout(_layout.Get());

	// Specify the vertex and pixel shaders we are going to use
	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);


	// Now draw the first cube
	_deviceContext->DrawIndexed(indices.size(), 0, 0);

}

void TeapotNode::BuildGeometryBuffers()
{
	// This method uses the arrays defined in Geometry.h
	// 
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor = { 0 };
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(teapotVertex) * vertices.size();
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData = { 0 };
	vertexInitialisationData.pSysMem = vertices.data();
	// and create the vertex buffer
	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor = { 0 };
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * indices.size();
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = indices.data();

	// and create the index buffer
	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void TeapotNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(ShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		VertexShaderName, "vs_5_0",
		shaderCompileFlags, 0,
		_vertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreateVertexShader(_vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), NULL, _vertexShader.GetAddressOf()));

	// Compile pixel shader
	hr = D3DCompileFromFile(ShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		PixelShaderName, "ps_5_0",
		shaderCompileFlags, 0,
		_pixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreatePixelShader(_pixelShaderByteCode->GetBufferPointer(), _pixelShaderByteCode->GetBufferSize(), NULL, _pixelShader.GetAddressOf()));
}

void TeapotNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it. The vertexDesc array is
	// defined in Geometry.h

	ThrowIfFailed(_device->CreateInputLayout(teapotvertexDesc, ARRAYSIZE(teapotvertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void TeapotNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(teaCBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void TeapotNode::GenerateVertexNormals(vector<ObjectVertexStruct>& vertices, vector<UINT>& indices)
{

	// Create an array for contributing counts
	vector<int>contributingCounts(vertices.size(), 0);
	//int contributingCounts[ARRAYSIZE(vertices)] = { 0 };

	// Loop through polygons
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		//calc the normal for polygon
		// Get the 3 indices of the vertices that make up the polygon
		UINT index0 = indices[i];
		UINT index1 = indices[i + 1];
		UINT index2 = indices[i + 2];


		// Get the vertices for those indices

		Vector3 vertex0 = vertices[index0].Position;
		Vector3 vertex1 = vertices[index1].Position;
		Vector3 vertex2 = vertices[index2].Position;
		//XMVECTOR point1 = XMVectorSet(vertex1.Position.x, vertex1.Pos.y, vertex1.Pos.z, 0.0f);

		// Calculate the polygon normal

		Vector3 vectorA = vertex1 - vertex0;
		Vector3 vectorB = vertex2 - vertex0;

		//Vector3 vectorB = vertex2 - vertex0;

		// Calculate the cross product of vectorA and vectorB to get the normal
		Vector3 polygonNormal = vectorA.Cross(vectorB);




		// Add the polygon normal to the vertex normal for each of the 3 vertices
		vertices[index0].Normal += polygonNormal;
		vertices[index1].Normal += polygonNormal;
		vertices[index2].Normal += polygonNormal;

		// Add 1 to the contributing count for each of the vertices
		contributingCounts[index0] += 1;
		contributingCounts[index1] += 1;
		contributingCounts[index2] += 1;
	}

	// Loop through vertices
	for (size_t i = 0; i < (vertices.size()); i += 1)
	{
		if (contributingCounts[i] > 0)
		{
			vertices[i].Normal /= (contributingCounts[i]);
			vertices[i].Normal.Normalize();
		}
		// Divide the summed vertex normals by the number of times they were contributed to
		// Normalize the resulting normal vector

	}


}
