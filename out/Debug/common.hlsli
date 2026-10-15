#ifndef COMMON_HLSLI
#define COMMON_HLSLI

struct VSIn {
  float3 position : POSITION;
};

struct VSOut {
  float4 position : SV_POSITION;
};

#endif
