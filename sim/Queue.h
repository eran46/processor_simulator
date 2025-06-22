#include <stdio.h>
#include <stdlib.h>

// Node structure
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Queue structure
typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

// Function to create a new node
Node* createNode(int data);

// Function to initialize a new queue
Queue* createQueue();

// Function to check if the queue is empty
int isQueueEmpty(Queue* queue);

// Function to enqueue (add) an element to the queue
void enqueue(Queue* queue, int data);

// Function to dequeue (remove) an element from the queue
int dequeue(Queue* queue);

// Function to get the front element of the queue without dequeuing
int peek(Queue* queue);

// Function to print the queue elements
void printQueue(Queue* queue);

// Function to free the queue
void freeQueue(Queue* queue);
