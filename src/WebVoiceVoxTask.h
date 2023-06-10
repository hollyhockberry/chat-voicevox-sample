// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef WEBVOICEVOXTASK_H_
#define WEBVOICEVOXTASK_H_

#include <WString.h>
#include <AudioGeneratorMP3.h>
#include <AudioFileSourceBuffer.h>
#include "AudioFileSourceHTTPSStream.h"

class WebVoiceVoxTask {
  String _key;
  AudioOutput& _audio;
  bool _busy = false;
  AudioGeneratorMP3 _mp3;
  AudioFileSourceHTTPSStream* _audioFile = nullptr;
  AudioFileSourceBuffer* _audiobuffer = nullptr;
  uint8_t* _buffer = nullptr;
  int _bufferSize;

 public:
  WebVoiceVoxTask(const char* key, AudioOutput& audio);
  ~WebVoiceVoxTask();

  bool isBusy() const { return _busy; }

  void begin(int bufferSize = 30 * 1024);
  void play(const char* message, const char* rootCACertificate = nullptr);

 private:
  static void main(void* arg);
  void loop();
  void release();
};

#endif  // WEBVOICEVOXTASK_H_