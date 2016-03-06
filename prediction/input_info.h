// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_PREDICTION_INPUT_INFO_H_
#define SERVICES_PREDICTION_INPUT_INFO_H_
#include <string>
// #include "mojo/services/prediction/interfaces/prediction.mojom.h";

namespace prediction {

class InputInfo {
 public:
  InputInfo(std::string& input, int input_size);
  ~InputInfo();

  int* GetCodepoints();
  int* GetXCoordinates();
  int* GetYCoordinates();
  int* GetPointerIds();
  int* GetTimes();
  int GetRealSize();

 private:
  void ProcessInput(std::string& input, int input_size);

  int real_size_;
  int* codepoints_;
  int* x_coordinates_;
  int* y_coordinates_;
  int* pointer_ids_;
  int* times_;

};  // class InputInfo

}  // namespace prediction

#endif  // SERVICES_PREDICTION_INPUT_INFO_H_
