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
float4 VS(float4 Pos : POSITION) : SV_POSITION {
	Pos.x += cb_c * 0.2;
	
float4 position = Pos;
position += float4(0.2, 0.0, 0.2, 0.0);
	return Pos;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION) : SV_Target{
	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	color += Pos / 500.0;
return float4(1.0f, cb_a,cb_b, cb_c);    // Yellow, with Alpha = 1
	//return color;
}
