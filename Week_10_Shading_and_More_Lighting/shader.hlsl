cbuffer ConstantBuffer 
{
	Matrix		WorldViewProjection;
	 
	Matrix		World;
	float4		MaterialColour;
    float4		AmbientLightColour;
    float4		DirectionalLightColour;
    float4		DirectionalLightVector;
    float4 cameraPosition;

    float4 specularCoefficient; // The specular reflection cooefficient
    float shininess; // The shininess factor
    float opacity; // The opacity (transparency) of the material. 0 = fully transparent, 1 = fully opaque
    float2 padding;
};



struct VertexIn
{
	float3 InputPosition : POSITION;
	float3 Normal        : NORMAL;
};

struct VertexOut
{
	float4 OutputPosition	: SV_POSITION;
    float4 PositionWS : TEXCOORD1;
    float4 NormalWS : TEXCOORD2;
};



VertexOut VS(VertexIn vin)
{
	VertexOut vout;
  
    vout.OutputPosition = mul(WorldViewProjection, float4(vin.InputPosition, 1.0f));
    vout.PositionWS = mul(World, float4(vin.InputPosition, 1.0f));
    vout.NormalWS = float4(mul((float3x3) World, vin.Normal), 1.0f);
  //  vout.TexCoord = vin.TexCoord;
	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
    float4 viewDirection = normalize(cameraPosition - pin.PositionWS);
    float4 directionToLight = normalize(-DirectionalLightVector);

	// Calculate diffuse lighting
    float4 adjustedNormal = normalize(pin.NormalWS);
    float NdotL = max(0, dot(adjustedNormal, directionToLight));
    float4 diffuse = saturate(DirectionalLightColour * NdotL * MaterialColour);

	// Calculate specular component
    float4 R = 2 * NdotL * adjustedNormal - directionToLight;
    float RdotV = max(0, dot(R, viewDirection));
    float4 specular = saturate(DirectionalLightColour * pow(RdotV, shininess) * specularCoefficient);

	// Calculate ambient lighting
    float4 ambientLight = AmbientLightColour * MaterialColour;

	// Combine all components
    float4 color = saturate((ambientLight + diffuse + specular)); // * Texture.Sample(ss, input.TexCoord)
    if (opacity < 1.0f)
    {
        color.a = opacity;
    }
    return color;
}

