//cbuffer ConstantBuffer : register(b0) {
//	matrix m_World;
//	matrix m_View;
//	matrix m_Proj;
//}

cbuffer WorldBuffer : register(b0) {
	matrix m_World;
}

cbuffer ViewBuffer : register(b1) {
	matrix m_View;
}

cbuffer ProjBuffer : register(b2) {
	matrix m_Proj;
}

struct VertexIn {
	float3 posL : POSITION;
	float4 color : COLOR;
};

struct VertexOut {
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};
