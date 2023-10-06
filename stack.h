#ifndef _STACK_H
#define _STACK_H  1

#include <stdlib.h>

typedef struct Stack {
    int top;
    unsigned capacity;
    char **array;
} Stack;

Stack *createStackWithCapcity(unsigned capacity);

int isFull(Stack *stack);

int isEmpty(Stack *stack);

void push(Stack *stack, char *item);

char *pop(Stack *stack);

char *peek(Stack *stack);

void freeStack(Stack *stack);
void freeStackArray(Stack *stack);

#endif
