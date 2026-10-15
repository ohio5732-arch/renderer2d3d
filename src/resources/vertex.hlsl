#include "common.hlsli"

VSOut VSMain(VSIn input) {
  VSOut output;
  output.position = mul(float4(input.position, 1.0f), u_transform);
  output.color = input.color;
  return output;
}
