
#include "player_polygon.h"

#include <math.h>
#include <stdio.h>

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
