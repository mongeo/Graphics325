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
	//Pos.x += cb_c*0.2f;	
	float4 position = Pos;
	position += float4(0.0, 0.0, 0.0, 0.0);
	return position;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION) : SV_Target{
	float4 color = float4(0.1f, 0.4f, 0.7f, 1.0f);
	color += Pos / 987;
//return float4(1.0f, cb_a,cb_b, cb_c);    // Yellow, with Alpha = 1
	return color;
}
