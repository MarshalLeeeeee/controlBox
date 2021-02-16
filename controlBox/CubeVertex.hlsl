#include "Cube.hlsli"

VertexOut VS(VertexIn vIn) {
    VertexOut vOut;
	vOut.posH = mul(float4(vIn.posL, 1.0f), m_World); // local to world
    vOut.posH = mul(vOut.posH, m_View); // world to view
    vOut.posH = mul(vOut.posH, m_Proj); // view projects to camera plane
    vOut.color = vIn.color;
    return vOut;
}
