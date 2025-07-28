
// Geometry shader on raylib from: https://github.com/MikiZX1/raylib-Examples-unofficial-/blob/main/example_geometry_shader.c#L104
// discord msg I got it from just in case: https://discord.com/channels/426912293134270465/427518168995725317/1392981953220710493

#include <stdio.h>
#include <string>

#include "raylib.h"
#include "rlgl.h"
#include "glad.h"

#define RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION     "vertexPosition"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD     "vertexTexCoord"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD
#define RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL       "vertexNormal"      // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL
#define RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR        "vertexColor"       // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT      "vertexTangent"     // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2    "vertexTexCoord2"   // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2
#define RL_DEFAULT_SHADER_UNIFORM_NAME_MVP         "mvp"               // model-view-projection matrix
#define RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW        "matView"           // view matrix
#define RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION  "matProjection"     // projection matrix
#define RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL       "matModel"          // model matrix
#define RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL      "matNormal"         // normal matrix (transpose(inverse(matModelView))
#define RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR       "colDiffuse"        // color diffuse (base tint color, multiplied by texture color)
#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0  "texture0"          // texture0 (texture slot active 0)
#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1  "texture1"          // texture1 (texture slot active 1)
#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2  "texture2"          // texture2 (texture slot active 2)


char* LoadFileTextToStr(std::string fileName)
{
	FILE* file;
	fopen_s(&file,fileName.c_str(), "rb");
	if (!file)
	{
		printf("ERROR: Could not open file: %s\n", fileName.c_str());
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(size + 1);
	if (!buffer)
	{
		printf("ERROR: Could not allocate buffer for file: %s\n", fileName.c_str());
		fclose(file);
		return NULL;
	}

	fread(buffer, 1, size, file);
	buffer[size] = '\0'; // Null-terminate the string

	fclose(file);
	return buffer;
}

// Compile custom shader and return shader id
unsigned int MyrlCompileShader(const char* shaderCode, int type)
{
	GLuint shader = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCode, NULL);

	GLint success = 0;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		switch (type)
		{
		case GL_VERTEX_SHADER: printf("SHADER: [ID %i] Failed to compile vertex shader code\n", shader); break;
		case GL_FRAGMENT_SHADER: printf("SHADER: [ID %i] Failed to compile fragment shader code\n", shader); break;
		case GL_GEOMETRY_SHADER: printf("SHADER: [ID %i] Failed to compile geometry shader code\n", shader); break;
		default:
			break;
		}

		int maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength > 0)
		{
			int length = 0;
			char* log = (char*)RL_CALLOC(maxLength, sizeof(char));
			glGetShaderInfoLog(shader, maxLength, &length, log);
			printf("SHADER: [ID %i] Compile error: %s\n", shader, log);
			RL_FREE(log);
		}

		shader = 0;
	}
	else
	{
		switch (type)
		{
		case GL_VERTEX_SHADER: printf("SHADER: [ID %i] Vertex shader compiled successfully\n", shader); break;
		case GL_FRAGMENT_SHADER: printf("SHADER: [ID %i] Fragment shader compiled successfully\n", shader); break;
		case GL_GEOMETRY_SHADER:printf("SHADER: [ID %i] Geometry shader compiled successfully\n", shader); break;
		default: break;
		}
	}
#endif

	return shader;
}


unsigned int MyrlLoadShaderCode(const char* vsCode, const char* gsCode, const char* fsCode)
{
	unsigned int id = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
	unsigned int vertexShaderId = 0;
	unsigned int geometryShaderId = 0;
	unsigned int fragmentShaderId = 0;

	if (vsCode != NULL) vertexShaderId = MyrlCompileShader(vsCode, GL_VERTEX_SHADER);
	if (gsCode != NULL) geometryShaderId = MyrlCompileShader(gsCode, GL_GEOMETRY_SHADER);
	if (fsCode != NULL) fragmentShaderId = MyrlCompileShader(fsCode, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShaderId);
	glAttachShader(id, geometryShaderId);
	glAttachShader(id, fragmentShaderId);
	glLinkProgram(id);
	glUseProgram(id);

	if (id > 0) glDetachShader(id, vertexShaderId);
	glDeleteShader(vertexShaderId);
	if (id > 0) glDetachShader(id, geometryShaderId);
	glDeleteShader(geometryShaderId);
	if (id > 0) glDetachShader(id, fragmentShaderId);
	glDeleteShader(fragmentShaderId);

	if (id == 0)
	{
		printf("SHADER: Failed to load custom shader code, using default shader\n");
	}
	return id;
#endif
}


// Load shader from code strings and bind default locations
Shader MyLoadShaderFromMemory(const char* vsCode, const char* gsCode, const char* fsCode)
{
	Shader shader = { 0 };

	shader.id = MyrlLoadShaderCode(vsCode, gsCode, fsCode);

	// After shader loading, we TRY to set default location names
	if (shader.id > 0)
	{
		shader.locs = (int*)RL_CALLOC(RL_MAX_SHADER_LOCATIONS, sizeof(int));
		for (int i = 0; i < RL_MAX_SHADER_LOCATIONS; i++) shader.locs[i] = -1;

		// Get handles to GLSL input attribute locations
		shader.locs[SHADER_LOC_VERTEX_POSITION] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
		shader.locs[SHADER_LOC_VERTEX_TEXCOORD01] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
		shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2);
		shader.locs[SHADER_LOC_VERTEX_NORMAL] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL);
		shader.locs[SHADER_LOC_VERTEX_TANGENT] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT);
		shader.locs[SHADER_LOC_VERTEX_COLOR] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR);

		// Get handles to GLSL uniform locations (vertex shader)
		shader.locs[SHADER_LOC_MATRIX_MVP] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_MVP);
		shader.locs[SHADER_LOC_MATRIX_VIEW] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW);
		shader.locs[SHADER_LOC_MATRIX_PROJECTION] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION);
		shader.locs[SHADER_LOC_MATRIX_MODEL] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL);
		shader.locs[SHADER_LOC_MATRIX_NORMAL] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL);

		// Get handles to GLSL uniform locations (fragment shader)
		shader.locs[SHADER_LOC_COLOR_DIFFUSE] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR);
		shader.locs[SHADER_LOC_MAP_DIFFUSE] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0);  // SHADER_LOC_MAP_ALBEDO
		shader.locs[SHADER_LOC_MAP_SPECULAR] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1); // SHADER_LOC_MAP_METALNESS
		shader.locs[SHADER_LOC_MAP_NORMAL] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2);
	}

	return shader;
}