
#version 420 core

layout (location = 0) in int in_node_index; // Location 0 is conventional for vertex data

// Output to Geometry Shader
out GS_DATA {
    int nodeIndex;
} gs_out;

void main() {

	// We don't need to set gl_Position here as the GS will handle all vertex generation.
    // We just pass the index through.
	
    gs_out.nodeIndex = in_node_index; // Pass the index of the segment to process
}