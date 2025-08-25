// Centralized configurable settings
#pragma once

namespace settings {
  // Conversation timings
  constexpr int kCountdownSeconds = 1;
  constexpr int kRecordSeconds = 2;

  // Detection threshold
  constexpr float kDetectThresholdCm = 20.0f;

  // HTTP
  constexpr uint32_t kHttpTimeoutMs = 30000;

  // Models & voice
  inline const char* kChatModel = "gpt-3.5-turbo"; // easy to update
  inline const char* kWhisperModel = "whisper-1";
  inline const char* kElevenVoiceId = "cgSgspJ2msm6clMCkdW9";
}
