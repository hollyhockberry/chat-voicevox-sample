// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include <ChatClient.h>
#include <WiFi.h>

#include "AudioOutputM5Speaker.h"
#include "WebVoiceVoxTask.h"
#include "MessageQueue.h"

namespace {
constexpr const char* WIFI_SSID = nullptr;
constexpr const char* WIFI_PSK = nullptr;
constexpr const char* OPENAI_APIKEY = "**** YOUR KEY ****";
constexpr const char* VOICEVOX_APIKEY = "**** YOUR KEY ****";

ChatClient _chatClient(OPENAI_APIKEY);
AudioOutputM5Speaker _audioOutput(&M5.Speaker, 0);
WebVoiceVoxTask _webVoiceVox(VOICEVOX_APIKEY, _audioOutput);
MessageQueue _messageQueue;
}  // namespace

void setup() {
  M5.begin();
  M5.Display.begin();
  M5.Display.setFont(&fonts::lgfxJapanMincho_16);
  M5.Display.setTextScroll(true);
  {
    auto spk_cfg = M5.Speaker.config();
    spk_cfg.sample_rate = 96000;
    spk_cfg.task_pinned_core = APP_CPU_NUM;
    M5.Speaker.config(spk_cfg);
    M5.Speaker.begin();
  }

  if (WIFI_SSID != nullptr && WIFI_PSK != nullptr) {
    WiFi.begin(WIFI_SSID, WIFI_PSK);
  } else {
    WiFi.begin();
  }
  while (WiFi.status() != WL_CONNECTED) {
    ::delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  _chatClient.begin();
  _chatClient.MaxHistory(3);
  _chatClient.TimeOut(20*1000);

  _webVoiceVox.begin();
  _messageQueue.begin();
}

void callback(const char* chunk) {
  _messageQueue.add(chunk);
  _messageQueue.loop(_webVoiceVox);
}

void loop() {
  static bool inTalk = true;
  _messageQueue.loop(_webVoiceVox);

  if (_messageQueue.isBusy() || _webVoiceVox.isBusy()) {
    return;
  }
  if (inTalk) {
    inTalk = false;
    Serial.print("Enter your message: ");
  }
  if (!Serial.available()) {
    return;
  }
  
  auto message = Serial.readStringUntil('\n');
  message.trim();
  if (message.equals("")) {
    return;
  }
  inTalk = true;

  Serial.println(message);
  M5.Display.printf("You: %s\r\n", message.c_str());
  M5.Display.println("GPT:");
  
  if (!_chatClient.ChatStream(message.c_str(), callback)) {
    return;
  }
  _messageQueue.flush();
}
