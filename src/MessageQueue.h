// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <queue>
#include <WString.h>

class WebVoiceVoxTask;

class MessageQueue {
  String _temporary;
  std::queue<String> _queue;
  std::vector<String> _delimiters;
  int _maxDequeue = 0;

 public:
  MessageQueue();

  bool isBusy() const;
  int MaxDequeue() const;
  void MaxDequeue(int max) {
    _maxDequeue = max;
  }

  void begin();
  void loop(WebVoiceVoxTask& voice);

  void add(const char* chunk);
  void flush();

 private:
  void queue(String& message);
};

#endif  // MESSAGEQUEUE_H_
