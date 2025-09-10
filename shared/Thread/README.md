# Thread

This module provides essential building blocks for concurrent programming, focusing on thread pools and thread-safe queues.

## Overview

- **Thread Pool:**
  Offers a fixed-size pool of worker threads that can execute tasks asynchronously.
  You can enqueue callable tasks, which are then distributed among the available threads for parallel execution. The pool manages thread lifecycle and ensures efficient task scheduling.

- **Thread-Safe Queue:**
  Implements a queue that can be safely accessed from multiple threads.
  It supports standard queue operations such as pushing, popping, checking if the queue is empty, and retrieving the front element. Internal synchronization ensures data integrity and prevents race conditions.

## Typical Usage

- **Thread Pool Example**:
  ```cpp
  // Create a thread pool with a specified number of worker threads
  ThreadPool<4> pool;

  // Enqueue a task for asynchronous execution
  pool.enqueue([]() {
      // Task logic here
      std::cout << "Running in the thread pool!" << std::endl;
  });
  ```
- **Thread-Safe Queue Example**:
  ```cpp
  // Create a thread-safe queue for integers
  ts::Queue<int> queue;

  // Push an item from one thread
  queue.push(123);

  // Pop an item from another thread
  if (!queue.empty()) {
      int value = queue.pop_front();
      // Use value...
  }
  ```