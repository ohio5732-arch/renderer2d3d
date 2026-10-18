#include "common.hlsli"

VSOut VSMain(VSIn input) {
  VSOut output;
  output.position = mul(u_transform, float4(input.position, 1.0f));
  output.normal = mul((float3x3)u_normalMatrix, input.normal);
  output.worldPos = mul(u_world, float4(input.position, 1.0f)).xyz;
  output.color = input.color;
  return output;
}
