# Virtual Mic Driver - Named Pipe Usage

## Overview

This modified virtual audio driver provides a **microphone-only** implementation that accepts audio input exclusively through a named pipe interface. Speaker/audio output functionality has been completely removed.

## Named Pipe Interface

### Important Note on Pipe Creation

The virtual audio driver uses a cooperative approach for named pipe communication:

- **User applications** should create the named pipe using standard Windows APIs (CreateNamedPipe)
- **The driver** will attempt to open existing pipes created by user applications  
- If no pipe exists, the driver will generate silence until a pipe becomes available

### Pipe Name Format
```
\\.\pipe\VirtualMicInput_{pin_number}
```

For the default microphone pin (pin 0):
```
\\.\pipe\VirtualMicInput_0
```

### Usage from User Applications

#### C++ Example (Updated)
```cpp
#include <windows.h>
#include <iostream>

int main() {
    // STEP 1: Create the named pipe (user application responsibility)
    HANDLE hPipeServer = CreateNamedPipe(
        L"\\\\.\\pipe\\VirtualMicInput_0",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,      // MaxInstances
        4096,   // OutBufferSize
        4096,   // InBufferSize  
        0,      // DefaultTimeOut
        NULL    // Security attributes
    );
    
    if (hPipeServer == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create named pipe: " << GetLastError() << std::endl;
        return 1;
    }
    
    std::cout << "Named pipe created successfully. Driver can now connect." << std::endl;
    
    // STEP 2: Write audio data to the pipe
    BYTE audioData[4096];
    // ... fill audioData with your audio samples ...
    
    DWORD bytesWritten;
    BOOL result = WriteFile(
        hPipeServer,
        audioData,
        sizeof(audioData),
        &bytesWritten,
        NULL
    );
    
    if (result) {
        std::cout << "Written " << bytesWritten << " bytes to pipe." << std::endl;
    }
    
    CloseHandle(hPipeServer);
    return 0;
}
```

#### Python Example (Updated)
```python
import win32pipe
import win32file
import time

# STEP 1: Create the named pipe (user application responsibility)
pipe_name = r'\\.\pipe\VirtualMicInput_0'

try:
    # Create the named pipe
    pipe_handle = win32pipe.CreateNamedPipe(
        pipe_name,
        win32pipe.PIPE_ACCESS_DUPLEX,
        win32pipe.PIPE_TYPE_BYTE | win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
        1,      # MaxInstances
        4096,   # OutBufferSize
        4096,   # InBufferSize
        0,      # DefaultTimeOut
        None    # Security attributes
    )
    
    print("Named pipe created successfully. Driver can now connect.")
    
    # STEP 2: Write audio data
    audio_data = b'\x00' * 4096  # Replace with actual audio data
    win32file.WriteFile(pipe_handle, audio_data)
    print(f"Written {len(audio_data)} bytes to pipe.")
    
    win32file.CloseHandle(pipe_handle)
    
except Exception as e:
    print(f"Error: {e}")
```

**Note**: You'll need `pywin32` package: `pip install pywin32`

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
- Ensure your application has sufficient privileges to create named pipes
- Run your application as Administrator if needed
- Verify that no other application is already using the same pipe name

### No Audio Output
- **IMPORTANT**: Make sure your user application **creates** the named pipe first
- The driver will generate silence if no pipe exists
- Verify the named pipe is being written to continuously  
- Check that the audio format matches the device capabilities
- Ensure the virtual microphone is set as the default recording device
- Check Windows Event Viewer for driver-related messages

### Driver Installation Issues
- Run Device Manager as Administrator
- Disable driver signature enforcement if using unsigned drivers
- Check Windows Event Viewer for driver-related errors

### Named Pipe Connection Issues
- Verify your application creates the pipe with the exact name: `\\.\pipe\VirtualMicInput_0`
- Check that your application keeps the pipe open while sending audio data
- The driver attempts to connect periodically - ensure your pipe remains available

## Architecture Changes

This modified driver removes:
- ✅ All speaker/render endpoint functionality
- ✅ Speaker topology and wave table files
- ✅ Render filter installation
- ✅ Speaker interface registrations in INF file
- ✅ Kernel-mode named pipe creation (replaced with user-mode pipe creation)

And adds:
- ✅ Named pipe opening and management (user-mode created pipes)
- ✅ Pipe-based audio input for microphone
- ✅ IOCTL support for direct data injection (placeholder)
- ✅ Thread-safe concurrent access handling
- ✅ Automatic fallback to silence when no pipe data available
- ✅ Dynamic pipe opening when user applications create pipes

## Technical Implementation Notes

### Named Pipe Communication Model

The driver implements a **hybrid communication model**:

1. **User applications create** the named pipe using `CreateNamedPipe()` Win32 API
2. **The driver attempts to open** existing pipes using `ZwOpenFile()` when audio data is needed
3. **Fallback behavior**: If no pipe exists, the driver generates silence
4. **Automatic retry**: The driver periodically attempts to reconnect to pipes

This approach is more aligned with Windows driver architecture principles where:
- Kernel drivers avoid creating user-accessible named objects
- User-mode applications manage the pipe lifecycle  
- Kernel drivers consume data from existing pipes