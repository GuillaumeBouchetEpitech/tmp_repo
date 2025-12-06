





const _clampValue = (val: number, minVal: number, maxVal: number) =>
  Math.min(Math.max(val, minVal), maxVal);

interface IActivationFunc {
  activation: (x: number) => number;
  derivative: (x: number) => number;
  getWeightInit: (currN: number, isBias: boolean, nInput: number, nOutput: number) => number;
}

const activations: Record<'tanh' | 'sigmoid' | 'relu' | 'leakyRelu', IActivationFunc> = {
  tanh: {
    activation: (x: number) => {
      return Math.tanh(x); // [-1.0..1.0]
    },
    derivative: (x: number) => {
      return 1.0 - Math.tanh(x * x);
      // return 1.0 - x * x; // faster, small loss of accuracy
    },
    getWeightInit: (currN: number, isBias: boolean, nInput: number, nOutput: number) => {
      if (isBias) {
        return 0.01;
      }
      return ((currN+1) / nInput) * 0.01
      // return ((currN+1) / nOutput) * 0.01

      //  U[-√(6/(n_in + n_out)), √(6/(n_in + n_out))]
      // return ((currN+1) / nOutput) * Math.sqrt(3/(nInput+nOutput)) - Math.sqrt(6/(nInput+nOutput))
      // return ((currN+1) / nOutput) * Math.sqrt(2/(nInput+nOutput))
      // return ((currN+1) / nOutput) * Math.sqrt(2/nInput)
      // return ((currN+1) / nOutput) * Math.sqrt(3/nInput) - Math.sqrt(6/nInput)

    }
  },
  sigmoid: {
    activation: (x: number) => {
      return 1 / (1 + Math.exp(-x)); // [0.0..1.0]
    },
    derivative: (x: number) => {
      return x * (1 - x);
    },
    getWeightInit: (currN: number, isBias: boolean, nInput: number, nOutput: number) => {
      if (isBias) {
        return 0.01;
      }
      return ((currN+1) / nOutput) * 0.01

      //  U[-√(6/(n_in + n_out)), √(6/(n_in + n_out))]
      // return ((currN+1) / nOutput) * Math.sqrt(3/(nInput+nOutput)) - Math.sqrt(6/(nInput+nOutput))
      // return ((currN+1) / nOutput) * Math.sqrt(2/(nInput+nOutput))
      // return ((currN+1) / nOutput) * Math.sqrt(2/nInput)
      // return ((currN+1) / nOutput) * Math.sqrt(3/nInput) - Math.sqrt(6/nInput)

    }
  },
  relu: {
    activation: (x: number) => {
      // return Math.max(0, x); // [0..x] -> unsafe?
      return _clampValue(x, 0, 1); // [0..1]
    },
    derivative: (x: number) => {
      return x < 0 ? 0 : 1;
    },
    getWeightInit: (currN: number, isBias: boolean, nInput: number, nOutput: number) => {
      if (isBias) {
        return 0.01;
      }
      return ((currN+1) / nInput) * 0.1
      // return ((currN+1) / nOutput) * 0.01

      //  U[-√(6/(n_in + n_out)), √(6/(n_in + n_out))]
      // return ((currN+1) / nOutput) * Math.sqrt(3/(nInput+nOutput)) - Math.sqrt(6/(nInput+nOutput))
      // return ((currN+1) / nOutput) * Math.sqrt(2/(nInput+nOutput))
      // return ((currN+1) / nOutput) * Math.sqrt(2/nInput)
      // return ((currN+1) / nOutput) * Math.sqrt(3/nInput) - Math.sqrt(6/nInput)
    }
  },
  leakyRelu: {
    activation: (x: number) => {
      // [-0.1x..x]
      if (x < 0) {
        return x * 0.1;
      }
      // return x; // [-0.1x..x]  -> unsafe?
      return _clampValue(x, 0, 1); // [-0.1x..x] -> [-0.1x..1]
    },
    derivative: (x: number) => {
      return x < 0 ? -0.1 : 1;
    },
    getWeightInit: (currN: number, isBias: boolean, nInput: number, nOutput: number) => {
      if (isBias) {
        return 0.1;
      }
      return ((currN+1) / nOutput) * 0.01;

      //  U[-√(6/(n_in + n_out)), √(6/(n_in + n_out))]
      // return ((currN+1) / nOutput) * Math.sqrt(3/(nInput+nOutput)) - Math.sqrt(6/(nInput+nOutput))
      // return ((currN+1) / nOutput) * Math.sqrt(2/(nInput+nOutput))
      // return ((currN+1) / nOutput) * Math.sqrt(2/nInput)
      // return ((currN+1) / nOutput) * Math.sqrt(3/nInput) - Math.sqrt(6/nInput)
    }
  },
}


// const currActivation: IActivationFunc = activations['relu'] // NOPE, FAILURE (-_-)
// const currActivation: IActivationFunc = activations['sigmoid'] // YES BUT SLOW (O_o)
// const currActivation: IActivationFunc = activations['tanh'] // YES AND FAST (^_^)
const currActivation: IActivationFunc = activations['leakyRelu'] // YES AND SUPER FAST (^_^)










interface Synapse {
  input: Neuron;
  output: Neuron;
  weight: number;
}


class Neuron {

  public _inputSynapses: Synapse[] = [];
  public _outputSynapses: Synapse[] = [];
  public _outputValue: number = 0;
  private _gradientOutputValue: number = 0;
  public _isBias: boolean;

  constructor(isBias: boolean) {
    this._isBias = isBias;

    if (this._isBias) {
      this._outputValue = 1;
    }
  }

  feedForward() {

    if (this._isBias) {
      this._outputValue = 1;
      return;
    }

    // no input synapses? (true for the input layer and the bias neurons)
    if (this._inputSynapses.length === 0) {
      return;
    }

    let sum = 0;
    for (const inputSynapse of this._inputSynapses) {
      sum += inputSynapse.input._outputValue * inputSynapse.weight;
    }
    this._outputValue = currActivation.activation(sum);
  }

  calculateGradient_output(targetValue: number) {
    const deltaValues = targetValue - this._outputValue;
    this._gradientOutputValue = deltaValues * currActivation.derivative(this._outputValue);

    // gradient clipping, avoid the exploding gradient issue
    this._gradientOutputValue = _clampValue(this._gradientOutputValue, -1, 1);
  }

  calculateGradient_hidden() {
    let sum = 0;
    this._outputSynapses.forEach((outputSynapse) => {
      if (outputSynapse.output._isBias) {
        return;
      }
      sum += outputSynapse.weight * outputSynapse.output._gradientOutputValue;
    });

    this._gradientOutputValue = sum * currActivation.derivative(this._outputValue);

    // gradient clipping, avoid the exploding gradient issue
    this._gradientOutputValue = _clampValue(this._gradientOutputValue, -1, 1);
  }

  updateInputWeights(learningRate: number) {
    // The weights to be updated are in the Connection container
    // in the neurons in the preceding layer

    this._inputSynapses.forEach((inputSynapse) => {
      // const deltaWeight = _clampValue(inputSynapse.input._outputValue * this._gradientOutputValue * learningRate, -1, 1);
      const deltaWeight = inputSynapse.input._outputValue * this._gradientOutputValue * learningRate;

      // inputSynapse.weight += deltaWeight;
      inputSynapse.weight += _clampValue(deltaWeight, -1.5, 1.5);

      // potential fix for relu-based activations
      inputSynapse.weight = _clampValue(inputSynapse.weight, -1.5, 1.5);

    });
  }

}














const k_minSamples = 100;


interface INeuralNetworkDef {
  topology: number[];
  useBias: boolean;
};

class NeuralNetwork {

  public _def: INeuralNetworkDef;
  public _layers: Neuron[][] = [];
  public _loss: number = 0;
  public _recentAvgError: number = 0;

  constructor(def: INeuralNetworkDef) {
    this._def = def;

    if (def.topology.length < 2) {
      throw new Error("invalid amount of layers");
    }
    def.topology.forEach((totalNeurons, index) => {
      if (totalNeurons === 0) {
        throw new Error(`invalid amount of layer neurons in layer ${index}`);
      }
    });

    this._buildTheLayers(def.topology, def.useBias);
    this._connectTheLayers();
  }

  private _buildTheLayers(topology: number[], useBias: boolean) {

    for (let ii = 0; ii < topology.length; ++ii) {

      const newLayer: Neuron[] = [];

      for (let jj = 0; jj < topology[ii]; ++jj) {
        newLayer.push(new Neuron(false));
      }

      if (useBias) {

        // are we on the last layer?
        const isLastLayer = (ii + 1 === topology.length);
        // if not -> add bias neuron
        if (!isLastLayer) {
          newLayer.push(new Neuron(true));
        }
      }

      this._layers.push(newLayer);
    }
  }

  private _connectTheLayers() {
    for (let ii = 0; ii + 1 < this._layers.length; ++ii) {

      const prevLayer = this._layers[ii];
      const nextLayer = this._layers[ii + 1];

      for (const prevNeuron of prevLayer) {

        for (let kk = 0; kk < nextLayer.length; ++kk) {

          const nextNeuron = nextLayer[kk];

          // bias neurons -> not connected to their previous layer
          if (nextNeuron._isBias) {
            continue;
          }

          const newSynapse: Synapse = {
            input: prevNeuron,
            output: nextNeuron,
            weight: currActivation.getWeightInit(kk, nextNeuron._isBias, prevLayer.length, nextLayer.length),
          };

          prevNeuron._outputSynapses.push(newSynapse);
          nextNeuron._inputSynapses.push(newSynapse);
        }
      }
    }
  }

  feedForward(inputValues: number[]): number[] {

    if (inputValues.length !== this._def.topology[0]) {
      throw new Error("invalid amount of input values");
    }

    // set the input values
    inputValues.forEach((value, index) => {
      this._layers[0][index]._outputValue = value;
    });

    this._layers.forEach((layer) => {
      layer.forEach(neuron => neuron.feedForward());
    });

    const outputLayer = this._layers[this._layers.length - 1];
    return outputLayer.map(neuron => neuron._outputValue);
  }

  calculateLoss(targetValues: number[]): void {

    const outputLayer = this._layers[this._layers.length - 1];

    if (targetValues.length !== outputLayer.length) {
      throw new Error("invalid amount of input values");
    }

    this._loss = 0;

    outputLayer.forEach((outputNeuron, index) => {
      const delta = targetValues[index] - outputNeuron._outputValue;
      this._loss += delta * delta;
    });
    this._loss /= outputLayer.length; // MSE (Mean Square Error)
    this._loss = Math.sqrt(this._loss); // RMSE (Root Mean Square Error)
  }

  backProp(targetValues: number[], learningRate: number) {

    const outputLayer = this._layers[this._layers.length - 1];

    if (targetValues.length !== outputLayer.length) {
      throw new Error("invalid amount of input values");
    }

    //
    // error
    //

    this.calculateLoss(targetValues);

    // this._loss = 0;

    // outputLayer.forEach((outputNeuron, index) => {
    //   const delta = targetValues[index] - outputNeuron._outputValue;
    //   this._loss += delta * delta;
    // });
    // this._loss /= outputLayer.length; // MSE (Mean Square Error)
    // this._loss = Math.sqrt(this._loss); // RMSE (Root Mean Square Error)

    this._recentAvgError = (this._recentAvgError * k_minSamples + this._loss) / (k_minSamples + 1.0);

    //
    // gradients
    //

    outputLayer.forEach((neuron, index) => {
      neuron.calculateGradient_output(targetValues[index]);
    });

    // from last to first hidden layer(s)
    for (let ii = this._layers.length - 2; ii > 0; --ii) {
      this._layers[ii].forEach(neuron => neuron.calculateGradient_hidden());
    }

    //
    // update inputs
    //

    this._layers.forEach((currLayer) => {
      currLayer.forEach(neuron => neuron.updateInputWeights(learningRate));
    });

  }

  print() {

    console.log(`*---`);
    for (let ii = 0; ii < this._layers.length; ++ii) {
      console.log(`| [LAYER ${ii}]`);
      for (let jj = 0; jj < this._layers[ii].length; ++jj) {
        if (this._layers[ii][jj]._isBias) {
          console.log(`| ___(neuron, in=${this._layers[ii][jj]._inputSynapses.length}, out=${this._layers[ii][jj]._outputSynapses.length} IS_BIAS)`);
        } else {
          console.log(`| ___(neuron, in=${this._layers[ii][jj]._inputSynapses.length}, out=${this._layers[ii][jj]._outputSynapses.length})`);
        }

        for (const currSyn of this._layers[ii][jj]._outputSynapses) {
          if (currSyn.input._isBias) {
            console.log(`|    ___{new synapse, weight=${currSyn.weight.toFixed(3)} FROM_BIAS}`);
          } else {
            console.log(`|    ___{new synapse, weight=${currSyn.weight.toFixed(3)}}`);
          }
        }

      }
    }
    console.log(`*---`);
  }


}

















const showVector = (vector: number[]): string => {
  return `[${vector.map(value => value.toFixed(3)).join(', ')}]`;
};

const showDeltaVector = (vectorA: number[], vectorB: number[]): string => {
  return `[${vectorA.map((valueA, index) => (valueA - vectorB[index]).toFixed(3)).join(', ')}]`;
};


interface ITrainingData {
  in: number[];
  out: number[];
};
interface ITrainingResult {
  trainingPass: number,
  percent: number;
  neuralNetwork: NeuralNetwork,
};
interface ITrainingPhase {
  name: string;
  networkDef: INeuralNetworkDef;
  trainingData: ITrainingData[];
  result?: ITrainingResult;
};

const allTrainingPhases: ITrainingPhase[] = [
  {
    name: 'YES',
    networkDef: { topology: [1,1], useBias: false},
    trainingData: [
      { in: [0], out: [0] },
      { in: [1], out: [1] },
    ],
  },
  {
    name: 'NO',
    networkDef: { topology: [1,1], useBias: true},
    trainingData: [
      { in: [0], out: [1] },
      { in: [1], out: [0] },
    ],
  },
  {
    name: 'NAND',
    networkDef: { topology: [2,2,1], useBias: true},
    trainingData: [
      { in: [0,0], out: [1] },
      { in: [1,0], out: [0] },
      { in: [0,1], out: [0] },
      { in: [1,1], out: [0] },
    ],
  },
  {
    name: 'AND',
    networkDef: { topology: [2,2,1], useBias: true},
    trainingData: [
      { in: [0,0], out: [0] },
      { in: [1,0], out: [0] },
      { in: [0,1], out: [0] },
      { in: [1,1], out: [1] },
    ],
  },
  {
    name: 'OR',
    networkDef: { topology: [2,2,1], useBias: false},
    trainingData: [
      { in: [0,0], out: [0] },
      { in: [1,0], out: [1] },
      { in: [0,1], out: [1] },
      { in: [1,1], out: [1] },
    ],
  },
  {
    name: 'XOR',
    networkDef: { topology: [2,2,1], useBias: true},
    trainingData: [
      { in: [0,0], out: [0] },
      { in: [1,0], out: [1] },
      { in: [0,1], out: [1] },
      { in: [1,1], out: [0] },
    ],
  },
  {
    name: 'SIN-WAVE',
    networkDef: { topology: [1,8,8,1], useBias: true},
    trainingData: ((): ITrainingData[] => {
      const tmpData: ITrainingData[] = [];

      const k_quality = 30;
      for (let ii = 0; ii < k_quality; ++ii) {

        const currVal = (ii / k_quality) * Math.PI * 0.5;

        tmpData.push({ in: [ currVal ], out: [ Math.sin(currVal) ] });
      }
      return tmpData;
    })(),
    // trainingData: [
    //   { in: [0,0], out: [0] },
    //   { in: [1,0], out: [1] },
    //   { in: [0,1], out: [1] },
    //   { in: [1,1], out: [0] },
    // ],
  },
];


allTrainingPhases.forEach((currTrainingPhase) => {

  console.log();
  console.log(`Start training phase for "${currTrainingPhase.name}"`);
  console.log();

  const neuralNetwork = new NeuralNetwork(currTrainingPhase.networkDef);

  const maxPasses = 200000;
  let trainingPass = 0;

  while (trainingPass < maxPasses) {

    // const percent = _clampValue(trainingPass / maxPasses, 0, 1);

    // decrease learning rate over time
    // const learningRate = 0.25 - 0.15 * _clampValue(percent * 1, 0, 1);
    // const learningRate = 1.0;
    const learningRate = 0.25;
    // const learningRate = 0.10;

    const currData = currTrainingPhase.trainingData[trainingPass % currTrainingPhase.trainingData.length];

    trainingPass++;

    // const results = neuralNetwork.feedForward(currData.in);
    neuralNetwork.feedForward(currData.in);
    neuralNetwork.backProp(currData.out, learningRate);

    // if ((trainingPass % 1000) === 0) {

    //   //
    //   //
    //   //

    //   let currentFitness = currTrainingPhase.trainingData.length;

    //   for (const currData of currTrainingPhase.trainingData) {
    //     const output = neuralNetwork.feedForward(currData.in);
    //     const diff = output[0] - currData.out[0];
    //     currentFitness -= diff * diff;
    //   }

    //   currentFitness /= currTrainingPhase.trainingData.length;

    //   //
    //   //
    //   //

    //   console.log(
    //     currTrainingPhase.name,
    //     `${trainingPass}/${maxPasses} (${((trainingPass/maxPasses) * 100).toFixed(0)}%)`,
    //     `error: ${neuralNetwork._loss.toFixed(3)}`,
    //     `recentAvgError: ${(Math.ceil(neuralNetwork._recentAvgError * 1000) / 1000).toFixed(3)}`,
    //     `input: ${showVector(currData.in)}`,
    //     `output: ${showVector(results)}`,
    //     `delta: ${showDeltaVector(currData.out, results)}`,
    //     `fitness: ${currentFitness}`,
    //   );
    // }


    if (
      // we need enough sample data for the average, here 100 samples
      trainingPass > k_minSamples &&
      // is the average error acceptable?
      neuralNetwork._recentAvgError < 0.01
    ) {
      console.log("\naverage error acceptable -> break");
      break;
    }
  }

  // console.log("neuralNetwork._layers", neuralNetwork._layers);

  console.log("DONE");

  console.log("PRINT FINAL RESULT")

  currTrainingPhase.result = { trainingPass, percent: trainingPass/maxPasses, neuralNetwork };

  for (const currData of currTrainingPhase.trainingData) {

    const results = currTrainingPhase.result.neuralNetwork.feedForward(currData.in);

    console.log(
      currTrainingPhase.name,
      `input: ${showVector(currData.in)}`,
      `expected: ${showVector(currData.out)}`,
      `result: ${showVector(results)}`,
      `delta: ${showDeltaVector(currData.out, results)}`
    );
  }

});

for (let ii = 0; ii < 10; ++ii) {
  console.log(`###############################################################################`);
}
console.log(`### FINAL RESULTS:`);

allTrainingPhases.forEach((currTrainingPhase) => {

  //
  //
  //

  let currentFitness = currTrainingPhase.trainingData.length;

  for (const currData of currTrainingPhase.trainingData) {
    const output = currTrainingPhase.result!.neuralNetwork.feedForward(currData.in);
    const diff = output[0] - currData.out[0];
    currentFitness -= diff * diff;
  }

  currentFitness /= currTrainingPhase.trainingData.length;

  //
  //
  //

  console.log();
  console.log(`---> "${currTrainingPhase.name}"`);
  console.log(
    `  -> `,
    `trainingPass: ${currTrainingPhase.result!.trainingPass}`,
    `(${(currTrainingPhase.result!.percent * 100).toFixed(2)}%)`,
  );
  console.log(
    `  -> `,
    `recentAvgError: ${currTrainingPhase.result!.neuralNetwork._recentAvgError.toFixed(4)}`,
  );
  console.log(
    `  -> `,
    `fitness: ${currentFitness.toFixed(4)}`,
  );

  for (const currData of currTrainingPhase.trainingData) {

    const results = currTrainingPhase.result!.neuralNetwork.feedForward(currData.in);

    currTrainingPhase.result!.neuralNetwork.calculateLoss(currData.out);

    console.log(
      currTrainingPhase.name,
      `[ input: ${showVector(currData.in)}`,
      `expected: ${showVector(currData.out)}`,
      `result: ${showVector(results)} ]`,
      `(delta: ${showDeltaVector(currData.out, results)}`,
      `RMSE: ${currTrainingPhase.result!.neuralNetwork._loss.toFixed(3)})`
    );

  }

  // currTrainingPhase.result!.neuralNetwork.print();

  console.log();

});
