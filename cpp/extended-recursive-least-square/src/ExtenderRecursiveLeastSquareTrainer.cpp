
#include "ExtenderRecursiveLeastSquareTrainer.hpp"

#include <stdexcept>
#include <iostream>
#include <iomanip>

//MARK:ctor
ExtenderRecursiveLeastSquareTrainer::ExtenderRecursiveLeastSquareTrainer(const ExtenderRecursiveLeastSquareTrainerDef& def)
  : _def(def)
{
  if (this->_def.lambda < 0.0f || this->_def.lambda > 1.0f) {
    throw std::invalid_argument("lambda value cannot be outside the range [0..1]");
  }
  if (this->_def.windowSize < 10U) {
    throw std::invalid_argument("windowSize value cannot be bellow 10");
  }

  // Initialises P = 100 * I for each output, expressing high initial uncertainty
  // so the first few samples dominate the parameter estimates.
  constexpr float k_scale = 100.0f;
  const auto& topology = this->_def.network->getTopology();
  const uint32_t totalWeights = topology.getTotalWeights();
  const uint32_t totalOutputs = topology.getRawValues().back();
  this->_perOutputMatrixP.reserve(std::size_t(totalOutputs));
  for (uint32_t ii = 0; ii < totalOutputs; ++ii) {
    this->_perOutputMatrixP.emplace_back(totalWeights, k_scale);
  }

}

//MARK:trainSample
float ExtenderRecursiveLeastSquareTrainer::trainSample(const TrainingSample& inSample)
{
  const auto& topology = this->_def.network->getTopology();
  if (topology.getRawValues().front() != inSample.inputs.size()) {
    throw std::invalid_argument("invalid sample inputs length");
  }
  if (topology.getRawValues().back() != inSample.targets.size()) {
    throw std::invalid_argument("invalid sample outputs length");
  }

  std::vector<float> tmpOutputs;
  this->_def.network->forward(inSample.inputs, tmpOutputs);

  struct LayerOutputs {
    std::vector<float> neuronsOutputs;
  };
  std::vector<LayerOutputs> allNeuronsOutputs;
  allNeuronsOutputs.push_back({ inSample.inputs }); // TODO: hard copy
  for (const auto& currLayer : this->_def.network->getLayers()) {
    std::vector<float> allOutputs;
    allOutputs.reserve(currLayer.neurons.size());
    for (auto& currNeuron : currLayer.neurons) {
      allOutputs.push_back(currNeuron.getLastOutput());
    }
    allNeuronsOutputs.push_back({ allOutputs }); // TODO: hard copy
  }

  const std::size_t numLayers = this->_def.network->getLayers().size();
  const LayerOutputs& outputLayerOutputs = allNeuronsOutputs.back(); // post-activation outputs of last layer

  float squaredErrorSum = 0.0f;

  for (std::size_t currTargetIndex = 0; currTargetIndex < inSample.targets.size(); ++currTargetIndex) {

    const float currExpected = inSample.targets.at(currTargetIndex);
    const float currOutput = outputLayerOutputs.neuronsOutputs.at(currTargetIndex);

    //
    // --- Build Jacobian phi_k = d(y_k)/d(theta) via chain rule ---
    //

    struct LayerDeltas {
      std::vector<float> neuronsDeltas;
    };

    std::vector<LayerDeltas> allLayersDeltas;
    allLayersDeltas.resize(numLayers);

    // Output layer: only neuron [currTargetIndex] has a non-zero delta; all others are 0.
    allLayersDeltas.back().neuronsDeltas.resize(this->_def.network->getLayers().back().neurons.size(), 0.0f);
    allLayersDeltas.back().neuronsDeltas.at(currTargetIndex) = currOutput * (1.0f - currOutput);

    // Propagate deltas backward through hidden layers
    for (int32_t layerIndex = int32_t(numLayers) - 2; layerIndex >= 0; --layerIndex) {
      const auto& nextNetworkLayer = this->_def.network->getLayers().at(layerIndex + 1);
      const auto& nextLayerDeltas = allLayersDeltas.at(layerIndex + 1).neuronsDeltas;
      const auto& nextLayerOutputs = allNeuronsOutputs.at(layerIndex + 1);

      // delta[layerIndex][inputNeuronIndex] = sigmoid'(y_n) * sum_j( w[layerIndex+1][j][inputNeuronIndex] * delta[layerIndex+1][j] )

      std::vector<float> currLayerDeltas;
      currLayerDeltas.reserve(nextLayerOutputs.neuronsOutputs.size());
      for (std::size_t inputNeuronIndex = 0; inputNeuronIndex < nextLayerOutputs.neuronsOutputs.size(); ++inputNeuronIndex) {
        const float outputVal = nextLayerOutputs.neuronsOutputs.at(inputNeuronIndex);
        float weightSum = 0.0f;
        for (std::size_t outputNeuronIndex = 0; outputNeuronIndex < nextLayerDeltas.size(); ++outputNeuronIndex) {
          weightSum += nextNetworkLayer.neurons.at(outputNeuronIndex).getInputSynapseWeights().at(inputNeuronIndex) * nextLayerDeltas.at(outputNeuronIndex);
        }
        currLayerDeltas.push_back(outputVal * (1.0f - outputVal) * weightSum);
      }
      allLayersDeltas.at(layerIndex).neuronsDeltas = currLayerDeltas;
    }

    //
    // Flatten Jacobian: phi[weight      layerIndex,neuronIndex,inNeuronIndex] = delta[layerIndex][neuronIndex] * activation[layerIndex][inNeuronIndex]
    //                   phi[bias weight layerIndex,neuronIndex]               = delta[layerIndex][neuronIndex]
    //

    // the phi array hold the gradient values
    // -> the gradient value represent how much each connections influenced this output value
    // -> The ordering here must match the parameter traversal order used when
    //    later on we apply the "K * e", so both loops walk layers -> neurons -> inputs.
    std::vector<float> phi;
    phi.reserve(this->_def.network->getTopology().getTotalWeights());
    for (std::size_t layerIndex = 0; layerIndex < numLayers; ++layerIndex) {
      const auto& currLayer = this->_def.network->getLayers().at(layerIndex);
      const auto& layerInputs = allNeuronsOutputs.at(layerIndex);
      for (std::size_t currNeuronIndex = 0; currNeuronIndex < currLayer.neurons.size(); ++currNeuronIndex) {
        const float currNeuronDelta = allLayersDeltas.at(layerIndex).neuronsDeltas.at(currNeuronIndex);
        for (std::size_t inputNeuronIndex = 0; inputNeuronIndex < currLayer.neurons.at(currNeuronIndex).getInputSynapseWeights().size(); ++inputNeuronIndex) {
          // input connection gradient
          phi.push_back(currNeuronDelta * layerInputs.neuronsOutputs.at(inputNeuronIndex));
        }
        // bias connection gradient (since the input value is "1" we just the delta)
        phi.push_back(currNeuronDelta);
      }
    }

    //
    // --- RLS update ---
    //

    const auto& matrixP = this->_perOutputMatrixP.at(currTargetIndex);
    // P_mul_Phi = P * phi  (numerator of the Kalman gain)
    std::vector<float> P_mul_Phi;
    matrixP.mulVec(phi, P_mul_Phi);
    // Scalar denominator: lambda + phi^T * P * phi
    float phiTP_mul_Phi = 0.0f;
    for (std::size_t ii = 0; ii < phi.size(); ++ii) {
      phiTP_mul_Phi += phi.at(ii) * P_mul_Phi.at(ii);
    }

    // Kalman gain vector K (one entry per parameter)
    // -> The ordering here must match the parameter traversal order used when computing the phi values
    std::vector<float> K;
    K.reserve(P_mul_Phi.size());
    for (float currVal : P_mul_Phi) {
      K.push_back(currVal / (this->_def.lambda + phiTP_mul_Phi));
    }

    // Prediction error for current output
    const float errorVal = currExpected - currOutput;
    squaredErrorSum += errorVal * errorVal;

    // std::cout << std::fixed << "_ errorVal=" << errorVal << ", squaredErrorSum=" << squaredErrorSum << std::endl;

    //
    // Update the neural network
    // -> Update all weights and biases weights.
    //

    // idx walks the same parameter order used when building phi.
    // -> which is why ordering mattered so much until now
    std::size_t idx = 0;
    for (const auto& layer :  this->_def.network->getLayers()) {
      for (const auto& currNeuron : layer.neurons) {
        for (std::size_t inputNeuronIndex = 0; inputNeuronIndex < currNeuron.getInputSynapseWeights().size(); inputNeuronIndex++) {
          // input weight update
          currNeuron.updateInputSynapseWeights(inputNeuronIndex, K.at(idx++) * errorVal);
        }
        // bias weight update
        currNeuron.updateBiasSynapseWeight(K.at(idx++) * errorVal);
      }
    }

    //
    // Update the inverse covariance matrix
    //

    // here the final use-case of why K and phi must have a matching ordering
    auto& nonconst_matrixP = this->_perOutputMatrixP.at(currTargetIndex);
    nonconst_matrixP.updateMatrix(K, phi, this->_def.lambda);
  }

  // return this sample's MSE (sum of squared errors / num outputs)
  return squaredErrorSum / float(inSample.targets.size());
}

//MARK:trainEpoch
// Runs one full pass over the dataset, updating parameters for each sample.
// Returns the mean squared error averaged across all samples in the epoch.
// The MSE is appended to the rolling error window used by isConverged().
float ExtenderRecursiveLeastSquareTrainer::trainEpoch(const std::vector<TrainingSample>& inDataset)
{
  float allEpochsMSE = 0.0f;
  for (const auto& currSample : inDataset) {
    allEpochsMSE += this->trainSample(currSample);
  }
  allEpochsMSE /= inDataset.size();

  // Append to rolling window, dropping the oldest entry when full.
  if (this->_epochErrors.size() >= this->_def.windowSize) {
    this->_epochErrors.pop_front();
  }
  this->_epochErrors.push_back(allEpochsMSE);

  if (this->_epochErrors.size() >= this->_def.windowSize) {
    // const max = Math.max(...this->_epochErrors);
    // const min = Math.min(...this->_epochErrors);
    // this->_averageError = (max - min);

    this->_averageError = 0.0f;
    for (auto errVal : this->_epochErrors) {
      this->_averageError += errVal;
    }
    this->_averageError /= float(this->_epochErrors.size());

    // std::cout << " -> allEpochsMSE=" << std::fixed << allEpochsMSE << ", averageError=" << std::fixed << this->_averageError << std::endl;
  // } else {
  //   std::cout << " -> allEpochsMSE=" << std::fixed << allEpochsMSE << std::endl;
  }

  return allEpochsMSE;
}

//MARK:isConverged
bool ExtenderRecursiveLeastSquareTrainer::isConverged(float inThreshold /*= 1e-4*/) const
{
  if (this->_epochErrors.size() < this->_def.windowSize)
  {
    // not yet stable enough
    return false;
  }
  // stable enough, can be compared
  return this->_averageError < inThreshold;
}
