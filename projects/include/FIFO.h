
#ifndef __FIFO_H__
#define __FIFO_H__

struct Coord
{ //only used for fifo stack implementation
  int x,y;
  Coord *nextCoord;
};

void InsertCoord(Coord **stack, int x, int y);

void ShowStack(Coord *stack);

int GetStackCount(Coord *stack);

Coord *BotStack(Coord *stack,int mode);

Coord *TopStack(Coord *stack);

Coord *ExtractFIFO(Coord **stack);

void DestroyStack(Coord **stack);

#endif
