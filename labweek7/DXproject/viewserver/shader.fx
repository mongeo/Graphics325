//--------------------------------------------------------------------------------------
// File: Tutorial022.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float cb_a;
	float cb_b;
	float cb_c;
	float cb_d;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
float4 VS(float4 Pos : POSITION) : SV_POSITION
{

	return Pos+cb_a;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
	if (cb_d)
	{
		return float4(1, 1, 1, 1);
	}
	return float4(1, 0, 0, 1); //rgba (transparency not enabled)
}
