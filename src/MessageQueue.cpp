// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include "MessageQueue.h"
#include "WebVoiceVoxTask.h"

MessageQueue::MessageQueue() {
}

bool MessageQueue::isBusy() const {
  return !_temporary.isEmpty() || _queue.size() > 0;
}

void MessageQueue::begin() {
  _temporary = "";
  _delimiters = { "。", "！", "？", "、", ".", ",", "!", "?" };
  // _delimiters = { "。", "！", "？", "!", "?" };
}

void MessageQueue::loop(WebVoiceVoxTask& voice) {
  if (voice.isBusy() || _queue.empty()) {
    return;
  }
  String message = "";
  int counter = 0;
  while (!_queue.empty() && (_maxDequeue <= 0 || _maxDequeue > counter)) {
    message += _queue.front();
    _queue.pop();
    ++counter;
  }
  // Serial.printf("POP : %d, %s\r\n", counter, message.c_str());
  if (!message.isEmpty()) {
    M5.Display.println(message);
    voice.play(message.c_str());
  }
}

void MessageQueue::add(const char* chunk) {
  // Serial.printf("%s", chunk);
  _temporary += chunk;
  for (const auto& delimiter : _delimiters) {
    auto idx = _temporary.indexOf(delimiter);
    if (idx >= 0) {
      auto res = _temporary.substring(0, idx + delimiter.length());
      String message(res);
      queue(message);
      _temporary.remove(0, idx + delimiter.length());
      return;
    }
  }
}

void MessageQueue::flush() {
  if (_temporary.length() > 0) {
    queue(_temporary);
    _temporary.clear();
  }
}

void MessageQueue::queue(String& message) {
  message.trim();
  if (message.isEmpty()) {
    return;
  }
  for (const auto& delimiter : _delimiters) {
    if (message.equals(delimiter)) {
      return;
    }
  }
  // Serial.printf("\r\nPUSH: %d >", _queue.size());
  _queue.push(message);
}
