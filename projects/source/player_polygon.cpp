
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

// Update the texture each frame :
void UpdateTexture(int textureId, int sideSize,TopPointData* data,int currentPoints) {
	
	int numPoints = currentPoints;

	static unsigned char* rawTextureData = (unsigned char*)calloc(MAX_TOP_POINTS * 4 /* 4 bytes casue (RGBA8)*/,sizeof(unsigned char));

	// Clear texture data
	memset(rawTextureData, 0, sizeof(char) * currentPoints * 4);

	// Pack the data into the texture data vector
	for (int i = 0; i < numPoints; ++i) {
		const TopPointData& pointData = data[i];

		// Calculate the starting index in the texture data
		int startIndex = i * 6 * 4; // 6 ints per TopPointData, 4 bytes per int

		// Pack each int into 4 bytes (RGBA8)
		PackIntToRGBA8(pointData.start[0], rawTextureData, startIndex);
		PackIntToRGBA8(pointData.start[1], rawTextureData, startIndex + 4);
		PackIntToRGBA8(pointData.end[0], rawTextureData, startIndex + 8);
		PackIntToRGBA8(pointData.end[1], rawTextureData, startIndex + 12);
		PackIntToRGBA8(pointData.topLeft[0], rawTextureData, startIndex + 16);
		PackIntToRGBA8(pointData.topLeft[1], rawTextureData, startIndex + 20);
		PackIntToRGBA8(pointData.topRight[0], rawTextureData, startIndex + 24);
		PackIntToRGBA8(pointData.topRight[1], rawTextureData, startIndex + 28);
		PackIntToRGBA8(pointData.botLeft[0], rawTextureData, startIndex + 32);
		PackIntToRGBA8(pointData.botLeft[1], rawTextureData, startIndex + 36);
		PackIntToRGBA8(pointData.botRight[0], rawTextureData, startIndex + 40);
		PackIntToRGBA8(pointData.botRight[1], rawTextureData, startIndex + 44);
	}

	// Update the texture
	rlUpdateTexture(textureId, 0,0, sideSize, sideSize, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, rawTextureData);
}

// Helper function to pack an int into 4 bytes (RGBA8)
inline void PackIntToRGBA8(int value, unsigned char* rawTexData, int startIndex) {
	rawTexData[startIndex] = (value >> 0) & 0xFF;
	rawTexData[startIndex + 1] = (value >> 8) & 0xFF;
	rawTexData[startIndex + 2] = (value >> 16) & 0xFF;
	rawTexData[startIndex + 3] = (value >> 24) & 0xFF;
}
