#ifndef COMMON_HLSLI
#define COMMON_HLSLI

struct VSIn {
  float3 position : POSITION;
  float4 color : COLOR;
};

struct VSOut {
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

cbuffer ObjectBuffer : register(b0) {
  float4x4 u_transform;
  float u_time;
};

#endif
