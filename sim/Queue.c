#include "Queue.h"


// Function to create a new node
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Node Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize a new queue
Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        printf("Queue Memory allocation failed.\n");
        exit(1);
    }
    queue->front = queue->rear = NULL;
    return queue;
}

// Function to check if the queue is empty
int isQueueEmpty(Queue* queue) {
    return queue->front == NULL;
}

// Function to enqueue (add) an element to the queue
void enqueue(Queue* queue, int data) {
    Node* newNode = createNode(data);
    if (queue->rear == NULL) {
        // If the queue is empty, the new node is both the front and rear
        queue->front = queue->rear = newNode;
        return;
    }
    // Add the new node at the end of the queue and update the rear
    queue->rear->next = newNode;
    queue->rear = newNode;
}

// Function to dequeue (remove) an element from the queue
int dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("Queue is allready empty.\n");
        exit(1);
    }
    Node* temp = queue->front;
    int data = temp->data;
    queue->front = queue->front->next;

    // If the queue is now empty, set the rear to NULL as well
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    return data;
}

// Function to get the front element of the queue without dequeuing
int peek(Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("Queue is empty.\n");
        exit(1);
    }
    return queue->front->data;
}

// Function to print the queue elements
void printQueue(Queue* queue) {
    Node* temp = queue->front;
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}

// Function to free the queue
void freeQueue(Queue* queue) {
    while (!isQueueEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}