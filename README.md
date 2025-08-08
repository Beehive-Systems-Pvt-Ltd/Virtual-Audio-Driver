<table>
  <tr>
    <td><img src="https://github.com/user-attachments/assets/ca93d971-67dc-41dd-b945-ab4f372ea72a" /></td>
    <td>Free code signing on Windows provided by <a href="https://signpath.io">SignPath.io</a>, certificate by <a href="https://signpath.org">SignPath Foundation</a></td>
  </tr>
</table>

> [!NOTE]
> Software Developers/Organizations: Looking to implement Virtual speakers/mics into your app? For advanced/custom functionality like named pipes, shared memory buffers (for no-latency audio), direct integration with your existing apps, and more; Contact us for quotes on a custom build! contact@mikethetech.com

# Virtual Audio Driver by MikeTheTech (Modified - Microphone Only)

**🎤 This is a modified version that provides ONLY virtual microphone functionality with named pipe input support.**

Welcome to the **Modified Virtual Audio Driver**! This version has been specifically modified to:

- **✅ Provide only virtual microphone input** (speaker output completely removed)
- **✅ Accept audio data exclusively via named pipe interface**
- **✅ Support IOCTL-based direct audio injection**
- **✅ Prevent other applications from using the microphone as standard input**

**Original Features Removed:**
- ❌ Virtual speaker output device  
- ❌ Audio rendering/playback capabilities
- ❌ Property pages for external application input

The modified virtual audio driver provides:

- **Virtual Microphone with Named Pipe Input** – A virtual microphone that accepts audio data exclusively through a named pipe interface (`\\.\pipe\VirtualMicInput_0`), perfect for applications that need to inject custom audio data programmatically.

## Key Modifications

This version has been specifically modified for applications requiring:
- **Exclusive control over microphone input** – Only your application can provide audio data
- **Programmatic audio injection** – Send audio data directly via named pipe or IOCTL
- **No interference from other apps** – Standard Windows audio applications cannot access the microphone
- **Simplified driver architecture** – Microphone-only implementation reduces complexity

## Named Pipe Interface

Your applications can send audio data to the virtual microphone using:

```
Pipe Name: \\.\pipe\VirtualMicInput_0
Format: 16-bit PCM, 44.1kHz, Stereo
Access: Write-only from user applications
```

See [NAMED_PIPE_USAGE.md](NAMED_PIPE_USAGE.md) for detailed usage examples and API documentation.

## Quick Start with Named Pipes

1. **Install the driver** using Device Manager (see Installation section below)
2. **Run an example application** to create the named pipe:
   ```bash
   # C++ Example  
   cd examples
   cl CreatePipeExample.cpp && CreatePipeExample.exe
   
   # Python Example (requires: pip install pywin32 numpy)
   python create_pipe_example.py
   ```
3. **Set the virtual microphone** as your default recording device in Windows Sound Settings
4. **Test with any application** that uses microphone input (Discord, OBS, etc.)

**📁 Example Applications**: Check the [`examples/`](examples/) directory for complete working examples in C++ and Python that demonstrate proper named pipe creation and usage.

---

## Key Features

| Feature                                      | Virtual Speaker                                                                                                                                                                          | Virtual Microphone                                                                                                                              |
|----------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------|
| **Emulated Device**                          | Emulates a speaker device recognized by Windows.                                                                                                                                         | Emulates a microphone device recognized by Windows.                                                                                             |
| **Supported Audio Formats**                  | **8-bit, 8000 Hz** (lowest quality) up to **32-bit, 192,000 Hz** (studio quality). This includes various presets such as Telephone Quality (8-bit, 11025 Hz) and DVD/Studio Quality (16/24/32-bit, up to 192 kHz). | **16-bit 44,100 Hz**, **16-bit 48,000 Hz**, **24-bit 96,000 Hz**, **24-bit 192,000 Hz**, **32-bit 48,000 Hz** (for testing or specialized scenarios). |
| **Spatial Sound Support** (Speaker Only)     | Integrates with Windows Sonic, enabling immersive 3D audio features.                                                                                                                      | Integrated with Audio Enhancements such as Voice Focus and Background Noise Reduction.                                                          |
| **Exclusive Mode and App Priority**          | Applications can claim exclusive control of the device.                                                                                                                                  | Same WDK architecture applies, allowing exclusive access in supported workflows.                                                                 |
| **Volume Level Handling**                    | Handles global and per-application volume changes (Windows mixer).                                                                                                                        | Microphone level adjustments accessible via Windows Sound Settings or audio software.                                                            |
| **High Customizability**                     | Built to be extended with future features and audio enhancements.                                                                                                                         | Allows flexible configuration of sampling rates and bit depths for specialized audio requirements.                                               |

---

## Compatibility

- **OS**: Windows 10 (Build 1903 and above) and Windows 11  
- **Architecture**: x64 (tested); ARM64  

---

## Installation

1. **Open Device Manager**
   - Choose **Audio inputs and outputs**, then in the top **Action** menu, choose **Add Legacy Hardware**.
   - Choose **Install the hardware that I manually select from a list (Advanced)**
   - Choose **Sound, video and game controllers**
   - Choose "Have Disk..." and locate the **VirtualAudioDriver.inf**
   - Continue with the installation.

2. **Verify Installation**  
   - Open **Device Manager**.  
   - Check under **Sound, video and game controllers** for “Virtual Audio Driver” (speaker).  
   - Check under **Audio inputs and outputs** for “Virtual Mic Driver” (microphone).

---

## Usage

### Using the Virtual Speaker

1. **Select as Default Device**  
   - Open **Sound Settings** → **Output** → Choose “Virtual Audio Driver” as your default output device.  
   - Or use the **Volume Mixer** to route specific apps to the virtual speaker.

2. **Remote Desktop or Streaming**  
   - When initiating a remote desktop session, the virtual speaker device can appear as a valid playback device.  
   - Streaming or capture apps can detect the virtual speaker for capturing system audio.

3. **Choosing Quality Settings**  
   - In **Sound Settings**, under **Playback Devices**, right-click the “Virtual Audio Driver” and select **Properties** → **Advanced** tab.  
   - Choose from the newly added sample rates and bit depths (ranging from 8-bit, 8000 Hz to 32-bit, 192,000 Hz) to match your desired use case (e.g., “Telephone Quality,” “DVD Quality,” or “Studio Quality”).

4. **Spatial Sound**  
   - In **Sound Settings**, right-click the device, select **Properties** → **Spatial Sound** tab, and enable **Windows Sonic for Headphones** or another supported format.

### Using the Virtual Microphone

1. **Select as Default Recording Device**  
   - Open **Sound Settings** → **Input** → Choose “Virtual Mic Driver” as your default input device.  
   - Alternatively, in the **Volume Mixer** or your specific application’s audio settings, route or select the “Virtual Mic Driver” for input.

2. **Supported Formats**  
   - The Virtual Microphone Driver supports:
     - **16-bit, 44,100 Hz**
     - **16-bit, 48,000 Hz**
     - **24-bit, 96,000 Hz**
     - **24-bit, 192,000 Hz**
     - **32-bit, 48,000 Hz**

3. **Use Cases**  
   - **Voice Chat / Conference Apps**: Emulate or inject audio into Zoom, Teams, Discord, etc.  
   - **Streaming / Broadcasting**: Feed application-generated audio to OBS, XSplit, or other streaming tools.  
   - **Audio Testing**: Confirm that your software or game engine’s microphone-handling logic works without real hardware.

4. **Volume & Level Controls**  
   - Adjust input levels in **Sound Settings** → **Recording** tab.  
   - Per-app mic levels can be configured in certain software or system volume mixers (where supported).

---

## Configuration

- **Exclusive Mode** (Speaker and Mic)  
  By default, shared mode is enabled. For real-time, low-latency usage, open device properties, go to the **Advanced** tab, and uncheck “Allow applications to take exclusive control.”

- **Application Priority**  
  Supported through Windows APIs. To prioritize a specific application for either the speaker or microphone, configure it via Windows advanced sound options or in your audio software.

- **Volume/Level Management**  
  - For the Virtual Speaker, adjust in the main Sound Settings or the Volume Mixer.  
  - For the Virtual Microphone, adjust input levels in the Recording tab of Sound Settings or in your audio software’s device preferences.

---

## Future Plans

- **Advanced Diagnostics**: Logging and debugging tools  
- **New Modes & Additional Formats**: Continued expansion of supported audio qualities.  
- **Additional Features**: Such as Automatic Volume Leveling (AVL), further spatial audio improvements, and custom routing tools.

> This project is maintained and actively improved. We welcome contributions, suggestions, and issue reports!

---

**Thank you for using the Virtual Audio Driver!**  
Feel free to open issues or submit pull requests if you encounter any problems or have ideas to share. Happy audio routing!
