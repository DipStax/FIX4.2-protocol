#pragma once

namespace log
{
    enum Level
    {
        Verbose,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    constexpr const char *VerboseLevel = "VERB";
    constexpr const char *DebugLevel = "DEBUG";
    constexpr const char *InfoLevel = "INFO";
    constexpr const char *WarningLevel = "WARN";
    constexpr const char *ErrorLevel = "ERR";
    constexpr const char *FatalLevel = "FATAL";
    constexpr const char *UnknowLevel = "UKWN";

    constexpr const char *LevelToString(Level _lvl)
    {
        switch (_lvl) {
            case Level::Verbose:    return VerboseLevel;
            case Level::Debug:      return DebugLevel;
            case Level::Info:       return InfoLevel;
            case Level::Warning:    return WarningLevel;
            case Level::Error:      return ErrorLevel;
            case Level::Fatal:      return FatalLevel;
            default:                return UnknowLevel;
        }
    }
}