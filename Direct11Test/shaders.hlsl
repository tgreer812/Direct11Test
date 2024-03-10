cbuffer ConstantBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

// Define a structure for vertex shader input
struct VSInput
{
    float3 position : POSITION; // Vertex position
    float4 color : COLOR; // Vertex color
};

// Define a structure for pixel shader input (and vertex shader output)
struct PSInput
{
    float4 position : SV_POSITION; // Position in screen space
    float4 color : COLOR; // Color to be interpolated across the primitive
};

// Vertex Shader
PSInput VShader(VSInput input)
{
    PSInput output;
    // Transform the vertex position from model space to clip space
    output.position = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Pass the vertex color through unmodified
    output.color = input.color;
    return output;
}

// Pixel Shader
float4 PShader(PSInput input) : SV_TARGET
{
    // Output the color interpolated across the primitive
    return input.color;
}
