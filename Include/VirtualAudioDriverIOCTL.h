/*++

Module Name:

    VirtualAudioDriverIOCTL.h

Abstract:

    Header file for applications that want to communicate with the 
    Virtual Audio Driver using IOCTL commands.

--*/

#ifndef _VIRTUAL_AUDIO_DRIVER_IOCTL_H_
#define _VIRTUAL_AUDIO_DRIVER_IOCTL_H_

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// IOCTL Definitions
//=============================================================================

// Device type for Virtual Audio Driver IOCTLs
#define FILE_DEVICE_VIRTUAL_AUDIO   0x8000

// IOCTL function codes
#define IOCTL_VIRTUAL_AUDIO_GET_INFO    CTL_CODE(FILE_DEVICE_VIRTUAL_AUDIO, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIRTUAL_AUDIO_GET_STATUS  CTL_CODE(FILE_DEVICE_VIRTUAL_AUDIO, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS)

//=============================================================================
// Data Structures
//=============================================================================

// Structure for driver information
typedef struct _VIRTUAL_AUDIO_INFO {
    ULONG DriverVersion;
    ULONG SpeakerDeviceCount;
    ULONG MicrophoneDeviceCount;
    ULONG Reserved[5];
} VIRTUAL_AUDIO_INFO, *PVIRTUAL_AUDIO_INFO;

// Structure for driver status
typedef struct _VIRTUAL_AUDIO_STATUS {
    BOOLEAN SpeakerActive;
    BOOLEAN MicrophoneActive;
    ULONG CurrentSampleRate;
    ULONG Reserved[7];
} VIRTUAL_AUDIO_STATUS, *PVIRTUAL_AUDIO_STATUS;

//=============================================================================
// Helper Functions (Example implementations)
//=============================================================================

// Example function to open the driver device
// Applications should implement their own version based on actual device path
inline HANDLE OpenVirtualAudioDriver(void)
{
    // Note: The actual device path may vary depending on installation
    // This is just an example - check Windows Device Manager for actual path
    return CreateFile(
        L"\\\\.\\VirtualAudioDriver",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
}

// Example function to get driver information
inline BOOL GetVirtualAudioInfo(HANDLE hDevice, PVIRTUAL_AUDIO_INFO pInfo)
{
    DWORD bytesReturned = 0;
    
    if (!hDevice || hDevice == INVALID_HANDLE_VALUE || !pInfo)
        return FALSE;
        
    return DeviceIoControl(
        hDevice,
        IOCTL_VIRTUAL_AUDIO_GET_INFO,
        NULL,                       // No input buffer
        0,                          // Input buffer size
        pInfo,                      // Output buffer
        sizeof(VIRTUAL_AUDIO_INFO), // Output buffer size
        &bytesReturned,
        NULL
    );
}

// Example function to get driver status
inline BOOL GetVirtualAudioStatus(HANDLE hDevice, PVIRTUAL_AUDIO_STATUS pStatus)
{
    DWORD bytesReturned = 0;
    
    if (!hDevice || hDevice == INVALID_HANDLE_VALUE || !pStatus)
        return FALSE;
        
    return DeviceIoControl(
        hDevice,
        IOCTL_VIRTUAL_AUDIO_GET_STATUS,
        NULL,                          // No input buffer
        0,                             // Input buffer size
        pStatus,                       // Output buffer
        sizeof(VIRTUAL_AUDIO_STATUS),  // Output buffer size
        &bytesReturned,
        NULL
    );
}

#ifdef __cplusplus
}
#endif

#endif // _VIRTUAL_AUDIO_DRIVER_IOCTL_H_