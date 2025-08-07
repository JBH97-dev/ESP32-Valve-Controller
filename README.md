# ESP32 Valve Controller - Professional Architecture

A professional-grade ESP32 water valve controller built with ESP-IDF, featuring clean separation of concerns and modular architecture.

## 🏗️ Architecture Overview

This project follows a clean architecture pattern with clear separation between business logic, web API, and presentation layers:

\`\`\`
├── main/                   # Application entry point
│   ├── main.c             # Main application initialization
│   └── CMakeLists.txt     # Main component build config
├── webapi/                # Web API layer (Business Logic)
│   ├── models/            # Data models and structures
│   │   ├── valve_model.h  # Valve data model interface
│   │   └── valve_model.c  # Valve data model implementation
│   ├── services/          # Business logic services
│   │   ├── valve_service.h # Valve business logic interface
│   │   └── valve_service.c # Valve business logic implementation
│   ├── controllers/       # HTTP request controllers
│   │   ├── valve_controller.h   # Valve API controller interface
│   │   ├── valve_controller.c   # Valve API controller implementation
│   │   ├── web_controller.h     # Web interface controller interface
│   │   └── web_controller.c     # Web interface controller implementation
│   └── CMakeLists.txt     # WebAPI component build config
├── web/                   # Web interface files
│   └── index.html         # Modern responsive web interface
└── scripts/               # Utility scripts
    └── upload_web_files.py # SPIFFS upload utility
\`\`\`

## 🚀 Key Features

### Business Logic (C on ESP32)
- **Valve Model**: Data structures and state management
- **Valve Service**: Core business logic with thread safety
- **Hardware Abstraction**: Clean separation between logic and GPIO
- **Error Handling**: Comprehensive error checking and recovery
- **Runtime Tracking**: Valve usage statistics and monitoring

### Web API Controllers
- **RESTful API**: Clean HTTP endpoints for all operations
- **JSON Responses**: Structured data exchange
- **Input Validation**: Comprehensive request validation
- **Error Responses**: Detailed error information
- **Thread Safety**: Mutex-protected operations

### Advanced Features
- **Emergency Stop**: System-wide safety shutdown
- **Valve Configuration**: Runtime valve naming and settings
- **System Health**: Real-time health monitoring
- **Connection Status**: Live connection monitoring
- **Responsive Design**: Works on desktop and mobile

## 📡 API Endpoints

### System Status
- `GET /api/status` - Get complete system status
- `POST /api/emergency-stop` - Emergency shutdown

### Individual Valve Control
- `GET /api/valve/{id}` - Get valve details
- `POST /api/valve/{id}/control` - Control valve (on/off/toggle)
- `PUT /api/valve/{id}` - Update valve configuration

### Bulk Operations
- `POST /api/valves/all` - Control all valves simultaneously

### Web Interface
- `GET /` - Serve responsive web interface

## 🔧 Hardware Setup

### Components Required
- ESP32 development board
- 4-8 solenoid valves (12V recommended)
- Relay modules (one per valve)
- 12V power supply for valves
- 5V power supply for ESP32/relays

### Default GPIO Mapping
\`\`\`c
Valve 1 -> GPIO 2
Valve 2 -> GPIO 4  
Valve 3 -> GPIO 5
Valve 4 -> GPIO 18
Valve 5 -> GPIO 19 (if using more valves)
Valve 6 -> GPIO 21
Valve 7 -> GPIO 22
Valve 8 -> GPIO 23
\`\`\`

## 🛠️ Development Setup

### 1. Install ESP-IDF
\`\`\`bash
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh
. ./export.sh
\`\`\`

### 2. Configure Project
\`\`\`bash
# Clone project
git clone <your-repo>
cd esp32-valve-controller

# Configure WiFi credentials in main/main.c
# Update WIFI_SSID and WIFI_PASS

# Configure valve count if needed
# Update VALVE_COUNT in main/main.c
\`\`\`

### 3. Build and Deploy
\`\`\`bash
# Build project
idf.py build

# Flash firmware
idf.py flash

# Upload web files to SPIFFS
python scripts/upload_web_files.py

# Monitor serial output
idf.py monitor
\`\`\`

## 🏛️ Architecture Benefits

### Separation of Concerns
- **Models**: Pure data structures and state management
- **Services**: Business logic isolated from HTTP concerns
- **Controllers**: HTTP request/response handling only
- **Main**: Application initialization and coordination

### Maintainability
- **Modular Design**: Each component has a single responsibility
- **Clean Interfaces**: Well-defined APIs between layers
- **Error Handling**: Consistent error propagation
- **Logging**: Comprehensive logging at each layer

### Scalability
- **Easy Extension**: Add new valves by updating configuration
- **New Features**: Add controllers/services without affecting others
- **Testing**: Each layer can be tested independently
- **Integration**: Clean APIs for external system integration

### Thread Safety
- **Mutex Protection**: Critical sections properly protected
- **Atomic Operations**: State changes are atomic
- **Error Recovery**: Graceful handling of concurrent access

## 🔒 Safety Features

### Emergency Stop
- Immediate shutdown of all valves
- System disable to prevent accidental activation
- Web interface confirmation dialog
- Hardware-level GPIO control

### Error Handling
- Valve error states tracked and reported
- Hardware failure detection
- Automatic error recovery where possible
- Comprehensive error logging

### Input Validation
- All API inputs validated
- Malformed JSON rejected
- Invalid valve IDs handled gracefully
- Range checking on all parameters

## 📊 Monitoring & Diagnostics

### System Health
- Real-time health status monitoring
- Valve error state detection
- Connection status tracking
- System uptime reporting

### Usage Statistics
- Individual valve runtime tracking
- Last activation timestamps
- Total system usage metrics
- Historical data preservation

### Logging
- Structured logging with levels
- Component-specific log tags
- Error condition logging
- Performance monitoring

## 🎯 Production Considerations

### Security
- Input validation on all endpoints
- Error message sanitization
- Network access control recommended
- Firmware signing for production

### Performance
- Optimized memory usage
- Efficient JSON processing
- Minimal HTTP overhead
- Fast GPIO switching

### Reliability
- Watchdog timer integration
- Automatic error recovery
- Graceful degradation
- System health monitoring

This architecture provides a solid foundation for professional water management systems with room for future enhancements like scheduling, sensor integration, and IoT platform connectivity.
