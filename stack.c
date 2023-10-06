#include "stack.h"

Stack *createStackWithCapcity(unsigned capacity) {
    Stack* stack = (Stack*) malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (char **) malloc(stack->capacity * sizeof(char *));
    return stack;
}

int isFull(Stack *stack) {
    return (stack->top) == (int) ((stack->capacity) - 1);
}

int isEmpty(Stack *stack) {
    return stack->top == -1;
}

void push(Stack *stack, char *item) {
    /* check if stack is full => double the capacity */
    if (isFull(stack)) {
        /* make sure the capacity is valid */
        if (stack->capacity <= 0) {
            stack->capacity = 1;
        }

        /* double the capacity */
        stack->capacity *= 2;
        stack->array = (char **) realloc(stack->array, 
                stack->capacity * sizeof(char *));
    }

    /* add the item onto the stack */
    stack->array[++(stack->top)] = item;
}

char *pop(Stack *stack) {
    if (isEmpty(stack)) {
        return NULL;
    }

    return stack->array[stack->top--];
}

char *peek(Stack *stack) {
    if (isEmpty(stack)) {
        return NULL;
    }

    return stack->array[stack->top];
}

void freeStack(Stack *stack) {
    free(stack->array);
    free(stack);
}

void freeStackArray(Stack *stack) {
    int i;
    for (i = 0; i <= stack->top; i++) {
        free(stack->array[i]);
    }
}
