#include "BasicShader.hlsli"

[maxvertexcount(3)]
void main(
	triangle GSInput input[3],
	inout TriangleStream< GSOutput > output
)
{
	[unroll]
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.position = input[i].position;
		element.worldPosition = input[i].worldPosition;
		element.worldNormal = input[i].worldNormal;
		element.texCoord = input[i].texCoord;
		output.Append(element);
	}
}