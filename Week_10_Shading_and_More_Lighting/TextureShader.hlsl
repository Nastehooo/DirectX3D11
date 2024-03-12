cbuffer ConstantBuffer
{
    Matrix WorldViewProjection;
	 
    Matrix World;
    float4 MaterialColour;
    float4 AmbientLightColour;
    float4 DirectionalLightColour;
    float4 DirectionalLightVector;
    float4 cameraPosition;
   // float4 lightVector; // the light's vector
  //  float4 lightColor; // the light's color
   // float4 AmbientLightColour; // the ambient light's color
    //float4 diffuseCoefficient; // The diffuse reflection cooefficient
    float4 specularCoefficient; // The specular reflection cooefficient
    float shininess; // The shininess factor
    float opacity; // The opacity (transparency) of the material. 0 = fully transparent, 1 = fully opaque
    float2 padding;
};

Texture2D Texture;
SamplerState ss;


struct VertexIn
{
    float3 InputPosition : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
    float4 OutputPosition : SV_POSITION;
    float4 PositionWS : TEXCOORD1;
    float4 NormalWS : TEXCOORD2;
    float2 TexCoord : TEXCOORD0;
	
};



VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.OutputPosition = mul(WorldViewProjection, float4(vin.InputPosition, 1.0f));
    vout.PositionWS = mul(World, float4(vin.InputPosition, 1.0f));
    vout.NormalWS = float4(mul((float3x3) World, vin.Normal), 1.0f);
    vout.TexCoord = vin.TexCoord;


   /* vout.OutputPosition = mul(WorldViewProjection, float4(vin.InputPosition, 1.0f));
    float4 norm = (mul(World, float4(vin.Normal, 0.0f)));
	
	// Dot product of adjusted normal and vector back to the light source
    float diffuseLight = saturate(normalize(dot(norm, -DirectionalLightVector)));
   
	// Calculate the amount of diffuse light hitting the vertex
	// Normalize it and ensure it's between 0 and 1
    float4 lighting = (DirectionalLightColour * diffuseLight);

	// Add ambient light and ensure each component is between 0 and 1
    lighting += AmbientLightColour;
    lighting = saturate(lighting);
    vout.Colour = lighting * MaterialColour;
    ;*/

    
    
   // vout.OutputPosition = mul(completeTransformation, float4(vin.Position, 1.0f));
    //vout.PositionWS = mul(worldTransformation, float4(vin.Position, 1.0f));
    //vout.NormalWS = float4(mul((float3x3) worldTransformation, vin.Normal), 1.0f);
    //vout.TexCoord = vin.TexCoord;
    return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
    //float4 colour = float4(1.0f, 1.0f, 1.0f, 1.0f);
	//return colour;
	
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
    return color * Texture.Sample(ss, pin.TexCoord);;
   // return pin.Colour * Texture.Sample(ss, pin.TexCoord);
}
