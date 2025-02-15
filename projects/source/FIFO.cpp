//#ifndef WINDOWS
//#include <conio.h>
//#elifndef LINUX
//#endif
#include <stdio.h>
#include <stdlib.h> //memory stuff

#include "FIFO.h"

int currDepth = 0;

void InsertCoord(Coord **stack, int x, int y)
{
  Coord* aux;
  aux = (Coord*) malloc(sizeof(Coord));
  aux->x = x;
  aux->y = y;
  aux->intersected = false;
  aux->depth = currDepth;
  currDepth++;
  //printf("depth %d\n", currDepth);

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
  Coord *aux = nullptr;

  if(*stack == nullptr){ /*printf("NO QUEDA COORD\n")*/;return NULL;}
  else if((*stack)->nextCoord == nullptr)
  {
    // printf("QUEDA UNA COORD\n");
    aux = &**stack;
     *stack = nullptr;
	 currDepth--;
    return aux;
  }
  else
  {
    aux = BotStack(*stack,0);
    BotStack(*stack,-1)->nextCoord = nullptr;
	currDepth--;
    return aux;
  }
}

void DestroyStack(Coord **stack)
{
  while(TopStack(*stack)!= nullptr) free(ExtractFIFO(&*stack));
  currDepth = 0;
}

void freeCoordsBackward(Coord* start, Coord* end) {
	if (!start || !end) return;

	Coord* aux = start;
	Coord* nextToFree = nullptr;

	// Traverse the list up to end->nextCoord to find the last node
	while (aux && aux->nextCoord != end->nextCoord) {
		nextToFree = aux;    // Save the last valid node
		aux = aux->nextCoord;
	}
	
	int intersectionDepth = end->depth;


	// Traverse backward, freeing nodes
	while (nextToFree) {
		Coord* prev = start;

		// Find the node just before `nextToFree`
		while (prev->nextCoord != nextToFree && prev != nextToFree) {
			prev = prev->nextCoord;
		}

		free(nextToFree);
		//freed++;
		nextToFree = (prev == nextToFree) ? nullptr : prev; // Update to the previous node
	}
	//printf("freed %d\n", freed);


	// Reset depth values starting from the intersection
	aux = end;
	int newDepth = intersectionDepth;
	while (aux) {
		aux->depth = newDepth++;
		aux = aux->nextCoord;
	}

	// Update global depth tracker
	currDepth = newDepth;
}

int GetStackDepth(Coord* stack)
{
	int count = 0;
	Coord* current = stack;

	while (current) {
		count++;
		current = current->nextCoord;
	}

	return count;
}