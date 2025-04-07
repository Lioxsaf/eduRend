
Texture2D texDiffuse : register(t0);

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
    float4 WorldPos : POSITION;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------


cbuffer LightCameraBuffer : register(b0)
{
    float4 cameraPos; // Camera position in world space
    float4 lightColorPos; // RGB light color + optional intensity
    float4 lightPos; // Light position in world space
};

cbuffer MaterialBuffer : register(b2)
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float shininess;
    float3 padding; // 16-byte alignment
};


float4 PS_main(PSIn input) : SV_Target
{
    // Normalize interpolated inputs
    float3 N = normalize(input.Normal);
    float3 L = normalize(-lightPos.xyz - input.WorldPos.xyz);
    float3 V = normalize(cameraPos.xyz - input.WorldPos.xyz);
    
    float3 R = reflect(L, N);
    
    float3 ambient = ambientColor.xyz * lightColorPos.xyz;
    
    float diff = max(dot(L, N), 0.0f);
    float3 diffuse = diffuseColor.xyz * diff * lightColorPos.xyz;
    
    float spec = pow(max(dot(R, V), 0.0f), shininess);
    float3 specular = specularColor.xyz * spec * lightColorPos.xyz;
    
    float2 scaledTextCoord = input.TexCoord;
    
    float3 finalColor = ambient + diffuse + specular;

    return float4(finalColor, 1.0f);

}