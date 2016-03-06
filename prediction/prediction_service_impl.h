// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_PREDICTION_PREDICTION_SERVICE_IMPL_H_
#define SERVICES_PREDICTION_PREDICTION_SERVICE_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "mojo/services/prediction/interfaces/prediction.mojom.h"

namespace prediction {

class PredictionServiceImpl : public PredictionService {
 public:
  explicit PredictionServiceImpl(
      mojo::InterfaceRequest<PredictionService> request);
  ~PredictionServiceImpl() override;

  // PredictionService implementation
  void GetPredictionList(PredictionInfoPtr prediction_info,
                         const GetPredictionListCallback& callback) override;

 private:
  mojo::StrongBinding<PredictionService> strong_binding_;

  scoped_ptr<latinime::ProximityInfo> proximity_settings_;
  DictionaryService dictionary_service_;

  DISALLOW_COPY_AND_ASSIGN(PredictionServiceImpl);
};

class PredictionServiceDelegate
    : public mojo::ApplicationDelegate,
      public mojo::InterfaceFactory<PredictionService> {
 public:
  PredictionServiceDelegate();
  ~PredictionServiceDelegate() override;

  // mojo::ApplicationDelegate implementation
  bool ConfigureIncomingConnection(
      mojo::ApplicationConnection* connection) override;

  // mojo::InterfaceRequest<PredictionService> implementation
  void Create(mojo::ApplicationConnection* connection,
              mojo::InterfaceRequest<PredictionService> request) override;
};

}  // namespace prediction

#endif  // SERVICES_PREDICTION_PREDICTION_SERVICE_IMPL_H_
