#pragma once
#include <vector>
#include "pch.h"

// Vertex structure for position
struct Vertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color; // Added color

	Vertex(float x, float y, float z, DirectX::XMFLOAT4 col) : position(x, y, z), color(col) {}
};

class TriangularPyramid
{
	public:
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

		TriangularPyramid();


};

