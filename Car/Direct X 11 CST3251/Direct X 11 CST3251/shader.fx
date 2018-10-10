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

	Pos.x += cb_c*0.2f;	
	float4 position = Pos;
	position += float4(0.0f, 0.0f, 0.0f, 6.0f);
	return position;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION) : SV_Target{
	float4 color = (Pos)  / 500;;
	//color +=  float4(cb_b, cb_a,cb_a, 1.0f);    // Yellow, with Alpha = 1
	return color;
}
