// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_PREDICTION_KEY_SET_H_
#define SERVICES_PREDICTION_KEY_SET_H_

// #include "mojo/services/prediction/interfaces/prediction.mojom.h"

// qwerty keyboard key sets

namespace prediction {

// NOTE: This struct has been modified from the Android Open
// Source Project. Specifically from the following file:
// https://android.googlesource.com/platform/packages/inputmethods/LatinIME/+/
// android-5.1.1_r8/java/src/com/android/inputmethod/keyboard/Key.java
struct Key {
  int kcode;
  // Width of the key, not including the gap
  int kwidth;
  // Height of the key, not including the gap
  int kheight;
  // X coordinate of the key in the keyboard layout
  int kx;
  // Y coordinate of the key in the keyboard layout
  int ky;
  // Hit bounding box of the key
  int khit_box_left;
  int khit_box_top;
  int khit_box_right;
  int khit_box_bottom;

  Key() {}

  Key(const int code,
      const int x,
      const int y,
      const int width,
      const int height,
      const int horizontal_gap,
      const int vertical_gap) {
    kheight = height - vertical_gap;
    kwidth = width - horizontal_gap;
    kcode = code;
    kx = x + horizontal_gap / 2;
    ky = y;
    khit_box_left = x;
    khit_box_top = y;
    khit_box_right = x + width + 1;
    khit_box_bottom = y + height;
  }
};

namespace keyset {

const Key A(97, 43, 58, 29, 58, 4, 9);
const Key B(98, 188, 116, 29, 58, 4, 9);
const Key C(99, 130, 116, 29, 58, 4, 9);
const Key D(100, 101, 58, 29, 58, 4, 9);
const Key E(101, 87, 0, 29, 58, 4, 9);
const Key F(102, 130, 58, 29, 58, 4, 9);
const Key G(103, 159, 58, 29, 58, 4, 9);
const Key H(104, 188, 58, 29, 58, 4, 9);
const Key I(105, 232, 0, 29, 58, 4, 9);
const Key J(106, 217, 58, 29, 58, 4, 9);
const Key K(107, 246, 58, 29, 58, 4, 9);
const Key L(108, 275, 58, 29, 58, 4, 9);
const Key M(109, 246, 116, 29, 58, 4, 9);
const Key N(110, 217, 116, 29, 58, 4, 9);
const Key O(111, 261, 0, 29, 58, 4, 9);
const Key P(112, 290, 0, 29, 58, 4, 9);
const Key Q(113, 29, 0, 29, 58, 4, 9);
const Key R(114, 116, 0, 29, 58, 4, 9);
const Key S(115, 72, 58, 29, 58, 4, 9);
const Key T(116, 145, 0, 29, 58, 4, 9);
const Key U(117, 203, 0, 29, 58, 4, 9);
const Key V(118, 159, 116, 29, 58, 4, 9);
const Key W(119, 58, 0, 29, 58, 4, 9);
const Key X(120, 101, 116, 29, 58, 4, 9);
const Key Y(121, 174, 0, 29, 58, 4, 9);
const Key Z(122, 72, 116, 29, 58, 4, 9);

const Key key_set[] = {Q, W, E, R, T, Y, U, I, O, P, A, S, D,
                       F, G, H, J, K, L, Z, X, C, V, B, N, M};

const int key_count = 26;

}  // namespace keyset
}  // namespace prediction

#endif  // SERVICES_PREDICTION_KEY_SET_H_
