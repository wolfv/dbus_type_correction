// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_PREDICTION_TOUCH_POSITION_CORRECTION_H_
#define SERVICES_PREDICTION_TOUCH_POSITION_CORRECTION_H_

// #include "mojo/services/prediction/interfaces/prediction.mojom.h"
#include <string>

// NOTE: This class has been translated to C++ and modified from the Android
// Open Source Project. Specifically from some functions of the following file:
// https://android.googlesource.com/platform/packages/inputmethods/LatinIME/+/
// android-5.1.1_r8/java/src/com/android/inputmethod/keyboard/internal/
// TouchPositionCorrection.java

namespace prediction {

class TouchPositionCorrection {
 public:
  TouchPositionCorrection();
  ~TouchPositionCorrection();

  bool IsValid();

  int GetRows();
  float GetX(const int row);
  float GetY(const int row);
  float GetRadius(const int row);

 private:
  static const int TOUCH_POSITION_CORRECTION_RECORD_SIZE;

  bool enabled_;
  float xs_[3];
  float ys_[3];
  float radii_[3];
};  // class TouchPositionCorrection

}  // namespace prediction

#endif  // SERVICES_PREDICTION_TOUCH_POSITION_CORRECTION_H_
