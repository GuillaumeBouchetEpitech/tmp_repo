
#include "NeuralNetwork.hpp"
#include "ExtenderRecursiveLeastSquareTrainer.hpp"

#include "utils/Clock.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio> // <= EXIT_SUCCESS
#include <cmath> // <= std::round

bool _runTest(const std::vector<TrainingSample>& inDataset, const NeuralNetworkTopology& inTopology)
{

  // NeuralNetworkTopology topology = NeuralNetworkTopology({2, 4, 1});
  auto neuralNetwork = std::make_shared<NeuralNetwork>(inTopology);

  ExtenderRecursiveLeastSquareTrainerDef def;
  def.network = neuralNetwork;
  def.lambda = 0.99f;
  def.windowSize = 100;

  ExtenderRecursiveLeastSquareTrainer trainer = ExtenderRecursiveLeastSquareTrainer(def);

  utils::Clock tmpClock;
  tmpClock.start();

  constexpr int32_t maxEpochs = 1000;
  int32_t epochs = 0;
  for (; epochs < maxEpochs; ++epochs) {
    trainer.trainEpoch(inDataset);

    // for (auto& currSample : inDataset)
    // {
    //   std::vector<float> tmpOutputs;
    //   neuralNetwork->forward(currSample.inputs, tmpOutputs);

    //   std::stringstream sstr;
    //   sstr << "[";
    //   for (auto currVal : currSample.inputs) {
    //     sstr << currVal << ",";
    //   }
    //   sstr << "] => [";
    //   for (auto currVal : currSample.targets) {
    //     sstr << currVal << ",";
    //   }
    //   sstr << "] ----> [";
    //   for (auto currVal : tmpOutputs) {
    //     sstr << std::round(currVal) << ",";
    //   }
    //   sstr << "] (";
    //   for (auto currVal : tmpOutputs) {
    //     sstr << std::fixed << currVal << ",";
    //   }
    //   sstr << ")";

    //   std::cout << sstr.str() << std::endl;
    // }

    if (trainer.isConverged(1e-3)) {
      break;
    }
  }

  tmpClock.stop();

  if (epochs < maxEpochs) {
    std::cout << "*------*" << std::endl;
    std::cout << "* PASS *" << std::endl;
    std::cout << "*------*" << std::endl;
  } else {
    std::cout << "########" << std::endl;
    std::cout << "# FAIL #" << std::endl;
    std::cout << "########" << std::endl;
  }

  std::cout << "epochs -> " << epochs << " / " << maxEpochs << std::endl;
  std::cout << "trainer.averageError() -> " << trainer.averageError() << std::endl;
  std::cout << "tmpClock.getDuration() -> " << tmpClock.getDuration() << "ns" << std::endl;
  std::cout << "tmpClock.getDuration() -> " << float(tmpClock.getDuration()) / 1000000.0f << "ms" << std::endl;

  for (auto& currSample : inDataset)
  {
    std::vector<float> tmpOutputs;
    neuralNetwork->forward(currSample.inputs, tmpOutputs);

    std::stringstream sstr;
    sstr << "[";
    for (auto currVal : currSample.inputs) {
      sstr << currVal << ",";
    }
    sstr << "] => [";
    for (auto currVal : currSample.targets) {
      sstr << currVal << ",";
    }
    sstr << "] ----> [";
    for (auto currVal : tmpOutputs) {
      sstr << std::round(currVal) << ",";
    }
    sstr << "] (";
    for (auto currVal : tmpOutputs) {
      sstr << std::fixed << currVal << ",";
    }
    sstr << ")";

    std::cout << sstr.str() << std::endl;
  }

  return (epochs < maxEpochs);
}

int main()
{
  std::cout << "[START]" << std::endl;

  uint32_t totalTests = 0;
  uint32_t totalPass = 0;

  // NeuralNetworkActivations currActivation = NeuralNetworkActivations::sigmoid;
  // NeuralNetworkActivations currActivation = NeuralNetworkActivations::linear;
  // NeuralNetworkActivations currActivation = NeuralNetworkActivations::ReLU;
  NeuralNetworkActivations currActivation = NeuralNetworkActivations::leakyReLU;

  auto topologyA = NeuralNetworkTopologyBuilder()
    .addInputLayer(1)
    .addHiddenLayer(4, currActivation)
    .addOutputLayer(1, currActivation)
    .build();

  auto topologyB = NeuralNetworkTopologyBuilder()
    .addInputLayer(2)
    .addHiddenLayer(4, currActivation)
    .addOutputLayer(1, currActivation)
    .build();

  {
    totalTests += 1;
    std::cout << std::endl;
    std::cout << " ===========================================" << std::endl;
    std::cout << " == YES ==" << std::endl;
    std::cout << " ==" << std::endl;
    std::vector<TrainingSample> tmpDataset;
    tmpDataset.reserve(2);
    tmpDataset.push_back({ {0}, {0} });
    tmpDataset.push_back({ {1}, {1} });

    bool success = _runTest(tmpDataset, topologyA);
    if (success) {
      totalPass += 1;
    }
  }

  {
    totalTests += 1;
    std::cout << std::endl;
    std::cout << " ===========================================" << std::endl;
    std::cout << " == NO ==" << std::endl;
    std::cout << " ==" << std::endl;
    std::vector<TrainingSample> tmpDataset;
    tmpDataset.reserve(2);
    tmpDataset.push_back({ {0}, {1} });
    tmpDataset.push_back({ {1}, {0} });
    bool success = _runTest(tmpDataset, topologyA);
    if (success) {
      totalPass += 1;
    }
  }

  {
    totalTests += 1;
    std::cout << std::endl;
    std::cout << " ===========================================" << std::endl;
    std::cout << " == AND ==" << std::endl;
    std::cout << " ==" << std::endl;
    std::vector<TrainingSample> tmpDataset;
    tmpDataset.reserve(4);
    tmpDataset.push_back({ {0,0}, {0} });
    tmpDataset.push_back({ {1,0}, {0} });
    tmpDataset.push_back({ {0,1}, {0} });
    tmpDataset.push_back({ {1,1}, {1} });
    bool success = _runTest(tmpDataset, topologyB);
    if (success) {
      totalPass += 1;
    }
  }

  {
    totalTests += 1;
    std::cout << std::endl;
    std::cout << " ===========================================" << std::endl;
    std::cout << " == OR ==" << std::endl;
    std::cout << " ==" << std::endl;
    std::vector<TrainingSample> tmpDataset;
    tmpDataset.reserve(4);
    tmpDataset.push_back({ {0,0}, {0} });
    tmpDataset.push_back({ {1,0}, {1} });
    tmpDataset.push_back({ {0,1}, {1} });
    tmpDataset.push_back({ {1,1}, {1} });
    bool success = _runTest(tmpDataset, topologyB);
    if (success) {
      totalPass += 1;
    }
  }

  {
    totalTests += 1;
    std::cout << std::endl;
    std::cout << " ===========================================" << std::endl;
    std::cout << " == NOR ==" << std::endl;
    std::cout << " ==" << std::endl;
    std::vector<TrainingSample> tmpDataset;
    tmpDataset.reserve(4);
    tmpDataset.push_back({ {0,0}, {1} });
    tmpDataset.push_back({ {1,0}, {0} });
    tmpDataset.push_back({ {0,1}, {0} });
    tmpDataset.push_back({ {1,1}, {0} });
    bool success = _runTest(tmpDataset, topologyB);
    if (success) {
      totalPass += 1;
    }
  }

  {
    totalTests += 1;
    std::cout << std::endl;
    std::cout << " ===========================================" << std::endl;
    std::cout << " == XOR ==" << std::endl;
    std::cout << " ==" << std::endl;
    std::vector<TrainingSample> tmpDataset;
    tmpDataset.reserve(4);
    tmpDataset.push_back({ {0,0}, {0} });
    tmpDataset.push_back({ {1,0}, {1} });
    tmpDataset.push_back({ {0,1}, {1} });
    tmpDataset.push_back({ {1,1}, {0} });
    bool success = _runTest(tmpDataset, topologyB);
    if (success) {
      totalPass += 1;
    }
  }

  {
    totalTests += 1;
    std::cout << std::endl;
    std::cout << " ===========================================" << std::endl;
    std::cout << " == XNOR ==" << std::endl;
    std::cout << " ==" << std::endl;
    std::vector<TrainingSample> tmpDataset;
    tmpDataset.reserve(4);
    tmpDataset.push_back({ {0,0}, {1} });
    tmpDataset.push_back({ {1,0}, {0} });
    tmpDataset.push_back({ {0,1}, {0} });
    tmpDataset.push_back({ {1,1}, {1} });
    bool success = _runTest(tmpDataset, topologyB);
    if (success) {
      totalPass += 1;
    }
  }


  std::cout << std::endl;
  std::cout << "    *-------------------------" << std::endl;
  std::cout << "    | totalPass: " << totalPass << " / " << totalTests << std::endl;
  std::cout << "    *-----------" << std::endl;
  std::cout << std::endl;

  std::cout << "[END]" << std::endl;
  return EXIT_SUCCESS;
}
