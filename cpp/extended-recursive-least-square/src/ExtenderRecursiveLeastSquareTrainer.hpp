
#pragma once

#include "NeuralNetwork.hpp"

#include "ExtenderRecursiveLeastSquareMatrix.hpp"

#include <vector>
#include <list>
#include <memory>

struct TrainingSample {
  std::vector<float> inputs;
  std::vector<float> targets;
};

struct ExtenderRecursiveLeastSquareTrainerDef {

  std::shared_ptr<NeuralNetwork> network;

  // The forgetting factor (0 < lambda <= 1).
  // -> Values to 1.0: means no forgetting (standard RLS).
  // -> Values < 1.0: discount older samples -> useful for non-stationary targets.
  float lambda;
  // windowSize controls how many recent epoch errors are retained for convergence checks.
  std::size_t windowSize;
};

class ExtenderRecursiveLeastSquareTrainer
{
public:
  ExtenderRecursiveLeastSquareTrainer(const ExtenderRecursiveLeastSquareTrainerDef& def);

public:
  float trainSample(const TrainingSample& inSample);
  float trainEpoch(const std::vector<TrainingSample>& inDataset);

public:
  // Returns true when the rolling error window is full and the error has stabilised:
  // -> the difference between the highest and lowest recent epoch MSE is below threshold.
  // -> A small spread means error has stopped improving significantly.
  bool isConverged(float inThreshold = 1e-4) const;

  // The MSE recorded for each of the most recent epochs (up to windowSize entries).
  const std::list<float> lastEpochErrors() const { return this->_epochErrors; }
  float averageError() const { return this->_averageError; }

private:
  ExtenderRecursiveLeastSquareTrainerDef _def;

  std::vector<ExtenderRecursiveLeastSquareMatrix> _perOutputMatrixP;

  std::list<float> _epochErrors;
  float _averageError = -1.0f;

};
