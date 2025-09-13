//#ifndef WINDOWS
//#include <conio.h>
//#elifndef LINUX
//#endif
#include <stdio.h>
#include <stdlib.h> //memory stuff

#include "FIFO.h"

int currDepth = 0;

void InsertCoord(Coord **stack, float x, float y)
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

void InsertCoordNoDepth(Coord** stack, float x, float y)
{
	Coord* aux;
	aux = (Coord*)malloc(sizeof(Coord));
	aux->x = x;
	aux->y = y;
	aux->intersected = false;

	//printf("depth %d\n", currDepth);

	if (*stack != nullptr)
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
  for(aux=stack;aux!=nullptr;aux=aux->nextCoord) printf("%f %f\n",aux->x,aux->y);
  printf("=====================\n");
}

int GetStackCount(Coord *stack)
{
  int count = 0;
  Coord *aux;
  for(aux=stack;aux!=nullptr;aux=aux->nextCoord) count++;
  return count;
}

Coord* BotStack(Coord* stack, int mode)
{
	if (stack == nullptr) {
		return nullptr; // Empty stack
	}

	if (mode == 0) { // Get the last element
		Coord* aux = stack;
		while (aux->nextCoord != nullptr) {
			aux = aux->nextCoord;
		}
		return aux;
	}
	else if (mode == -1) { // Get the second to last element
	 // Need at least 2 elements for second to last
		if (stack->nextCoord == nullptr) {
			return nullptr; // Only one element, no second to last
		}
		Coord* aux = stack;
		while (aux->nextCoord != nullptr && aux->nextCoord->nextCoord != nullptr) {
			aux = aux->nextCoord;
		}
		return aux;
	}
	return nullptr; // Invalid mode
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

Coord* ExtractFIFONoDepth(Coord** stack)
{
	Coord* aux = nullptr;

	if (*stack == nullptr) { /*printf("NO QUEDA COORD\n")*/; return NULL; }
	else if ((*stack)->nextCoord == nullptr)
	{
		// printf("QUEDA UNA COORD\n");
		aux = &**stack;
		*stack = nullptr;
		return aux;
	}
	else
	{
		aux = BotStack(*stack, 0);
		BotStack(*stack, -1)->nextCoord = nullptr;
		return aux;
	}
}

void DestroyStack(Coord **stack)
{
	while (*stack != nullptr)
	{
		Coord* aux = ExtractFIFO(&*stack);
		free(aux);
	}

	currDepth = 0;
}
void DestroyStackNoDepth(Coord** stack)
{
	while (*stack != nullptr)
	{
		Coord* aux = ExtractFIFO(&*stack);
		//printf("\nDestroyed");
		free(aux);
	}
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

