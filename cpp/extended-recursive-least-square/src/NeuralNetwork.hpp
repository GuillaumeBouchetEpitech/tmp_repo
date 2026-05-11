
#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>
#include <array>


//MARK:enum
enum class NeuralNetworkActivations : std::size_t {
  sigmoid = 0,
  ReLU,
  leakyReLU,
};



//MARK:AllActivations
namespace AllActivations {

namespace sigmoid {
  float activate(float x);
  float derive(float x);
};

namespace ReLU {
  float activate(float x);
  float derive(float x);
};

namespace leakyReLU {
  float activate(float x);
  float derive(float x);
};

using ActivationFuncPtr = float(*)(float x);

struct ActivationType {
  ActivationFuncPtr activate;
  ActivationFuncPtr derive;
};

const ActivationType& fromEnum(NeuralNetworkActivations type);

};



//MARK:NeuralNetworkLayerDef
struct NeuralNetworkLayerDef
{
  enum class AllTypes : int32_t {
    input,
    hidden,
    output
  };

  AllTypes type;
  uint32_t numNeurons;
  NeuralNetworkActivations activation = NeuralNetworkActivations::sigmoid;
};


// forward declaration
class NeuralNetworkTopologyBuilder;

//MARK:NeuralNetworkTopology
class NeuralNetworkTopology
{
// public:
//   NeuralNetworkTopology(const std::initializer_list<int32_t>& list);

  friend NeuralNetworkTopologyBuilder;
private:
  NeuralNetworkTopology() = default;
public:
  ~NeuralNetworkTopology() = default;

public:
  uint32_t getTotalWeights() const { return this->_totalWeights; }
  // uint32_t getTotalNeurons() const { return this->_totalNeurons; }
  const std::vector<NeuralNetworkLayerDef>& getRawValues() const { return this->_rawValues; }

private:
  uint32_t _totalWeights = 0;
  // uint32_t _totalNeurons = 0;
  std::vector<NeuralNetworkLayerDef> _rawValues;
};



//MARK:NeuralNetworkTopologyBuilder
class NeuralNetworkTopologyBuilder {

public:
  NeuralNetworkTopologyBuilder() = default;

public:
  NeuralNetworkTopologyBuilder& clear();
  NeuralNetworkTopologyBuilder& addInputLayer(uint32_t numNeurons);
  NeuralNetworkTopologyBuilder& addHiddenLayer(uint32_t numNeurons, NeuralNetworkActivations activation = NeuralNetworkActivations::sigmoid);
  NeuralNetworkTopologyBuilder& addOutputLayer(uint32_t numNeurons, NeuralNetworkActivations activation = NeuralNetworkActivations::sigmoid);
  NeuralNetworkTopology build();

private:
  NeuralNetworkTopology _topology;

};



//MARK:NeuralNetworkNeuron
class NeuralNetworkNeuron {

public:
  NeuralNetworkNeuron(std::size_t numInputs);

public:
  // void setInputSynapseWeights(std::size_t ii, float value) const { this->_inputSynapseWeights.at(ii) = value; }
  // void setBiasSynapseWeight(float value) const { this->_biasSynapseWeight = value; }
  void setLastOutput(float value) const { this->_lastOutput = value; }

public:
  void updateInputSynapseWeights(std::size_t ii, float value) const;
  void updateBiasSynapseWeight(float value) const;

public:
  const std::vector<float>& getInputSynapseWeights() const { return this->_inputSynapseWeights; }
  float getBiasSynapseWeight() const { return this->_biasSynapseWeight; }
  float getLastOutput() const { return this->_lastOutput; }

private:
  mutable std::vector<float> _inputSynapseWeights;
  mutable float _biasSynapseWeight = 0.0f;
  mutable float _lastOutput = 0.0f;
};



//MARK:NeuralNetworkLayer
struct NeuralNetworkLayer {
  std::vector<NeuralNetworkNeuron> inputNeurons;
};



//MARK:NeuralNetwork
class NeuralNetwork {

public:
  NeuralNetwork(const NeuralNetworkTopology& topology);

public:
  void forward(const std::vector<float>& inputs, std::vector<float>& outputs);

public:
  const NeuralNetworkTopology& getTopology() const { return _topology; }
  const std::vector<NeuralNetworkLayer>& getLayers() const { return _layers; }

private:
  NeuralNetworkTopology _topology;

  std::vector<NeuralNetworkLayer> _layers;

};
