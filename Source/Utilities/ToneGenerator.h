/*++

Copyright (c) Microsoft Corporation All Rights Reserved

Module Name:

    ToneGenerator.h

Abstract:

    Declaration of Simple Audio Sample sine wave generator.
--*/
#ifndef _VIRTUALAUDIODRIVER_TONEGENERATOR_H
#define _VIRTUALAUDIODRIVER_TONEGENERATOR_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>

// Define constants for kernel mode compatibility
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef _I32_MAX
#define _I32_MAX 2147483647
#endif

class ToneGenerator
{
public:
    DWORD           m_Frequency; 
    WORD            m_ChannelCount; 
    WORD            m_BitsPerSample;
    DWORD           m_SamplesPerSecond;
    double          m_Theta;
    double          m_SampleIncrement;  
    bool            m_Mute;
    BYTE*           m_PartialFrame;
    DWORD           m_PartialFrameBytes;
    DWORD           m_FrameSize;
    double          m_ToneAmplitude;
    double          m_ToneDCOffset;

public:
    ToneGenerator();
    ~ToneGenerator();
    
    NTSTATUS
    Init
    (
        _In_    DWORD                   ToneFrequency, 
        _In_    double                  ToneAmplitude,
        _In_    double                  ToneDCOffset,
        _In_    double                  ToneInitialPhase,
        _In_    PWAVEFORMATEXTENSIBLE   WfExt
    );
    
    VOID 
    GenerateSine
    (
        _Out_writes_bytes_(BufferLength) BYTE       *Buffer, 
        _In_                             size_t      BufferLength
    );

    VOID
    SetMute
    (
        _In_ bool Value
    )
    {
        m_Mute = Value;
    }

private:
    VOID InitNewFrame
    (
        _Out_writes_bytes_(FrameSize)   BYTE*  Frame, 
        _In_                            DWORD  FrameSize
    );
};

#endif // _VIRTUALAUDIODRIVER_TONEGENERATOR_H
