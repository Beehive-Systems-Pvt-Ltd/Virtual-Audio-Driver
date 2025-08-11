# IOCTL Support in Virtual Audio Driver

## Overview

This Virtual Audio Driver now supports **ioctl (Input/Output Control)** functionality, allowing applications to communicate directly with the driver for status information and configuration.

## What is IOCTL?

IOCTL is a Windows system call used for device-specific operations that cannot be expressed by regular system calls. It allows applications to:
- Query device information
- Get device status
- Send configuration commands
- Perform device-specific operations

## Supported IOCTLs

### IOCTL_VIRTUAL_AUDIO_GET_INFO (0x80008000)
**Purpose**: Get basic driver information
**Input**: None
**Output**: `VIRTUAL_AUDIO_INFO` structure
```c
typedef struct _VIRTUAL_AUDIO_INFO {
    ULONG DriverVersion;        // Driver version number
    ULONG SpeakerDeviceCount;   // Number of speaker devices
    ULONG MicrophoneDeviceCount; // Number of microphone devices
    ULONG Reserved[5];          // Reserved for future use
} VIRTUAL_AUDIO_INFO;
```

### IOCTL_VIRTUAL_AUDIO_GET_STATUS (0x80008001)
**Purpose**: Get current driver status
**Input**: None
**Output**: `VIRTUAL_AUDIO_STATUS` structure
```c
typedef struct _VIRTUAL_AUDIO_STATUS {
    BOOLEAN SpeakerActive;      // TRUE if speaker devices are active
    BOOLEAN MicrophoneActive;   // TRUE if microphone devices are active
    ULONG CurrentSampleRate;    // Current sample rate in Hz
    ULONG Reserved[7];          // Reserved for future use
} VIRTUAL_AUDIO_STATUS;
```

## Usage Example

```cpp
#include <windows.h>

// Open handle to the driver device
HANDLE hDevice = CreateFile(
    L"\\\\.\\VirtualAudioDriver",
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    0,
    NULL
);

if (hDevice != INVALID_HANDLE_VALUE) {
    // Get driver information
    VIRTUAL_AUDIO_INFO info;
    DWORD bytesReturned;
    
    if (DeviceIoControl(
        hDevice,
        IOCTL_VIRTUAL_AUDIO_GET_INFO,
        NULL, 0,                    // No input
        &info, sizeof(info),        // Output buffer
        &bytesReturned,
        NULL)) {
        
        printf("Driver version: %d\n", info.DriverVersion);
        printf("Speaker devices: %d\n", info.SpeakerDeviceCount);
        printf("Microphone devices: %d\n", info.MicrophoneDeviceCount);
    }
    
    CloseHandle(hDevice);
}
```

## Technical Implementation

The ioctl support is implemented through:

1. **Device Control Dispatch Handler**: `IRP_MJ_DEVICE_CONTROL` handler registered in `DriverEntry`
2. **IOCTL Processing**: `DeviceControlHandler` function processes ioctl requests
3. **Buffer Management**: Proper validation and handling of input/output buffers
4. **Error Handling**: Standard Windows error codes returned for invalid requests

## IOCTL Codes Definition

The IOCTL codes are defined using the Windows `CTL_CODE` macro:
```c
#define FILE_DEVICE_VIRTUAL_AUDIO   0x8000
#define IOCTL_VIRTUAL_AUDIO_GET_INFO   CTL_CODE(FILE_DEVICE_VIRTUAL_AUDIO, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIRTUAL_AUDIO_GET_STATUS CTL_CODE(FILE_DEVICE_VIRTUAL_AUDIO, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS)
```

## Applications

This ioctl functionality enables:
- **Monitoring Applications**: Check driver status and device availability
- **Configuration Tools**: Get driver information and current settings  
- **Audio Applications**: Query device capabilities before use
- **System Diagnostics**: Monitor virtual audio device health

## Future Extensions

The ioctl infrastructure supports easy addition of new commands such as:
- Device configuration changes
- Audio format control
- Performance monitoring
- Advanced diagnostics

## Notes

- All ioctl operations require appropriate access rights
- The driver uses buffered I/O method for security and reliability
- Error codes follow standard Windows conventions
- The implementation is thread-safe and supports concurrent requests