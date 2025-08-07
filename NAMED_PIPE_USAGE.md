# Virtual Mic Driver - Named Pipe Usage

## Overview

This modified virtual audio driver provides a **microphone-only** implementation that accepts audio input exclusively through a named pipe interface. Speaker/audio output functionality has been completely removed.

## Named Pipe Interface

### Pipe Name Format
```
\\.\pipe\VirtualMicInput_{pin_number}
```

For the default microphone pin (pin 0):
```
\\.\pipe\VirtualMicInput_0
```

### Usage from User Applications

#### C++ Example
```cpp
#include <windows.h>
#include <iostream>

int main() {
    // Open the named pipe
    HANDLE hPipe = CreateFile(
        L"\\\\.\\pipe\\VirtualMicInput_0",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open pipe: " << GetLastError() << std::endl;
        return 1;
    }

    // Write audio data (16-bit PCM, 44.1kHz, stereo)
    BYTE audioData[4096];
    // ... fill audioData with your audio samples ...
    
    DWORD bytesWritten;
    BOOL result = WriteFile(
        hPipe,
        audioData,
        sizeof(audioData),
        &bytesWritten,
        NULL
    );

    CloseHandle(hPipe);
    return 0;
}
```

#### Python Example
```python
import os
import time

# Open the named pipe
pipe_name = r'\\.\pipe\VirtualMicInput_0'

try:
    with open(pipe_name, 'wb') as pipe:
        # Write audio data
        audio_data = b'\x00' * 4096  # Replace with actual audio data
        pipe.write(audio_data)
        pipe.flush()
except Exception as e:
    print(f"Error: {e}")
```

## Audio Format Requirements

The virtual microphone supports the following audio formats:

- **Sample Rates**: 16 kHz, 22.05 kHz, 44.1 kHz, 48 kHz, 96 kHz, 192 kHz
- **Bit Depths**: 16-bit, 24-bit, 32-bit
- **Channels**: Mono (1 channel), Stereo (2 channels)
- **Format**: PCM (uncompressed)

### Default Format
- Sample Rate: 44.1 kHz
- Bit Depth: 16-bit
- Channels: 2 (Stereo)
- Data Rate: 176.4 KB/s

## IOCTL Interface (Advanced)

For applications requiring lower latency or direct kernel-mode communication, an IOCTL interface is also available. This requires more advanced programming and understanding of Windows driver development.

## Installation

1. Install the driver using Device Manager:
   - Open Device Manager
   - Select "Add Legacy Hardware"
   - Choose "Install the hardware that I manually select from a list (Advanced)"
   - Select "Sound, video and game controllers"
   - Click "Have Disk..." and browse to `VirtualAudioDriver.inf`

2. After installation, the virtual microphone will appear as:
   - **Device Name**: "Virtual Mic Driver by MTT"
   - **Type**: Audio input device

## Usage Notes

- The driver will output silence if no data is available from the named pipe
- Multiple applications can attempt to write to the pipe, but only one can have it open at a time
- The pipe has a 4KB internal buffer
- Data should be written continuously for smooth audio playback
- The driver is thread-safe and uses spinlocks for concurrent access protection

## Troubleshooting

### Pipe Access Denied
- Ensure your application has sufficient privileges
- Close other applications that might be using the pipe

### No Audio Output
- Verify the named pipe is being written to continuously
- Check that the audio format matches the device capabilities
- Ensure the virtual microphone is set as the default recording device

### Driver Installation Issues
- Run Device Manager as Administrator
- Disable driver signature enforcement if using unsigned drivers
- Check Windows Event Viewer for driver-related errors

## Architecture Changes

This modified driver removes:
- ✅ All speaker/render endpoint functionality
- ✅ Speaker topology and wave table files
- ✅ Render filter installation
- ✅ Speaker interface registrations in INF file

And adds:
- ✅ Named pipe creation and management
- ✅ Pipe-based audio input for microphone
- ✅ IOCTL support for direct data injection
- ✅ Thread-safe concurrent access handling
- ✅ Automatic fallback to silence when no pipe data available