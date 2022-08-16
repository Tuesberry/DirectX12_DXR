struct VS_OUTPUT
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    return input.color;
}