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
	return Pos;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION) : SV_Target
{

	if (cb_d)
	{
		if (cb_a % 4 == 0) {
			return float4(.3, .6, 1, 1);
		}
		else if (cb_a % 4 == 1) {
			return float4(.6, .3, .6, 1);
		}
		else if (cb_a % 4 == 2) {
			return float4(.1, .6, .3, 1);
			}
		else if (cb_a % 4 == 3) {
			return float4(1, 1, .6, 1);
			//return float4(.3, .6, 1, 1);
		}

	}
	return float4(1, 0, 0, 1); //rgba (transparency not enabled)
}
