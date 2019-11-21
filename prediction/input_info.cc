// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <new>

#include "input_info.h"
#include "key_set.h"
#include "third_party/android_prediction/defines.h"

namespace prediction {

InputInfo::InputInfo(std::string& input, int input_size, const std::vector<Key>* keyset)
  : keyset_(keyset)
{
  real_size_ = 0;
  for (int i = 0; i < input_size; i++) {
    int codepoint = (int)input[i];
    if ((codepoint >= 'a' && codepoint <= 'z') ||
        (codepoint >= 'A' && codepoint <= 'Z')) {
      real_size_++;
    }
  }
  codepoints_ = new int[real_size_];
  x_coordinates_ = new int[real_size_];
  y_coordinates_ = new int[real_size_];
  pointer_ids_ = new int[real_size_];
  times_ = new int[real_size_];

  ProcessInput(input, input_size);
}

InputInfo::~InputInfo() {
  delete[] codepoints_;
  delete[] x_coordinates_;
  delete[] y_coordinates_;
  delete[] pointer_ids_;
  delete[] times_;
}

int* InputInfo::GetCodepoints() {
  return codepoints_;
}

int* InputInfo::GetXCoordinates() {
  return x_coordinates_;
}

int* InputInfo::GetYCoordinates() {
  return y_coordinates_;
}

int* InputInfo::GetPointerIds() {
  return pointer_ids_;
}

int* InputInfo::GetTimes() {
  return times_;
}

int InputInfo::GetRealSize() {
  return real_size_;
}

void InputInfo::ProcessInput(std::string& input, int input_size) {
  int real_index = 0;
  for (int i = 0; i < input_size; i++) {
    int codepoint = (int)input[i];
    if ((codepoint >= 'a' && codepoint <= 'z') ||
        (codepoint >= 'A' && codepoint <= 'Z')) {
      codepoints_[real_index] = codepoint;
      for (int j = 0; j < keyset_->size(); j++) {
        if ((*keyset_)[j].kcode == tolower(codepoint)) {
          x_coordinates_[real_index] =
              (*keyset_)[j].kx + (*keyset_)[j].kwidth / 2;
          y_coordinates_[real_index] =
              (*keyset_)[j].ky + (*keyset_)[j].kheight / 2;
          break;
        }
      }
      pointer_ids_[real_index] = 0;
      times_[real_index] = 0;
      real_index++;
    }
  }
}

}  // namespace prediction
