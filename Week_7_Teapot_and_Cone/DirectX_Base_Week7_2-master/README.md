# DirectX_Base_7Week
```// Assuming this code is within the function where you're calculating normals in Geometry.h

// Create an array to store the contributing counts for each vertex
int contributingCounts[sizeof(vertices) / sizeof(vertices[0])];

// Initialize the contributing counts array to 0 for each vertex
memset(contributingCounts, 0, sizeof(contributingCounts));

// Loop through each polygon and update the contributing counts and calculate the polygon normal
for (UINT i = 0; i < sizeof(indices) / sizeof(indices[0]); i += 3)
{
    // ... (rest of the code as previously mentioned)
}

// Loop through each vertex and divide the summed vertex normals by the contributing counts, then normalize the resulting normal vector
for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); ++i)
{
    if (contributingCounts[i] > 0)
    {
        vertices[i].Normal /= contributingCounts[i];
        vertices[i].Normal = XMVector3Normalize(vertices[i].Normal);
    }
}
```
