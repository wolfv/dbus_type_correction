// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <cmath>
#include <new>
#include <vector>

#include "proximity_info_factory.h"
#include "touch_position_correction.h"

// NOTE: This class has been translated to C++ and modified from the Android
// Open Source Project. Specifically from some functions of the following file:
// https://android.googlesource.com/platform/packages/inputmethods/LatinIME/+/
// android-5.1.1_r8/java/src/com/android/inputmethod/keyboard/ProximityInfo.java

namespace prediction {

const float ProximityInfoFactory::SEARCH_DISTANCE = 1.2f;

const float ProximityInfoFactory::DEFAULT_TOUCH_POSITION_CORRECTION_RADIUS =
    0.15f;

// Hardcoded qwerty keyboard proximity settings
ProximityInfoFactory::ProximityInfoFactory() {
  plocale_ = "en";
  pgrid_width_ = 32;
  pgrid_height_ = 16;
  pgrid_size_ = pgrid_width_ * pgrid_height_;
  pcell_width_ = (348 + pgrid_width_ - 1) / pgrid_width_;
  pcell_height_ = (174 + pgrid_height_ - 1) / pgrid_height_;
  pkeyboard_min_width_ = 348;
  pkeyboard_height_ = 174;
  pmost_common_key_height_ = 29;
  pmost_common_key_width_ = 58;
}

ProximityInfoFactory::~ProximityInfoFactory() {
}

latinime::ProximityInfo* ProximityInfoFactory::GetNativeProximityInfo() {
  const int default_width = pmost_common_key_width_;
  const int threshold = (int)(default_width * SEARCH_DISTANCE);
  const int threshold_squared = threshold * threshold;
  const int last_pixel_x_coordinate = pgrid_width_ * pcell_width_ - 1;
  const int last_pixel_y_coordinate = pgrid_height_ * pcell_height_ - 1;

  std::vector<Key> pgrid_neighbors[32 * 16 /*pgrid_size_*/];
  int neighbor_count_per_cell[pgrid_size_];
  std::fill_n(neighbor_count_per_cell, pgrid_size_, 0);
  Key neighbors_flat_buffer[32 * 16 * 26 /*pgrid_size_ * keyset::key_count*/];

  const int half_cell_width = pcell_width_ / 2;
  const int half_cell_height = pcell_height_ / 2;
  for (int i = 0; i < keyset::key_count; i++) {
    const Key key = keyset::key_set[i];

    const int key_x = key.kx;
    const int key_y = key.ky;
    const int top_pixel_within_threshold = key_y - threshold;
    const int y_delta_to_grid = top_pixel_within_threshold % pcell_height_;
    const int y_middle_of_top_cell =
        top_pixel_within_threshold - y_delta_to_grid + half_cell_height;
    const int y_start =
        std::max(half_cell_height,
                 y_middle_of_top_cell +
                     (y_delta_to_grid <= half_cell_height ? 0 : pcell_height_));
    const int y_end =
        std::min(last_pixel_y_coordinate, key_y + key.kheight + threshold);

    const int left_pixel_within_threshold = key_x - threshold;
    const int x_delta_to_grid = left_pixel_within_threshold % pcell_width_;
    const int x_middle_of_left_cell =
        left_pixel_within_threshold - x_delta_to_grid + half_cell_width;
    const int x_start =
        std::max(half_cell_width,
                 x_middle_of_left_cell +
                     (x_delta_to_grid <= half_cell_width ? 0 : pcell_width_));
    const int x_end =
        std::min(last_pixel_x_coordinate, key_x + key.kwidth + threshold);

    int base_index_of_current_row =
        (y_start / pcell_height_) * pgrid_width_ + (x_start / pcell_width_);
    for (int center_y = y_start; center_y <= y_end; center_y += pcell_height_) {
      int index = base_index_of_current_row;
      for (int center_x = x_start; center_x <= x_end;
           center_x += pcell_width_) {
        if (SquaredDistanceToEdge(center_x, center_y, key) <
            threshold_squared) {
          neighbors_flat_buffer[index * keyset::key_count +
                                neighbor_count_per_cell[index]] =
              keyset::key_set[i];
          ++neighbor_count_per_cell[index];
        }
        ++index;
      }
      base_index_of_current_row += pgrid_width_;
    }
  }

  for (int i = 0; i < pgrid_size_; ++i) {
    const int index_start = i * keyset::key_count;
    const int index_end = index_start + neighbor_count_per_cell[i];
    for (int index = index_start; index < index_end; index++) {
      pgrid_neighbors[i].push_back(neighbors_flat_buffer[index]);
    }
  }

  int proximity_chars_array[pgrid_size_ * MAX_PROXIMITY_CHARS_SIZE];
  for (int i = 0; i < pgrid_size_; i++) {
    int info_index = i * MAX_PROXIMITY_CHARS_SIZE;
    for (int j = 0; j < neighbor_count_per_cell[i]; j++) {
      Key neighbor_key = pgrid_neighbors[i][j];
      proximity_chars_array[info_index] = neighbor_key.kcode;
      info_index++;
    }
  }

  int key_x_coordinates[keyset::key_count];
  int key_y_coordinates[keyset::key_count];
  int key_widths[keyset::key_count];
  int key_heights[keyset::key_count];
  int key_char_codes[keyset::key_count];
  float sweet_spot_center_xs[keyset::key_count];
  float sweet_spot_center_ys[keyset::key_count];
  float sweet_spot_radii[keyset::key_count];

  for (int key_index = 0; key_index < keyset::key_count; key_index++) {
    Key key = keyset::key_set[key_index];
    key_x_coordinates[key_index] = key.kx;
    key_y_coordinates[key_index] = key.ky;
    key_widths[key_index] = key.kwidth;
    key_heights[key_index] = key.kheight;
    key_char_codes[key_index] = key.kcode;
  }

  TouchPositionCorrection touch_position_correction;
  if (touch_position_correction.IsValid()) {
    const int rows = touch_position_correction.GetRows();
    const float default_radius =
        DEFAULT_TOUCH_POSITION_CORRECTION_RADIUS *
        (float)std::hypot(pmost_common_key_width_, pmost_common_key_height_);
    for (int key_index = 0; key_index < keyset::key_count; key_index++) {
      Key key = keyset::key_set[key_index];
      sweet_spot_center_xs[key_index] =
          (key.khit_box_left + key.khit_box_right) * 0.5f;
      sweet_spot_center_ys[key_index] =
          (key.khit_box_top + key.khit_box_bottom) * 0.5f;
      sweet_spot_radii[key_index] = default_radius;
      const int row = key.khit_box_top / pmost_common_key_height_;
      if (row < rows) {
        const int hit_box_width = key.khit_box_right - key.khit_box_left;
        const int hit_box_height = key.khit_box_bottom - key.khit_box_top;
        const float hit_box_diagonal =
            (float)std::hypot(hit_box_width, hit_box_height);
        sweet_spot_center_xs[key_index] +=
            touch_position_correction.GetX(row) * hit_box_width;
        sweet_spot_center_ys[key_index] +=
            touch_position_correction.GetY(row) * hit_box_height;
        sweet_spot_radii[key_index] =
            touch_position_correction.GetRadius(row) * hit_box_diagonal;
      }
    }
  }

  latinime::ProximityInfo* proximity_info = new latinime::ProximityInfo(
      plocale_, pkeyboard_min_width_, pkeyboard_height_, pgrid_width_,
      pgrid_height_, pmost_common_key_width_, pmost_common_key_height_,
      proximity_chars_array, pgrid_size_ * MAX_PROXIMITY_CHARS_SIZE,
      keyset::key_count, key_x_coordinates, key_y_coordinates, key_widths,
      key_heights, key_char_codes, sweet_spot_center_xs, sweet_spot_center_ys,
      sweet_spot_radii);

  return proximity_info;
}

int ProximityInfoFactory::SquaredDistanceToEdge(int x, int y, Key k) {
  const int left = k.kx;
  const int right = left + k.kwidth;
  const int top = k.ky;
  const int bottom = top + k.kheight;
  const int edge_x = x < left ? left : (x > right ? right : x);
  const int edge_y = y < top ? top : (y > bottom ? bottom : y);
  const int dx = x - edge_x;
  const int dy = y - edge_y;
  return dx * dx + dy * dy;
}

}  // namespace prediction
