#include "Server/Core.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Log/Imp/Console.hpp"
#include "Shared/Log/Imp/File.hpp"
#include "Shared/Log/Imp/Buffer.hpp"

/**
 * @brief Main entry point for the FIX 4.2 protocol server
 * 
 * This function initializes the logging system and starts the server core.
 * The server will run until interrupted or a fatal error occurs.
 * 
 * @param _ac Argument count (currently unused - reserved for future CLI support)
 * @param _av Argument values (currently unused - reserved for future configuration)
 * @return Exit code: 0 for successful shutdown, non-zero for errors
 * 
 * @todo Implement command-line argument parsing for:
 *       - Custom port configuration
 *       - Configuration file path
 *       - Log level settings
 *       - Debug mode toggle
 */
int main(int _ac, const char **_av)
{
    // TODO: Replace with proper argument parsing
    std::ignore = _ac;
    std::ignore = _av;

    // Initialize logging system with multiple output targets
    // Console logger for immediate feedback during development
    logger::Manager::registerNewLogger<logger::imp::Console>("console");
    
    // File logger for persistent log storage and analysis
    logger::Manager::registerNewLogger<logger::imp::File>("file");
    
    // Buffer logger as default for high-performance logging
    logger::Manager::registerDefaultLogger<logger::imp::Buffer>();

    // Initialize server core with hardcoded ports
    // TODO: Make these configurable via command-line or config file
    // Port 8080: TCP port for FIX protocol client connections
    // Port 8081: UDP port for market data broadcast
    Core core{8080, 8081};

    // Start the server and block until shutdown
    // Returns true on clean shutdown, false on error
    return core.start();
}