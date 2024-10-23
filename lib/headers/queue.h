#pragma once


#include <stdbool.h>
#include <stddef.h>

// A structure to hold the data required for a queue to work
// Should not be used directly
struct Queue {
	unsigned int front;
	unsigned int back;
	unsigned int capacity;
	size_t item_size;
	void* data;
	bool full;
};

// Enumeration values for `Queue_next`, to indicate whether or not the
// output structure was filled correctly.
enum QueueNextSuccess { QUEUE_OK, QUEUE_EMPTY };

// Creates a new queue structure and returns it
//
// Parameters:
//	item_size: size of each item to be stored in the queue, as returned
//		by the `sizeof` operator
//	capacity: # of items the queue should be able to store initially
//
// Return value:
//	A `struct Queue` that can be used with the other functions in this
//	this header
struct Queue Queue_new(size_t item_size, unsigned int capacity);
// Gets the next item in a queue if one exists
//
// Parameters:
//	queue: a pointer to a `struct Queue` from which a value is to be
//		obtained
//	out: a pointer to where the retrieved value should be written
//
// Return value:
//	An `enum QueueNextSuccess` which represents whether or not the
//	value was written properly. On any value other than `QUEUE_OK`,
//	`out` is not affected.
enum QueueNextSuccess Queue_next(struct Queue* queue, void* out);
// Inserts an item into the queue
//
// Parameters:
//	queue: a pointer to a `struct Queue` into which `item` is inserted
//	item: a pointer to the item to be inserted. `item` will be copied
void Queue_queue(struct Queue* queue, void* item);
// Frees memory used by the queue
//
// Parameters:
//	queue: a pointer to a `struct Queue` which is to be freed
void Queue_free(struct Queue* queue);
