# Virtual Audio Driver Examples

This directory contains example programs demonstrating how to use the Virtual Audio Driver with named pipes.

## Overview

After the driver architecture changes, user applications must **create** the named pipe before the driver can use it. These examples show the correct way to create and manage named pipes for audio input.

## Examples

### 1. CreatePipeExample.cpp (C++)

A comprehensive C++ example that creates a named pipe and generates test audio signals.

**Features:**
- Creates named pipe with proper parameters
- Generates sine waves at different frequencies  
- Sends silence periods
- Handles errors gracefully
- Supports multiple virtual microphone pins

**Building:**
```bash
# Using Visual Studio
cl /EHsc CreatePipeExample.cpp

# Using MinGW
g++ -o CreatePipeExample.exe CreatePipeExample.cpp -lws2_32
```

**Usage:**
```bash
CreatePipeExample.exe [pin_number]
```

### 2. create_pipe_example.py (Python)

A Python equivalent with additional features like white noise generation and frequency sweeps.

**Requirements:**
```bash
pip install pywin32 numpy
```

**Usage:**
```bash
python create_pipe_example.py [pin_number]
```

**Features:**
- All features from C++ example
- White noise generation
- Frequency sweep generation
- Easy to modify and extend

## How It Works

1. **User Application** creates a named pipe using Windows APIs:
   - `CreateNamedPipe()` (Win32 API)
   - `win32pipe.CreateNamedPipe()` (Python)

2. **Virtual Audio Driver** detects and opens the existing pipe:
   - Periodically tries to open the pipe using `ZwOpenFile()`
   - Reads audio data from the pipe
   - Generates silence if no pipe is available

3. **Audio Flow:**
   ```
   User App → Named Pipe → Driver → Windows Audio System → Applications
   ```

## Usage Tips

### For Application Developers

1. **Create the pipe first** before expecting audio to work
2. **Keep the pipe open** while sending audio data
3. **Write data continuously** for smooth audio playback
4. **Handle pipe disconnections** gracefully
5. **Use the correct pipe name format**: `\\.\pipe\VirtualMicInput_[pin_number]`

### Audio Format

All examples use the following audio format:
- **Sample Rate**: 44,100 Hz
- **Channels**: 2 (Stereo)
- **Bit Depth**: 16-bit
- **Format**: PCM (Little Endian)
- **Data Rate**: 176.4 KB/s

### Multiple Pins

The driver supports multiple virtual microphone pins. Use different pin numbers to create separate audio inputs:

```cpp
// Pin 0: \\.\pipe\VirtualMicInput_0
// Pin 1: \\.\pipe\VirtualMicInput_1
// Pin 2: \\.\pipe\VirtualMicInput_2
// etc.
```

## Troubleshooting

### Common Issues

1. **"Failed to create named pipe"**
   - Run as Administrator
   - Check if another application is using the same pipe name
   - Verify Windows version supports named pipes

2. **"No audio output"**
   - Ensure Virtual Audio Driver is installed and running
   - Set the virtual microphone as default recording device
   - Check Windows audio settings
   - Verify your application is writing data continuously

3. **"Audio choppy or distorted"**
   - Ensure consistent data writing (no long pauses)
   - Check audio format matches driver expectations
   - Verify buffer sizes are appropriate

### Debug Tips

1. Use **Windows Event Viewer** to check for driver messages
2. Use **Device Manager** to verify driver installation
3. Use **Windows Sound Settings** to test microphone input
4. Monitor **pipe data flow** using tools like Process Monitor

## Integration Examples

### With OBS Studio
1. Run the pipe example
2. Set "Virtual Mic Driver" as microphone in OBS
3. Should receive the generated test audio

### With Discord/Teams  
1. Run the pipe example
2. Set "Virtual Mic Driver" as microphone in the application
3. Other users should hear the generated test audio

### With Audio Recording Software
1. Run the pipe example  
2. Set virtual microphone as input device
3. Should record the generated test audio

## Advanced Usage

### Custom Audio Formats

To use different audio formats, modify both the driver configuration and your application:

1. Update driver format support (requires driver modifications)
2. Change application sample rate/format generation
3. Ensure both use the same format parameters

### Multiple Applications

Multiple applications can write to the same pipe, but:
- Only one can have the pipe open at a time
- Use coordination mechanisms if needed
- Consider using different pins for different applications

## Support

For issues with these examples or the Virtual Audio Driver:
1. Check this repository's issues
2. Review the troubleshooting guide
3. Check Windows Event Viewer for driver errors
4. Ensure you're using the latest driver version