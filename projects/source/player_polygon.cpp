
#include "player_polygon.h"

#include <math.h>
#include <stdio.h>

#include "raylib.h"
#include "rlgl.h"

#include "constants.h"
#include "FIFO.h"

void DrawCircle(int radius,int x, int y)
{
    DrawCircle(x, y, radius, RED);
}

void DrawCurrentPolygon(Coord *stack)
{
  if(stack!=nullptr && stack->nextCoord!=NULL)
  {
    Coord *aux;
    for(aux=stack;aux->nextCoord!=nullptr;aux=aux->nextCoord)
    {
      DrawLine(aux->x, aux->y, aux->nextCoord->x, aux->nextCoord->y, RED);
      if((aux->nextCoord)->nextCoord==nullptr)
      {
        DrawCircle(KMarginToClosePoly,aux->nextCoord->x,aux->nextCoord->y);
      }
    }
  }
}

void DrawCurrentPolygonOnlyLines(Coord* stack)
{
	if (stack != nullptr && stack->nextCoord != NULL)
	{
		Coord* aux;
		for (aux = stack; aux->nextCoord != nullptr; aux = aux->nextCoord)
		{

            if (aux->intersected && aux->nextCoord->intersected)
            {
				DrawLine(aux->x, aux->y, aux->nextCoord->x, aux->nextCoord->y, GREEN);
            }
            else {
                DrawLine(aux->x, aux->y, aux->nextCoord->x, aux->nextCoord->y, RED);
            }
            //DrawCircle(aux->x, aux->y, 5, GREEN);
		}
	}
}

bool IsPolygonClosed(Coord *stack, int x,int y)
{
  if(stack!=nullptr && stack->nextCoord!=nullptr)
  {
    if(GetStackCount(stack) > KMarginToClosePoly / 2)
    {
      Coord *aux = BotStack(stack,0);
      if(abs(aux->x - x) <= KMarginToClosePoly && abs (aux->y - y) <= KMarginToClosePoly) return true;
      else return false;
    }
    else return false;

  }
  else return false;
}

// TMP LOCATION FOR THIS 

#include <cmath>

const int RECTANGLE_WIDE = 10;

void ComputeRectangle(int start[2], int end[2], int topLeft[2], int topRight[2], int botLeft[2], int botRight[2]) {
	// Compute direction vector
	float dx = end[0] - start[0];
	float dy = end[1] - start[1];

	// Normalize the direction vector
	float length = sqrt(dx * dx + dy * dy);
	if (length == 0) return;  // Avoid division by zero

	float dirNormX = dx / length;
	float dirNormY = dy / length;

	// Compute perpendicular vector (rotated 90 degrees counterclockwise)
	float perpX = -dirNormY;
	float perpY = dirNormX;

	// Scale perpendicular vector to half of the rectangle width
	float halfWidth = RECTANGLE_WIDE / 2.0f;
	float offsetX = perpX * halfWidth;
	float offsetY = perpY * halfWidth;

	// Compute rectangle corners
	topLeft[0] = (int)std::round(start[0] + offsetX);
	topLeft[1] = (int)std::round(start[1] + offsetY);

	topRight[0] = (int)std::round(end[0] + offsetX);
	topRight[1] = (int)std::round(end[1] + offsetY);

	botLeft[0] = (int)std::round(start[0] - offsetX);
	botLeft[1] = (int)std::round(start[1] - offsetY);

	botRight[0] = (int)std::round(end[0] - offsetX);
	botRight[1] = (int)std::round(end[1] - offsetY);
}

void PopulateTopPoints(TopPointData* topPointData, int numberOfPoints)
{
	for (int i = 0; i < numberOfPoints -1; i++)
	{
		ComputeRectangle(topPointData[i].start, topPointData[i].end, topPointData[i].topLeft, topPointData[i].topRight, topPointData[i].botLeft, topPointData[i].botRight);
	}
}

void ClearPointData(TopPointData* topPointData, int numberOfPoints)
{
	for (int i = 0; i < numberOfPoints; i++)
	{
		topPointData[i] = { {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
	}
}

#include <cstring> //memset

int UnpackRGBA8X(const unsigned char* rgba) {
	int x = rgba[0] + rgba[2] * 256;
	return x;
}
int UnpackRGBA8Y(const unsigned char* rgba) {
	int y = rgba[1] + rgba[3] * 256;
	return y;
}
// Update the texture each frame :
void UpdateTexture(int textureId, int sideSize,TopPointData* data,int currentPoints) {
	
	int numPoints = currentPoints;

	static unsigned char* rawTextureData = (unsigned char*)calloc(MAX_TOP_POINTS * 6* 4 /* 4 bytes casue (RGBA8)*/,sizeof(unsigned char));

	// Clear texture data
	memset(rawTextureData, 0, sizeof(char) * currentPoints * 6 * 4);

	// Pack the data into the texture data vector
	for (int i = 0; i < numPoints; ++i) {
		const TopPointData& pointData = data[i];

		// Calculate the starting index in the texture data
		int startIndex = i * 6 * 4; // 6 ints per TopPointData, 4 bytes per int

		// Pack a vec2 per texel, rn R and G are (x,y), B A are empty
		PackVec2ToRGBA8(pointData.start, rawTextureData, startIndex);
		PackVec2ToRGBA8(pointData.end, rawTextureData, startIndex + 4);
		PackVec2ToRGBA8(pointData.topLeft, rawTextureData, startIndex + 8);
		PackVec2ToRGBA8(pointData.topRight, rawTextureData, startIndex + 12);
		PackVec2ToRGBA8(pointData.botLeft, rawTextureData, startIndex + 16);
		PackVec2ToRGBA8(pointData.botRight, rawTextureData, startIndex + 20);
	}

	// Update the texture
	rlUpdateTexture(textureId, 0, 0, sideSize, 1, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, rawTextureData);

	//unsigned char* readbackData = new unsigned char[currentPoints * 6 * 4];
	//readbackData = (unsigned char*)rlReadTexturePixels(textureId, sideSize, 1, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

	//// Assuming you're testing with the first TopPointData (index 0)
	//int baseIndex = 0 * 6 * 4; // 6 ivec2s per TopPointData, 4 bytes per ivec2

	//// Unpack end
	//int unpackedEndX = UnpackRGBA8X(&readbackData[baseIndex + (1 * 4)]);
	//int unpackedEndY = UnpackRGBA8Y(&readbackData[baseIndex + (1 * 4)]);

	//int unpackedBotLeftX = UnpackRGBA8X(&readbackData[baseIndex + (4 * 4)]);
	//int unpackedBotLeftY = UnpackRGBA8Y(&readbackData[baseIndex + (4 * 4)]);

	//printf("end %d %d ", unpackedEndX, unpackedEndY);
	//printf("botleft %d %d\n", unpackedBotLeftX, unpackedBotLeftY);
}

// Helper function to pack two ints into a single RGBA8 texel
inline void PackVec2ToRGBA8(const int vec2[2], unsigned char* rawTexData, int startIndex) {
	rawTexData[startIndex] = (vec2[0] >> 0) & 0xFF; // Lower byte of X
	rawTexData[startIndex + 1] = (vec2[1] >> 0) & 0xFF;// Lower byte of Y
	rawTexData[startIndex + 2] = (vec2[0] >> 8) & 0xFF; // Higher byte of X
	rawTexData[startIndex + 3] = (vec2[1] >> 8) & 0xFF;// Higher byte of Y
}
