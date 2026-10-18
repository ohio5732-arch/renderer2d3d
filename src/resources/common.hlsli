#ifndef COMMON_HLSLI
#define COMMON_HLSLI

struct VSIn {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4 color : COLOR;
};

struct VSOut {
  float4 position : SV_POSITION;
  float3 normal : NORMAL;
  float4 color : COLOR;
  float3 worldPos : TEXCOORD0;
};

cbuffer ObjectBuffer : register(b0) {
  float4x4 u_transform;
  float4x4 u_world;
  float4x4 u_normalMatrix;
  float u_time;
  float3 u_lightDir;
  float3 u_cameraPos;
};

#endif
