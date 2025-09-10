# Process Unit

## Concept Description
A **Process Unit** is an abstraction designed to simplify and optimize concurrent computation within the application.
It represents a self-contained unit of work, typically running in its own thread or managed by a thread pool, allowing tasks to be executed in parallel for improved performance and responsiveness.

### Key Ideas

- **Threading Abstraction**:

    Instead of manually managing threads, synchronization, and task distribution, the Process Unit provides a high-level interface for defining and running computational tasks concurrently.
This abstraction hides the complexity of thread creation, lifecycle management, and inter-thread communication.

- **Parallelism for Fast Computation**:

    By encapsulating work in independent Process Units, the system can distribute tasks across multiple CPU cores.
This enables efficient parallel processing of data, such as handling multiple client requests, processing orders, or running background computations without blocking the main application flow.

- **Task Isolation**:

    Each Process Unit operates independently, reducing the risk of data races and making it easier to reason about program behavior.
Communication between units can be handled via message passing or thread-safe queues.

- **Scalability**:

    The abstraction allows the application to scale with available hardware.
As more cores are available, more Process Units can be run in parallel, increasing throughput and reducing latency.

## Abstraction

> **Note:** All process unit abstractions take a `std::string` (or compatible) as a constructor argument to assign a name to the process unit.
> This name is used for logging and identification purposes, allowing each process unit to have its own dedicated logger instance.

### AProcessUnitBase

This is the abstract base class for all process units. It provides the fundamental interface and shared functionality, such as naming and logging, for any unit of concurrent work.
You should inherit from `AProcessUnitBase` when you want to define a new kind of process unit with custom runtime logic.
It enforces the implementation of a `runtime(std::stop_token)` method, which is the entry point for the unit’s execution.

### AProcessUnit\<T>

This is a templated abstract class for process units that operate on a specific input type `T`.
It extends `AProcessUnitBase` and introduces the concept of an input queue, allowing the unit to process a stream of tasks or messages of type `T`.

Inherit from `AProcessUnit<T>` when you want your process unit to consume and process items from a thread-safe queue, such as orders, events, or messages.

### AInputProcess\<T>

This is a further specialization of `AProcessUnit<T>`, providing a ready-to-use pattern for process units that repeatedly consume inputs from their queue and process them.
It implements the main runtime loop, popping items from the queue and calling a user-defined `onInput()` method for each item.

Inherit from `AInputProcess<T>` when you want to quickly implement a worker that processes incoming data or tasks in a loop, such as an order handler, message dispatcher, or event processor.

### IProcessUnitStopable

This is an interface for process units that require custom cleanup or shutdown logic.
If your process unit needs to perform specific actions when stopping (such as flushing buffers, saving state, or notifying other components), inherit from this interface and implement the `onStop()` method.
The framework will automatically call `onStop()` during a graceful shutdown if your process unit inherits from `IProcessUnitStopable`.

## Implementation

### 1. Basic Process Unit

Implement your logic by inheriting from `pu::AProcessUnitBase`.
Pass a unique name to the base constructor for logging and identification.

```cpp
class PUDemo : public pu::AProcessUnitBase
{
    public:
        PUDemo()
            : AProcessUnitBase("ProcessUnitName")
        {
        }
        virtual ~PUDemo() = default;

    protected:
        void runtime(std::stop_token _st) final
        {
            while (!st.stop_requested()) {
                // Your business logic here
            }
        }
};
```

> Note the `virtual` destructor is present because `pu::ProcessUnit` is inheriting from `PUDemo`

You can now use this class as template parameter of `pu::ProcessUnit` to have your complete process unit, where you can start/stop and get it's status.
If the process unit implementation inherit from `pu::IProcessUnitStopable`, the `stop` function will call the `onStop` member function of the implementation after stopping the running thread.

```cpp
pu::ProcessUnit<PUDemo> demo{};

demo.start();   // Start the process unit in its own thread
demo.status();  // Query the status (e.g., running, stopped)
demo.stop();    // Stop the process unit gracefully
```

### 2. Queue-Based Process Unit

For units that process a stream of tasks or messages, inherit from `pu::AInputProcess<T>`, where `T` is the input type.

```cpp
class OrderHandler : public pu::AInputProcess<Order>
{
public:
    OrderHandler()
        : pu::AInputProcess<Order>("OrderHandler") // Name for logging
    {}

protected:
    void onInput(Order order) override
    {
        // Process the incoming order
    }
};
```

You can push work into the process unit’s queue and start it as follows:

```cpp
pu::ProcessUnit<OrderHandler> handler{};
handler.start();

Order order{/*...*/};
handler.getInput().push(order); // Add work to the queue

handler.stop();
```

---

### 3. Tips

- Always provide a unique name for each process unit for clear logging and debugging.
- Use `AInputProcess` for units that consume queued data; use `AProcessUnitBase` for custom logic.
- Implement `IProcessUnitStopable` if you need custom shutdown behavior.