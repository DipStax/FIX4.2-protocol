# Log

## Registration

Before any use, you must declare the available type of `ILogger` and their name for the entier application.

- Register a named `ILogger`:
```cpp
logger::Manager::registerNewLogger<logger::imp::Console>("console");
logger::Manager::registerNewLogger<logger::imp::File>("file");
```

- Register the default `ILogger`:
```cpp
logger::Manager::registerDefaultLogger<logger::imp::Buffer>();
```

The library provide builtin `ILogger` implementation:
- `logger::imp::File`: Using the name of the logger, it will create a file (and folder using `/`) to log into it.
- `logger::imp::Console`: Writte the log event to the console.
- `logger::imp::Buffer`: pass the event thought a global threadpool to unload the logging system from the current thread. Then, it log to a file and the console using `logger::imp::Console` and `logger::imp::File`.

## Create

After registering your logger types, you can create logger instances as needed.
There are two main ways to create a logger:

- **Default Logger**: Creates a logger using the default registered type. The string you pass (e.g, `"Name"`) is used as the logger's identifier (and, for file loggers, as the filename)
```cpp
std::unique_ptr<logger::ILogger> Logger = logger::Manager::newLogger("Name");
```

- **Named Logger Type:** Creates a logger of a specific registered type (e.g., "file" or "console"), with a custom name.
```cpp
std::unique_ptr<logger::ILogger> Logger = logger::Manager::newLogger("file", "Name");
```
#### Lifetime:

- Each call to newLogger returns a new, independent logger instance.

## Logging

Once you have a logger instance, you can log messages at various severity levels using the log method.
The log method is a template that takes the log level as a template parameter, followed by one or more message arguments:

```cpp
Logger->log<logger::Level::Info>("This is an info message");
Logger->log<logger::Level::Error>("Error code: ", errorCode, ", reason: ", reason);
```

There are six log levels available, converted to string by the function `LevelToString`:
- `logger::Level::Verbose` (**VERB**) — Detailed debugging information.
- `logger::Level::Debug` (**DEBUG**) — Debugging messages.
- `logger::Level::Info` (**INFO**) — General informational messages.
- `logger::Level::Warning` (**WARN**) — Warnings about potential issues.
- `logger::Level::Error` (**ERR**) — Errors that require attention.
- `logger::Level::Fatal` (**FATAL**) — Critical errors that may cause termination.

During the convertion to string, if a log level isn't unknow the display will be `UKWN`.

#### Formatting:

The logger handles formatting and type conversion automatically.
If you pass an unsupported type, ensure it can be converted to a string (e.g., via `operator<<`).
## Example Usage

```cpp
// Registration (once at startup)
logger::Manager::registerNewLogger<logger::imp::Console>("console");
logger::Manager::registerNewLogger<logger::imp::File>("file");
logger::Manager::registerDefaultLogger<logger::imp::Buffer>();

// Creating and using a logger
auto Logger = logger::Manager::newLogger("file", "MyApp");
Logger->log<logger::Level::Info>("Application started");
Logger->log<logger::Level::Error>("An error occurred: ", errorMsg);
```

## Customization

You can implement your own logger by inheriting from `logger::ILogger` and registering it:

```cpp
class MyLogger : public logger::ILogger {
    // Implement required methods...
};

logger::Manager::registerNewLogger<MyLogger>("mylogger");
```

## Notes

- Log files are created in the `./logs/` directory by default.
- The logging system is extensible and can be integrated with other components easily.
