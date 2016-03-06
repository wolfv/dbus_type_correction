// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "touch_position_correction.h"

// NOTE: This class has been translated to C++ and modified from the Android
// Open Source Project. Specifically from some functions of the following file:
// https://android.googlesource.com/platform/packages/inputmethods/LatinIME/+/
// android-5.1.1_r8/java/src/com/android/inputmethod/keyboard/internal/
// TouchPositionCorrection.java

namespace prediction {

const int TouchPositionCorrection::TOUCH_POSITION_CORRECTION_RECORD_SIZE = 3;

TouchPositionCorrection::TouchPositionCorrection() {
  // value currently used by Android TouchPositionCorrection
  std::string data[9] = {"0.0038756",
                         "-0.0005677",
                         "0.1577026",
                         "-0.0236678",
                         "0.0381731",
                         "0.1529972",
                         "-0.0086827",
                         "0.0880847",
                         "0.1522819"};
  const int data_length = 9;
  if (data_length % TOUCH_POSITION_CORRECTION_RECORD_SIZE != 0) {
    return;
  }

  for (int i = 0; i < data_length; ++i) {
    const int type = i % TOUCH_POSITION_CORRECTION_RECORD_SIZE;
    const int index = i / TOUCH_POSITION_CORRECTION_RECORD_SIZE;
    const float value = std::stof(data[i]);
    if (type == 0) {
      xs_[index] = value;
    } else if (type == 1) {
      ys_[index] = value;
    } else {
      radii_[index] = value;
    }
  }
  enabled_ = data_length > 0;
}

TouchPositionCorrection::~TouchPositionCorrection() {
}

bool TouchPositionCorrection::IsValid() {
  return enabled_;
}

int TouchPositionCorrection::GetRows() {
  return 3;
}

float TouchPositionCorrection::GetX(const int row) {
  // Touch position correction data for X coordinate is obsolete.
  return 0.0f;
}

float TouchPositionCorrection::GetY(const int row) {
  return ys_[row];
}

float TouchPositionCorrection::GetRadius(const int row) {
  return radii_[row];
}

}  // namespace prediction
