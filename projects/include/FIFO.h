

#ifndef __FIFO_H__
#define __FIFO_H__

struct Coord
{ //only used for fifo stack implementation
  int x,y;
  Coord *nextCoord;
  bool intersected = false;
  int depth = 0;
  float distance_at_point = 0.0f;
};

void InsertCoord(Coord **stack, int x, int y);

void ShowStack(Coord *stack);

int GetStackCount(Coord *stack);

//mode: 0 = goes up until the last coord, -1= goes to the one before last coord, so lastcoord -1
Coord *BotStack(Coord *stack,int mode);

Coord *TopStack(Coord *stack);

Coord *ExtractFIFO(Coord **stack);

void DestroyStack(Coord **stack);

int GetStackDepth(Coord* stack);

bool PointInPolygon(Coord* start, Coord* end, int x, int y);

void freeCoordsBackward(Coord* start, Coord* end);

#endif
