"""
Virtual Audio Driver - Python Named Pipe Example

This example demonstrates how to create a named pipe for the Virtual Audio Driver
to connect to. This shows the proper way to create and manage the named pipe
that the kernel driver will open.

Requirements:
    pip install pywin32 numpy

Usage:
    python create_pipe_example.py [pin_number]
"""

import sys
import time
import math
import struct
from typing import Optional

try:
    import win32pipe
    import win32file
    import numpy as np
except ImportError:
    print("Required packages not installed. Please run:")
    print("pip install pywin32 numpy")
    sys.exit(1)

# Audio constants
SAMPLE_RATE = 44100
CHANNELS = 2
BITS_PER_SAMPLE = 16
BYTES_PER_SAMPLE = BITS_PER_SAMPLE // 8

class VirtualMicPipeServer:
    def __init__(self, pin_number: int = 0):
        self.pipe_name = f"\\\\.\\pipe\\VirtualMicInput_{pin_number}"
        self.pipe_handle: Optional[int] = None
        self.is_running = False
        
    def start(self) -> bool:
        """Create and start the named pipe server"""
        try:
            # Create the named pipe
            self.pipe_handle = win32pipe.CreateNamedPipe(
                self.pipe_name,
                win32pipe.PIPE_ACCESS_OUTBOUND,
                win32pipe.PIPE_TYPE_BYTE | win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
                1,      # MaxInstances
                8192,   # OutBufferSize
                8192,   # InBufferSize
                0,      # DefaultTimeOut
                None    # Security attributes
            )
            
            print(f"Named pipe created successfully: {self.pipe_name}")
            print("Virtual Audio Driver can now connect to this pipe.")
            self.is_running = True
            return True
            
        except Exception as e:
            print(f"Failed to create named pipe: {e}")
            return False
    
    def stop(self):
        """Stop the pipe server and cleanup"""
        self.is_running = False
        if self.pipe_handle:
            win32file.CloseHandle(self.pipe_handle)
            self.pipe_handle = None
    
    def send_sine_wave(self, frequency: float, duration: float) -> bool:
        """Send a sine wave of specified frequency and duration"""
        if not self.pipe_handle or not self.is_running:
            return False
        
        try:
            # Generate samples
            total_samples = int(SAMPLE_RATE * duration)
            t = np.linspace(0, duration, total_samples, dtype=np.float32)
            
            # Generate sine wave
            wave = np.sin(2 * np.pi * frequency * t) * 16000  # Amplitude of 16000
            wave = wave.astype(np.int16)
            
            # Convert to stereo
            stereo_wave = np.zeros((total_samples * CHANNELS,), dtype=np.int16)
            stereo_wave[0::2] = wave  # Left channel
            stereo_wave[1::2] = wave  # Right channel
            
            # Convert to bytes
            audio_data = stereo_wave.tobytes()
            
            # Send to pipe
            win32file.WriteFile(self.pipe_handle, audio_data)
            
            print(f"Sent {len(audio_data)} bytes ({duration:.1f}s of {frequency:.0f}Hz sine wave)")
            return True
            
        except Exception as e:
            print(f"Failed to send sine wave: {e}")
            return False
    
    def send_silence(self, duration: float) -> bool:
        """Send silence for specified duration"""
        if not self.pipe_handle or not self.is_running:
            return False
        
        try:
            # Generate silence (zeros)
            total_samples = int(SAMPLE_RATE * duration * CHANNELS)
            silence = np.zeros(total_samples, dtype=np.int16)
            audio_data = silence.tobytes()
            
            # Send to pipe
            win32file.WriteFile(self.pipe_handle, audio_data)
            
            print(f"Sent {duration:.1f}s of silence ({len(audio_data)} bytes)")
            return True
            
        except Exception as e:
            print(f"Failed to send silence: {e}")
            return False
    
    def send_white_noise(self, duration: float, amplitude: float = 0.1) -> bool:
        """Send white noise for specified duration"""
        if not self.pipe_handle or not self.is_running:
            return False
        
        try:
            # Generate white noise
            total_samples = int(SAMPLE_RATE * duration)
            noise = np.random.normal(0, amplitude, total_samples) * 16000
            noise = np.clip(noise, -32767, 32767).astype(np.int16)
            
            # Convert to stereo
            stereo_noise = np.zeros((total_samples * CHANNELS,), dtype=np.int16)
            stereo_noise[0::2] = noise  # Left channel
            stereo_noise[1::2] = noise  # Right channel
            
            # Convert to bytes
            audio_data = stereo_noise.tobytes()
            
            # Send to pipe
            win32file.WriteFile(self.pipe_handle, audio_data)
            
            print(f"Sent {duration:.1f}s of white noise ({len(audio_data)} bytes)")
            return True
            
        except Exception as e:
            print(f"Failed to send white noise: {e}")
            return False

def print_usage():
    print("Usage: python create_pipe_example.py [pin_number]")
    print("  pin_number: Virtual microphone pin number (default: 0)")
    print("")
    print("This program creates a named pipe and sends test audio data.")
    print("Make sure the Virtual Audio Driver is installed and running.")

def main():
    pin_number = 0
    
    # Parse command line arguments
    if len(sys.argv) > 1:
        if sys.argv[1] in ["/?" , "-h", "--help"]:
            print_usage()
            return 0
        try:
            pin_number = int(sys.argv[1])
            if pin_number < 0:
                print("Invalid pin number. Using default: 0")
                pin_number = 0
        except ValueError:
            print("Invalid pin number. Using default: 0")
            pin_number = 0
    
    print("Virtual Audio Driver - Python Named Pipe Example")
    print("=" * 50)
    print(f"Using microphone pin: {pin_number}")
    print()
    
    pipe_server = VirtualMicPipeServer(pin_number)
    
    # Start the pipe server
    if not pipe_server.start():
        print("Failed to start pipe server")
        return 1
    
    print("Pipe server started. Sending test audio...")
    print("Press Ctrl+C to stop.")
    print()
    
    try:
        cycle = 0
        while True:
            cycle += 1
            print(f"\n--- Cycle {cycle} ---")
            
            # Send 1 second of 440Hz (A4)
            pipe_server.send_sine_wave(440.0, 1.0)
            time.sleep(0.1)
            
            # Send 1 second of 880Hz (A5)
            pipe_server.send_sine_wave(880.0, 1.0)
            time.sleep(0.1)
            
            # Send some white noise
            pipe_server.send_white_noise(0.5, 0.1)
            time.sleep(0.1)
            
            # Send 1 second of silence
            pipe_server.send_silence(1.0)
            time.sleep(0.1)
            
            # Send a sweep from 200Hz to 2000Hz
            for freq in [200, 400, 800, 1200, 1600, 2000]:
                pipe_server.send_sine_wave(freq, 0.3)
                time.sleep(0.05)
            
            time.sleep(1.0)  # Longer pause between cycles
            
    except KeyboardInterrupt:
        print("\nStopping...")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        pipe_server.stop()
        print("Pipe server stopped.")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())