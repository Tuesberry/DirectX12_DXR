struct VS_OUTPUT
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VS_OUTPUT VSMain(float4 pos : POSITION, float4 color : COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.position = pos;
    output.color = color;
    
    return output;
}
