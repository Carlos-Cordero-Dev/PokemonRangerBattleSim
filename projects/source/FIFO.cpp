//#ifndef WINDOWS
//#include <conio.h>
//#elifndef LINUX
//#endif
#include <stdio.h>
#include <stdlib.h> //memory stuff

#include "FIFO.h"

void InsertCoord(Coord **stack, int x, int y)
{
  Coord* aux;
  aux = (Coord*) malloc(sizeof(Coord));
  aux->x = x;
  aux->y = y;

  if(*stack!=nullptr)
  { //stack contains something
    aux->nextCoord = &**stack;
    *stack = aux;
  }
  else //empty stack
  {
    aux->nextCoord = nullptr;
    *stack = aux;
  }
}

void ShowStack(Coord *stack)
{
  Coord *aux;
  printf("=====================\n");
  for(aux=stack;aux!=nullptr;aux=aux->nextCoord) printf("%d %d\n",aux->x,aux->y);
  printf("=====================\n");
}

int GetStackCount(Coord *stack)
{
  int count = 0;
  Coord *aux;
  for(aux=stack;aux!=nullptr;aux=aux->nextCoord) count++;
  return count;
}
Coord *BotStack(Coord *stack,int mode)
{
  Coord *aux;
  if(mode == 0)for(aux=stack;aux->nextCoord!=nullptr;aux=aux->nextCoord);
  else if(mode == -1)for(aux=stack;(aux->nextCoord)->nextCoord!=nullptr;aux=aux->nextCoord);
  return aux;
}

Coord *TopStack(Coord *stack)
{
  return stack;
}

Coord *ExtractFIFO(Coord **stack)
{
  Coord *aux;

  if(*stack == nullptr){ /*printf("NO QUEDA COORD\n")*/;return NULL;}
  else if((*stack)->nextCoord == nullptr)
  {
    // printf("QUEDA UNA COORD\n");
    aux = &**stack;
     *stack = nullptr;
    return aux;
  }
  else
  {
    aux = BotStack(*stack,0);
    BotStack(*stack,-1)->nextCoord = nullptr;
    return aux;
  }
}

void DestroyStack(Coord **stack)
{
  while(TopStack(*stack)!= nullptr) free(ExtractFIFO(&*stack));
}
