
#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

//MARK:NeuralNetworkTopology
class NeuralNetworkTopology
{
public:
  NeuralNetworkTopology(const std::initializer_list<int32_t>& list);

public:
  uint32_t getTotalWeights() const { return this->_totalWeights; }
  // uint32_t getTotalNeurons() const { return this->_totalNeurons; }
  const std::vector<uint32_t>& getRawValues() const { return this->_rawValues; }

private:
  uint32_t _totalWeights = 0;
  // uint32_t _totalNeurons = 0;
  std::vector<uint32_t> _rawValues;
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
  void updateInputSynapseWeights(std::size_t ii, float value) const { this->_inputSynapseWeights.at(ii) += value; }
  void updateBiasSynapseWeight(float value) const { this->_biasSynapseWeight += value; }

public:
  const std::vector<float>& getInputSynapseWeights() const { return this->_inputSynapseWeights; }
  float getBiasSynapseWeight() const { return this->_biasSynapseWeight; }
  float getLastOutput() const { return this->_lastOutput; }

private:
  mutable std::vector<float> _inputSynapseWeights; // [inputNeuronIndex]
  mutable float _biasSynapseWeight = 0.0f;
  mutable float _lastOutput = 0.0f;
};

//MARK:NeuralNetworkLayer
struct NeuralNetworkLayer {
  std::vector<NeuralNetworkNeuron> neurons; // [inputNeuronIndex]
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
