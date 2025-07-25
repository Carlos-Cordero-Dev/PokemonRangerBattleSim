
#include "player_polygon.h"

#include <math.h>
#include <stdio.h>
#include <string> //memset

#include "raylib.h"

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
	//maybe a memeset is better?
	memset(topPointData, 0, sizeof(TopPointData) * numberOfPoints);
	 
	//for (int i = 0; i < numberOfPoints; i++)
	//{
	//	topPointData[i] = { {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
	//}
}