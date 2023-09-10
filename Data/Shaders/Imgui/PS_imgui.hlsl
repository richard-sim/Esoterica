struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

sampler sampler_llr;
Texture2D texture0;

float4 main( PS_INPUT input ) : SV_TARGET
{
    float4 out_col = input.col * texture0.Sample(sampler_llr, input.uv);
    return out_col;
}