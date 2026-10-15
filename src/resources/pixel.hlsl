#include "common.hlsli"

float4 PSMain(VSOut input) : SV_TARGET { return input.color; }
