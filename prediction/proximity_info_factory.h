// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_PREDICTION_PROXIMITY_INFO_FACTORY_H_
#define SERVICES_PREDICTION_PROXIMITY_INFO_FACTORY_H_

#include "key_set.h"
#include "third_party/android_prediction/suggest/core/layout/proximity_info.h"

// NOTE: This class has been translated to C++ and modified from the Android
// Open Source Project. Specifically from some functions of the following file:
// https://android.googlesource.com/platform/packages/inputmethods/LatinIME/+/
// android-5.1.1_r8/java/src/com/android/inputmethod/keyboard/ProximityInfo.java

namespace prediction {

class ProximityInfoFactory {
 public:
  ProximityInfoFactory();
  ~ProximityInfoFactory();

  latinime::ProximityInfo* GetNativeProximityInfo();

 private:
  // Number of key widths from current touch point to search for nearest keys.
  static const float SEARCH_DISTANCE;
  static const float DEFAULT_TOUCH_POSITION_CORRECTION_RADIUS;

  int SquaredDistanceToEdge(int x, int y, Key k);

  int pgrid_width_;
  int pgrid_height_;
  int pgrid_size_;
  int pcell_width_;
  int pcell_height_;
  int pkeyboard_min_width_;
  int pkeyboard_height_;
  int pmost_common_key_width_;
  int pmost_common_key_height_;
  std::string plocale_;
};  // class ProximityInfoFactory

}  // namespace prediction

#endif  // SERVICES_PREDICTION_PROXIMITY_INFO_FACTORY_H_
