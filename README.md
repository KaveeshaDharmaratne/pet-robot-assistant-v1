# Desk Assistant (ESP32‑S3 Voice Assistant)

## Overview
An on‑device voice assistant for ESP32‑S3 that:
- Detects a nearby person via an HC‑SR04 ultrasonic sensor
- Records ~2 seconds of audio from an I2S microphone (mono, 16 kHz)
- Transcribes with OpenAI Whisper
- Gets a response from OpenAI Chat
- Streams ElevenLabs TTS (MP3) and plays out via I2S to a speaker

## Hardware
- Board: ESP32‑S3 (env: `freenove_esp32_s3_wroom`)
- Ultrasonic (HC‑SR04): TRIG = 10, ECHO = 11
- I2S Mic/Speaker: BCLK = 42, WS = 41, DIN = 7 (mic), DOUT = 6 (speaker)

Pins are defined in `src/devices/Pins.h`.

## Setup
1) Prerequisites
- VS Code + PlatformIO extension
- ESP32 USB driver installed

2) Secrets
- Edit `config.example.h` and set:
	- `WIFI_SSID`, `WIFI_PASS`
	- `OPENAI_API_KEY` (Whisper + Chat)
	- `ELEVENLABS_API_KEY`

Optionally, copy `config.example.h` to `config.h` and change `#include "../../config.example.h"` in `src/app/App.cpp` to `#include "../../config.h"` if you prefer.

3) Build and upload
- Board/env is preconfigured in `platformio.ini`.
- Use PlatformIO: Build, Upload, then Monitor (115200 baud).

## How it works
1. Wi‑Fi connects using `WiFiManager`.
2. The App state machine runs: Countdown → Record → Process → Next.
3. On detection (`< 20 cm`, configurable), it:
	 - Records PCM via I2S RX, wraps it as WAV
	 - Sends to Whisper (transcribe)
	 - Calls OpenAI Chat with a system prompt (`include/Prompts.h`)
	 - Streams ElevenLabs MP3 to I2S TX for playback

## Project Structure
```text
include/
	Settings.h         # Tunables: thresholds, timeouts, models, voice
	Prompts.h          # System prompt text
config.example.h     # WiFi + API keys (can be renamed to config.h)
platformio.ini
src/
	main.cpp           # Thin Arduino entry → App
	app/
		App.h
		App.cpp          # Conversation state machine
	devices/
		Pins.h           # Pin assignments
		UltrasonicSensor.h
	audio/
		I2SManager.h     # RX/TX mode switching for I2S
		Microphone.h     # Record PCM via I2S RX
		WavEncoder.h     # Build in‑memory WAV
	net/
		WiFiManager.h
	services/
		STTService.h     # Whisper transcription
		ChatService.h    # OpenAI Chat
		TTSService.h     # ElevenLabs streaming MP3 → I2S
	utils/
		Logger.h         # Leveled logging
		Result.h         # Simple result wrapper
		Stopwatch.h      # millis() helper
```

## Configuration
- `include/Settings.h`: change detection threshold, record length, HTTP timeouts, model/voice IDs.
- `include/Prompts.h`: tweak the system prompt.
- `src/devices/Pins.h`: pin definitions if your wiring differs.

## Dependencies (auto‑installed via PlatformIO)
- `pschatzmann/arduino-libhelix` (MP3 decoding)
- `pschatzmann/arduino-audio-tools` (audio pipeline)
- `martinsos/HCSR04` (ultrasonic)
- `bblanchon/ArduinoJson` (JSON)

## Security notes
- TLS currently uses `setInsecure()` for simplicity. For production, add proper CA/fingerprint validation.

## Troubleshooting
- Empty transcription: verify mic wiring and I2S pins; check Wi‑Fi/API keys.
- No audio playback: confirm I2S DOUT/WS/BCLK and speaker amp wiring; watch serial logs.
- HTTP errors: ensure Wi‑Fi connectivity and valid API keys; see the serial log for HTTP status.

## Contributors
- Lahiru Wimalarathna @lahiruC22
- Pathini Ekanayake   @PathiniE

## License
This project is licensed under the GNU General Public License v3.0. See the `LICENSE` file for details.
