
#include "NeuralNetwork.hpp"

#include "RandomNumberGenerator.hpp"
#include "utils/clamp.hpp"
#include "utils/asValue.hpp"

#include <stdexcept>
#include <cmath>

namespace {

// // /**
// //  * it's a steeper sigmoid
// //  * => input:  [-x..x]
// //  * => output: [0..1]
// //  *
// //  * Notes:
// //  * => use "desmos.com" to visualise the curve
// //  * => link: https://www.desmos.com/calculator
// //  */
// // float
// // _steeperSigmoid(float x) {
// //   return 1.0f / (1.0f + std::exp(-4.9f * x));
// // }


// float
// _vanillaSigmoid(float x) {
//   return 1.0f / (1.0f + std::exp(-x));
// }

// float
// _restrictedRELU(float x) {
//   return utils::clamp(x, 0.0f, 1.0f);
// }

// float
// _restrictedleakyReLU(float x) {
//   float output = x;
//   if (output < 0.0f) {
//     output *= 0.1f;
//   }
//   return utils::clamp(output, -10.0f, 10.0f);
// }

constexpr float k_weightRange = 1000.0f;
// constexpr float k_weightRange = 100.0f;
// constexpr float k_weightRange = 10.0f;

};

//MARK:AllActivations
namespace AllActivations {

  namespace sigmoid {
    float activate(float x) {
      // [0..1]
      return 1.0f / (1.0f + std::exp(-x));
    }
    float derive(float x) {
      return x * (1.0f - x);
    }
    float initWeight()
    {
      // const float rawValue = RandomNumberGenerator::getRangedValue(0.05f, 0.10f);
      // // const float rawValue = RandomNumberGenerator::getRangedValue(0.5f, 0.9f);
      // const bool isNegative = RandomNumberGenerator::getRangedValue(0.0f, 1.0f) < 0.5f;
      // // const float newValue = rawValue * (isNegative ? -1.0f : 1.0f);
      // return rawValue * (isNegative ? -1.0f : 1.0f);
      return RandomNumberGenerator::getRangedValue(-0.2f, 0.2f);
    }
  };

  namespace linear {
    float activate(float x) {
      // return x;
      return utils::clamp(x, -3.0f, 3.0f);
    }
    float derive(float x) {
      return 1.0f;
    }
    float initWeight()
    {
      const float rawValue = RandomNumberGenerator::getRangedValue(0.50f, 0.99f);
      const bool isNegative = RandomNumberGenerator::getRangedValue(0.0f, 1.0f) < 0.5f;
      return rawValue * (isNegative ? -1.0f : 1.0f);
    }
  };

  namespace ReLU {
    float activate(float x) {
      // [0..x]
      return std::max(x, 0.0f);
      // return utils::clamp(x, 0.0f, 3.0f);
    }
    float derive(float x) {
      return (x < 0.0f) ? 0.0f : 1.0f;
    }
    float initWeight()
    {
      // const float rawValue = RandomNumberGenerator::getRangedValue(0.01f, 0.05f);
      // const bool isNegative = RandomNumberGenerator::getRangedValue(0.0f, 1.0f) < 0.5f;
      // return rawValue * (isNegative ? -1.0f : 1.0f);
      // // return rawValue;
      return RandomNumberGenerator::getRangedValue(-0.2f, 0.2f);
      // return 0.0f;
    }
  };

  namespace leakyReLU {
    float activate(float x) {
      // [-0.1*x..x]
      return (x < 0.0f) ? 0.1f * x : x;
      // return utils::clamp((x < 0.0f) ? 0.1f * x : x, -3.0f, 3.0f);
    }
    float derive(float x) {
      return (x < 0.0f) ? 0.1f : 1.0f;
    }
    float initWeight()
    {
      const float rawValue = RandomNumberGenerator::getRangedValue(0.05f, 0.10f);
      // const float rawValue = RandomNumberGenerator::getRangedValue(0.5f, 0.9f);
      const bool isNegative = RandomNumberGenerator::getRangedValue(-1.0f, 1.0f) < 0.0f;
      // const float newValue = rawValue * (isNegative ? -1.0f : 1.0f);
      return rawValue * (isNegative ? -1.0f : 1.0f);
      // this->_inputSynapseWeights.push_back(newValue);
      // this->_inputSynapseWeights.push_back(RandomNumberGenerator::getRangedValue(0.5f, 0.9f));
    }
  };

  static std::array<ActivationType, 4> s_asArray = {{
    { &sigmoid::activate, &sigmoid::derive, &sigmoid::initWeight },
    { &linear::activate, &linear::derive, &linear::initWeight },
    { &ReLU::activate, &ReLU::derive, &ReLU::initWeight },
    { &leakyReLU::activate, &leakyReLU::derive, &leakyReLU::initWeight }
  }};

  const ActivationType& fromEnum(NeuralNetworkActivations type)
  {
    return s_asArray.at(utils::asValue(type));
  }

};


//MARK:NeuralNetworkTopology
// NeuralNetworkTopology::NeuralNetworkTopology(const std::initializer_list<int32_t>& inList)
// {
//   if (inList.size() < 2) {
//     throw std::invalid_argument("invalid topology length for NeuralNetwork");
//   }
//   for (int32_t currSize : inList) {
//     if (currSize < 1) {
//       throw std::invalid_argument("invalid topology layer size for NeuralNetwork");
//     }
//   }

//   _rawValues.reserve(inList.size());
//   for (auto currSize : inList) {
//     _rawValues.push_back(uint32_t(currSize));
//   }

//   {
//     // compute total weights
//     // uint32_t prev_layer_num_neuron = 0;
//     for (std::size_t ii = 1; ii < _rawValues.size(); ++ii) {
//       const uint32_t numInputs = _rawValues.at(ii - 1);
//       const uint32_t numNeuron = _rawValues.at(ii);
//       // input synapse weights
//       // _totalWeights += prev_layer_numNeuron * numNeuron;
//       _totalWeights += numNeuron * numInputs;
//       // bias synapse weights
//       _totalWeights += numNeuron;
//       //
//       // prev_layer_numNeuron = numNeuron;
//     }
//   }

//   // {
//   //   // compute total neurons
//   //   for (std::size_t ii = 0; ii < _rawValues.size(); ++ii) {
//   //     const uint32_t num_neuron = _rawValues.at(ii);
//   //     _totalNeurons += num_neuron;
//   //   }
//   // }
// }

NeuralNetworkTopologyBuilder& NeuralNetworkTopologyBuilder::clear()
{
  this->_topology._rawValues.clear();
  this->_topology._totalWeights = 0;
  return *this;
}

NeuralNetworkTopologyBuilder& NeuralNetworkTopologyBuilder::addInputLayer(uint32_t numNeurons)
{
  if (this->_topology._rawValues.empty() == false) {
    throw std::invalid_argument("input layer must be added first in a neural network topology");
  }
  if (numNeurons == 0) {
    throw std::invalid_argument("total neurons cannot be 0 for layers in a neural network topology");
  }
  this->_topology._rawValues.push_back({ NeuralNetworkLayerDef::AllTypes::input, numNeurons });
  return *this;
}

NeuralNetworkTopologyBuilder& NeuralNetworkTopologyBuilder::addHiddenLayer(uint32_t numNeurons, NeuralNetworkActivations activation /*= NeuralNetworkActivations::sigmoid*/)
{
  if (this->_topology._rawValues.empty() == true) {
    throw std::invalid_argument("hidden layer must be added after an input layer in a neural network topology");
  }
  if (this->_topology._rawValues.back().type == NeuralNetworkLayerDef::AllTypes::output) {
    throw std::invalid_argument("hidden layer cannot be added after an output layer in a neural network topology");
  }
  if (numNeurons == 0) {
    throw std::invalid_argument("total neurons cannot be 0 for layers in a neural network topology");
  }
  this->_topology._rawValues.push_back({ NeuralNetworkLayerDef::AllTypes::input, numNeurons, activation });
  return *this;
}

NeuralNetworkTopologyBuilder& NeuralNetworkTopologyBuilder::addOutputLayer(uint32_t numNeurons, NeuralNetworkActivations activation /*= NeuralNetworkActivations::sigmoid*/)
{
  if (this->_topology._rawValues.empty() == true) {
    throw std::invalid_argument("output layer must be added after an input layer in a neural network topology");
  }
  if (this->_topology._rawValues.back().type == NeuralNetworkLayerDef::AllTypes::output) {
    throw std::invalid_argument("must only add one output layer in a neural network topology");
  }
  if (numNeurons == 0) {
    throw std::invalid_argument("total neurons cannot be 0 for layers in a neural network topology");
  }
  this->_topology._rawValues.push_back({ NeuralNetworkLayerDef::AllTypes::input, numNeurons, activation });
  return *this;
}

NeuralNetworkTopology NeuralNetworkTopologyBuilder::build()
{
  if (this->_topology._rawValues.size() < 2) {
    throw std::invalid_argument("must have at least 2 layers in a neural network topology");
  }

  {
    // compute total weights
    // uint32_t prev_layer_num_neuron = 0;
    for (std::size_t ii = 1; ii < this->_topology._rawValues.size(); ++ii) {
      const uint32_t numInputs = this->_topology._rawValues.at(ii - 1).numNeurons;
      const uint32_t numNeuron = this->_topology._rawValues.at(ii).numNeurons;
      // input synapse weights
      // _totalWeights += prev_layer_numNeuron * numNeuron;
      this->_topology._totalWeights += numNeuron * numInputs;
      // bias synapse weights
      this->_topology._totalWeights += numNeuron;
      //
      // prev_layer_numNeuron = numNeuron;
    }
  }

  return this->_topology; // TODO: hard copy (reallocation)
}


//MARK:NeuralNetworkNeuron
NeuralNetworkNeuron::NeuralNetworkNeuron(std::size_t numInputs, NeuralNetworkActivations activationType)
{
  this->_inputSynapseWeights.reserve(numInputs);
  for (std::size_t ii = 0; ii < numInputs; ++ii) {

    // // this->_inputSynapseWeights.push_back(RandomNumberGenerator::getRangedValue(-0.5f, 0.5f));

    // // ensure non-zero inital weight value

    // // const float rawValue = RandomNumberGenerator::getRangedValue(0.05f, 0.10f);
    // const float rawValue = RandomNumberGenerator::getRangedValue(0.5f, 0.9f);
    // const bool isNegative = RandomNumberGenerator::getRangedValue(-1.0f, 1.0f) < 0.0f;
    // const float newValue = rawValue * (isNegative ? -1.0f : 1.0f);
    // this->_inputSynapseWeights.push_back(newValue);
    // // this->_inputSynapseWeights.push_back(RandomNumberGenerator::getRangedValue(0.5f, 0.9f));
    // this->_inputSynapseWeights.push_back(newValue);

    this->_inputSynapseWeights.push_back(AllActivations::fromEnum(activationType).initWeight());
  }

  // // this->_biasSynapseWeight = RandomNumberGenerator::getRangedValue(-0.5f, 0.5f);

  // // ensure non-zero inital weight value

  // // const float rawValue = RandomNumberGenerator::getRangedValue(0.05f, 0.10f);
  // const float rawValue = RandomNumberGenerator::getRangedValue(0.5f, 0.9f);
  // const bool isNegative = RandomNumberGenerator::getRangedValue(-1.0f, 1.0f) < 0.0f;
  // const float newValue = rawValue * (isNegative ? -1.0f : 1.0f);
  // this->_biasSynapseWeight = newValue;
  // // this->_biasSynapseWeight = RandomNumberGenerator::getRangedValue(0.5f, 0.9f);

  this->_biasSynapseWeight = AllActivations::fromEnum(activationType).initWeight();
}

void NeuralNetworkNeuron::updateInputSynapseWeights(std::size_t ii, float value) const
{
  this->_inputSynapseWeights.at(ii) += value;
  this->_inputSynapseWeights.at(ii) = utils::clamp(this->_inputSynapseWeights.at(ii), -k_weightRange, k_weightRange);
}

void NeuralNetworkNeuron::updateBiasSynapseWeight(float value) const
{
  this->_biasSynapseWeight += value;
  this->_biasSynapseWeight = utils::clamp(this->_biasSynapseWeight, -k_weightRange, k_weightRange);
}


//MARK:NeuralNetwork
NeuralNetwork::NeuralNetwork(const NeuralNetworkTopology& topology)
  : _topology(topology)
{
  RandomNumberGenerator::ensureRandomSeed();

  this->_layers.reserve(topology.getRawValues().size() - 1);
  for (std::size_t layerIndex = 1; layerIndex < topology.getRawValues().size(); ++layerIndex) {
    const uint32_t numInputs = this->_topology.getRawValues().at(layerIndex - 1).numNeurons;
    const uint32_t numNeurons = this->_topology.getRawValues().at(layerIndex).numNeurons;
    const auto activationType = this->_topology.getRawValues().at(layerIndex).activation;

    NeuralNetworkLayer newLayer;
    newLayer.inputNeurons.reserve(numNeurons);
    for (uint32_t neuronIndex = 0; neuronIndex < numNeurons; ++neuronIndex) {
      newLayer.inputNeurons.emplace_back(numInputs, activationType);
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
  if (inputs.size() != this->_topology.getRawValues().at(0).numNeurons) {
    throw std::invalid_argument("invalid input length");
  }

  std::vector<float> currentInputs = inputs; // TODO: hard copy
  for (std::size_t ii = 0; ii < this->_layers.size(); ++ii) {
    auto& currLayer = this->_layers.at(ii);

    const auto activationType = this->_topology.getRawValues().at(ii + 1).activation;
    const auto& currActivations = AllActivations::fromEnum(activationType);

    std::vector<float> currentOutputs; // TODO: inefficient
    for (auto& currNeuron : currLayer.inputNeurons) {

      // since a bias neuron value is always "1" -> directly use the weight
      float sum = currNeuron.getBiasSynapseWeight();

      // sum of "input synapses weight x input neuron values"
      for (std::size_t ii = 0; ii < currentInputs.size(); ++ii) {
        sum += currNeuron.getInputSynapseWeights().at(ii) * currentInputs.at(ii);
      }

      // apply the squashing/activation function

      // const float outputValue = AllActivations::sigmoid::activate(sum);
      // const float outputValue = AllActivations::ReLU::activate(sum);
      // const float outputValue = AllActivations::leakyReLU::activate(sum);
      const float outputValue = currActivations.activate(sum);

      // const float outputValue = _restrictedRELU(sum);
      // const float outputValue = _restrictedleakyReLU(sum);

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





