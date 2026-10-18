#include "common.hlsli"

float4 PSMain(VSOut input) : SV_TARGET {
  float3 N = normalize(input.normal);
  float3 L = normalize(u_lightDir);
  float3 V = normalize(u_cameraPos - input.worldPos);
  float3 H = normalize(L + V);
  float diffuse = saturate(dot(N, L));
  float ambient = 0.2;
  float shininess = 32.0;
  float specular = (diffuse > 0.0) ? pow(saturate(dot(N, H)), shininess) : 0.0;

  float3 specColor = float3(1.0, 1.0, 1.0) * 0.5;
  float3 lit = input.color.rgb * (ambient + diffuse) + specColor * specular;
  return float4(saturate(lit), input.color.a);
}
