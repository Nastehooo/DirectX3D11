cbuffer ConstantBuffer 
{
	Matrix		WorldViewProjection;
	Matrix		World;
	float4		MaterialColour;
    float4		AmbientLightColour;
    float4		DirectionalLightColour;
    float4		DirectionalLightVector;
    float4      cameraPosition;
    float4      specularCoefficient; 
    float       shininess; 
    float       opacity; 
    float2      padding;
};

Texture2D Texture;
SamplerState ss;

struct VertexIn
{
	float3 InputPosition : POSITION;
	float3 Normal        : NORMAL;
    float2 TexCoord     : TEXCOORD;
};

struct VertexOut
{
	float4 OutputPosition	: SV_POSITION;
    float4 PositionWS : TEXCOORD1;
    float4 NormalWS : TEXCOORD0;
    float2 TexCoord : TEXCOORD3;
    

};



VertexOut VS(VertexIn vin)
{
	VertexOut vout;
  
    vout.OutputPosition = mul(WorldViewProjection, float4(vin.InputPosition, 1.0f));
    vout.PositionWS = mul(World, float4(vin.InputPosition, 1.0f));
    vout.NormalWS = float4(mul((float3x3) World, vin.Normal), 1.0f);
    vout.TexCoord = vin.TexCoord;
	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
    float4 viewDirection = normalize(cameraPosition - pin.PositionWS);
    float4 normal = normalize(pin.NormalWS);
    float NdotL = max(0, dot(normal, normalize(-DirectionalLightVector)));
    float4 R = 2 * NdotL * normal - normalize(-DirectionalLightVector);
    float RdotV = max(0, dot(R, viewDirection));
    
    // Calculate diffuse, specular, ambient lighting
    float4 diffuse = saturate(DirectionalLightColour * NdotL * MaterialColour);
    float4 specular = saturate(DirectionalLightColour * pow(RdotV, shininess) * specularCoefficient);
    float4 ambientLight = AmbientLightColour * MaterialColour;
    float4 diffuseColor = Texture.Sample(ss, pin.TexCoord) * MaterialColour;


	// Combine all components
    float4 color = saturate((ambientLight + diffuse + specular));
    
    if (opacity < 1.0f)
    {
        color.a = opacity;
    }
    return color;
}


