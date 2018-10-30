//--------------------------------------------------------------------------------------
// File: Tutorial022.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float moveX;
	float moveY;
	int objectNum;
	float roll;

	float4 color;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
float4 VS(float4 Pos : POSITION) : SV_POSITION
{
	if (objectNum == 3) {
		Pos.x += roll;
		return Pos;
	}
	Pos.x += moveX;
	Pos.y += moveY;

	return Pos;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
	if (objectNum > 0) {
		return color;
	}
	float a = moveX % 1.0f;
	float b = (moveY) % 1.0 ;
	float4 color = float4(a, b, 0.6f, 1.0f);
	color += Pos / 500.0;
	return color;

}
