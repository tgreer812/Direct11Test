#include "pch.h"
#include "TriangularPyramid.h"


// Implement TriangularPyramid constructor
TriangularPyramid::TriangularPyramid() {
    // Define vertices for a triangular pyramid (tetrahedron) with a given color
    vertices.push_back(Vertex(0.0f, 1.0f, 0.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));  // Top vertex - Red
    vertices.push_back(Vertex(0.0f, 0.0f, 1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));  // Base - front vertex - Green
    vertices.push_back(Vertex(1.0f, 0.0f, 0.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));  // Base - right vertex - Blue
    vertices.push_back(Vertex(-1.0f, 0.0f, 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f))); // Base - left vertex - Yellow

    // Define indices for the pyramid's faces (assuming clockwise winding)
    // Front face
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    // Right face
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    // Left face
    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(1);

    // Base face
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(2);
}
