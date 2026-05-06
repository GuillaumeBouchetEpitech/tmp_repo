
#include "NeuralNetwork.hpp"

#include "RandomNumberGenerator.hpp"

#include <stdexcept>
#include <cmath>

namespace {

// /**
//  * it's a steeper sigmoid
//  * => input:  [-x..x]
//  * => output: [0..1]
//  *
//  * Notes:
//  * => use "desmos.com" to visualise the curve
//  * => link: https://www.desmos.com/calculator
//  */
// float
// _steeperSigmoid(float x) {
//   return 1.0f / (1.0f + std::exp(-4.9f * x));
// }


float
_vanillaSigmoid(float x) {
  return 1.0f / (1.0f + std::exp(-x));
}

};

//MARK:NeuralNetworkTopology
NeuralNetworkTopology::NeuralNetworkTopology(const std::initializer_list<int32_t>& inList)
{
  if (inList.size() < 2) {
    throw std::invalid_argument("invalid topology length for NeuralNetwork");
  }
  for (int32_t currSize : inList) {
    if (currSize < 1) {
      throw std::invalid_argument("invalid topology layer size for NeuralNetwork");
    }
  }

  _rawValues.reserve(inList.size());
  for (auto currSize : inList) {
    _rawValues.push_back(uint32_t(currSize));
  }

  {
    // compute total weights
    // uint32_t prev_layer_num_neuron = 0;
    for (std::size_t ii = 1; ii < _rawValues.size(); ++ii) {
      const uint32_t numInputs = _rawValues.at(ii - 1);
      const uint32_t numNeuron = _rawValues.at(ii);
      // input synapse weights
      // _totalWeights += prev_layer_numNeuron * numNeuron;
      _totalWeights += numNeuron * numInputs;
      // bias synapse weights
      _totalWeights += numNeuron;
      //
      // prev_layer_numNeuron = numNeuron;
    }
  }

  // {
  //   // compute total neurons
  //   for (std::size_t ii = 0; ii < _rawValues.size(); ++ii) {
  //     const uint32_t num_neuron = _rawValues.at(ii);
  //     _totalNeurons += num_neuron;
  //   }
  // }
}

//MARK:NeuralNetworkNeuron
NeuralNetworkNeuron::NeuralNetworkNeuron(std::size_t numInputs)
{
  this->_inputSynapseWeights.reserve(numInputs);
  for (std::size_t ii = 0; ii < numInputs; ++ii) {
    this->_inputSynapseWeights.push_back(RandomNumberGenerator::getRangedValue(-0.5f, 0.5f));
  }

  this->_biasSynapseWeight = RandomNumberGenerator::getRangedValue(-0.5f, 0.5f);
}


//MARK:NeuralNetwork
NeuralNetwork::NeuralNetwork(const NeuralNetworkTopology& topology)
  : _topology(topology)
{
  RandomNumberGenerator::ensureRandomSeed();

  this->_layers.reserve(topology.getRawValues().size() - 1);
  for (std::size_t layerIndex = 1; layerIndex < topology.getRawValues().size(); ++layerIndex) {
    const uint32_t numInputs = this->_topology.getRawValues().at(layerIndex - 1);
    const uint32_t numNeurons = this->_topology.getRawValues().at(layerIndex);

    NeuralNetworkLayer newLayer;
    newLayer.neurons.reserve(numNeurons);
    for (uint32_t neuronIndex = 0; neuronIndex < numNeurons; ++neuronIndex) {
      newLayer.neurons.emplace_back(numInputs);
    }

    this->_layers.push_back(std::move(newLayer));

    // this->_layers.push_back({
    //   neurons: Array.from({ length: numNeurons }, () => {
    //     return new Neuron(numInputs);
    //   }),
    // });

  }
}

//MARK:forward
void NeuralNetwork::forward(const std::vector<float>& inputs, std::vector<float>& outputs)
{
  if (inputs.size() != this->_topology.getRawValues().at(0)) {
    throw std::invalid_argument("invalid input length");
  }

  std::vector<float> currentInputs = inputs; // TODO: hard copy
  for (auto& currLayer : this->_layers) {
    std::vector<float> currentOutputs; // TODO: inefficient
    for (auto& currNeuron : currLayer.neurons) {

      // since a bias neuron value is always "1" -> directly use the weight
      float sum = currNeuron.getBiasSynapseWeight();

      // sum of "input synapses weight x input neuron values"
      for (std::size_t ii = 0; ii < currentInputs.size(); ++ii) {
        sum += currNeuron.getInputSynapseWeights().at(ii) * currentInputs.at(ii);
      }

      // apply the squashing/activation function
      const float outputValue = _vanillaSigmoid(sum);
      currNeuron.setLastOutput(outputValue);
      currentOutputs.push_back(outputValue);
    }
    currentInputs = currentOutputs; // TODO: hard copy
  }
  // at this point in the logic -> currentInputs is the "output layers" outputs

  outputs.reserve(currentInputs.size());
  for (float currValue : currentInputs) {
    outputs.push_back(currValue);
  }

}





