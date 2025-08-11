/*++

Copyright (c) Beehive Systems Pvt Ltd. All Rights Reserved

Module Name:

    CreatePipeExample.cpp

Abstract:

    Example demonstrating how to create a named pipe for the Virtual Audio Driver
    to connect to. This example shows the proper way to create and manage the
    named pipe that the kernel driver will open.

--*/

#include <windows.h>
#include <iostream>
#include <vector>
#include <cmath>

// Constants for audio generation
const int SAMPLE_RATE = 44100;
const int CHANNELS = 2;
const int BITS_PER_SAMPLE = 16;
const int BYTES_PER_SAMPLE = BITS_PER_SAMPLE / 8;
const int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * BYTES_PER_SAMPLE / 10; // 100ms buffer

class VirtualMicPipeServer
{
private:
    HANDLE m_hPipe;
    std::wstring m_pipeName;
    bool m_isRunning;
    
public:
    VirtualMicPipeServer(int pinNumber = 0) : m_hPipe(INVALID_HANDLE_VALUE), m_isRunning(false)
    {
        // Create pipe name for the specified pin
        m_pipeName = L"\\\\.\\pipe\\VirtualMicInput_" + std::to_wstring(pinNumber);
    }
    
    ~VirtualMicPipeServer()
    {
        Stop();
    }
    
    bool Start()
    {
        // Create the named pipe
        m_hPipe = CreateNamedPipeW(
            m_pipeName.c_str(),
            PIPE_ACCESS_OUTBOUND,               // We write to the driver
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1,                                  // MaxInstances
            BUFFER_SIZE,                        // OutBufferSize  
            BUFFER_SIZE,                        // InBufferSize
            0,                                  // DefaultTimeOut
            NULL                                // Security attributes
        );
        
        if (m_hPipe == INVALID_HANDLE_VALUE)
        {
            std::wcerr << L"Failed to create named pipe: " << m_pipeName 
                      << L", Error: " << GetLastError() << std::endl;
            return false;
        }
        
        std::wcout << L"Named pipe created successfully: " << m_pipeName << std::endl;
        std::wcout << L"Virtual Audio Driver can now connect to this pipe." << std::endl;
        
        m_isRunning = true;
        return true;
    }
    
    void Stop()
    {
        m_isRunning = false;
        if (m_hPipe != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_hPipe);
            m_hPipe = INVALID_HANDLE_VALUE;
        }
    }
    
    bool SendSineWave(double frequency, double duration)
    {
        if (m_hPipe == INVALID_HANDLE_VALUE || !m_isRunning)
        {
            return false;
        }
        
        int totalSamples = (int)(SAMPLE_RATE * duration);
        std::vector<int16_t> buffer(totalSamples * CHANNELS);
        
        // Generate stereo sine wave
        for (int i = 0; i < totalSamples; i++)
        {
            double time = (double)i / SAMPLE_RATE;
            double sample = sin(2.0 * M_PI * frequency * time) * 16000; // Amplitude of 16000
            
            int16_t sampleValue = (int16_t)sample;
            buffer[i * 2] = sampleValue;     // Left channel
            buffer[i * 2 + 1] = sampleValue; // Right channel
        }
        
        // Send data to pipe
        DWORD bytesToWrite = totalSamples * CHANNELS * sizeof(int16_t);
        DWORD bytesWritten = 0;
        
        BOOL result = WriteFile(
            m_hPipe,
            buffer.data(),
            bytesToWrite,
            &bytesWritten,
            NULL
        );
        
        if (!result || bytesWritten != bytesToWrite)
        {
            std::wcerr << L"Failed to write audio data to pipe, Error: " 
                      << GetLastError() << std::endl;
            return false;
        }
        
        std::wcout << L"Sent " << bytesWritten << L" bytes (" << duration 
                  << L"s of " << frequency << L"Hz sine wave)" << std::endl;
        
        return true;
    }
    
    bool SendSilence(double duration)
    {
        if (m_hPipe == INVALID_HANDLE_VALUE || !m_isRunning)
        {
            return false;
        }
        
        int totalSamples = (int)(SAMPLE_RATE * duration);
        std::vector<int16_t> buffer(totalSamples * CHANNELS, 0); // All zeros for silence
        
        DWORD bytesToWrite = totalSamples * CHANNELS * sizeof(int16_t);
        DWORD bytesWritten = 0;
        
        BOOL result = WriteFile(
            m_hPipe,
            buffer.data(),
            bytesToWrite,
            &bytesWritten,
            NULL
        );
        
        if (!result || bytesWritten != bytesToWrite)
        {
            std::wcerr << L"Failed to write silence to pipe, Error: " 
                      << GetLastError() << std::endl;
            return false;
        }
        
        std::wcout << L"Sent " << duration << L"s of silence (" << bytesWritten 
                  << L" bytes)" << std::endl;
        
        return true;
    }
};

void PrintUsage()
{
    std::wcout << L"Usage: CreatePipeExample.exe [pin_number]" << std::endl;
    std::wcout << L"  pin_number: Virtual microphone pin number (default: 0)" << std::endl;
    std::wcout << L"" << std::endl;
    std::wcout << L"This program creates a named pipe and sends test audio data." << std::endl;
    std::wcout << L"Make sure the Virtual Audio Driver is installed and running." << std::endl;
}

int main(int argc, char* argv[])
{
    int pinNumber = 0;
    
    // Parse command line arguments
    if (argc > 1)
    {
        pinNumber = atoi(argv[1]);
        if (pinNumber < 0)
        {
            std::wcerr << L"Invalid pin number. Using default: 0" << std::endl;
            pinNumber = 0;
        }
    }
    
    if (argc > 1 && (strcmp(argv[1], "/?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        PrintUsage();
        return 0;
    }
    
    std::wcout << L"Virtual Audio Driver - Named Pipe Example" << std::endl;
    std::wcout << L"=========================================" << std::endl;
    std::wcout << L"Using microphone pin: " << pinNumber << std::endl;
    std::wcout << L"" << std::endl;
    
    VirtualMicPipeServer pipeServer(pinNumber);
    
    // Start the pipe server
    if (!pipeServer.Start())
    {
        std::wcerr << L"Failed to start pipe server" << std::endl;
        return 1;
    }
    
    std::wcout << L"Pipe server started. Sending test audio..." << std::endl;
    std::wcout << L"Press Ctrl+C to stop." << std::endl;
    std::wcout << L"" << std::endl;
    
    try
    {
        // Send a series of test tones
        while (true)
        {
            // Send 1 second of 440Hz (A4)
            pipeServer.SendSineWave(440.0, 1.0);
            Sleep(100); // Small gap
            
            // Send 1 second of 880Hz (A5)  
            pipeServer.SendSineWave(880.0, 1.0);
            Sleep(100);
            
            // Send 1 second of silence
            pipeServer.SendSilence(1.0);
            Sleep(100);
            
            // Send 1 second of 1000Hz
            pipeServer.SendSineWave(1000.0, 1.0);
            Sleep(1000); // Longer pause between cycles
        }
    }
    catch (...)
    {
        std::wcout << L"Stopping..." << std::endl;
    }
    
    pipeServer.Stop();
    std::wcout << L"Pipe server stopped." << std::endl;
    
    return 0;
}