

export class MathUtils {

	/**
	 *
	 * @param x [-inf..+inf]
	 * @return [0..1]
	 */
	static steeperSigmoid(x: number): number {
		return 1.0 / (1.0 + Math.exp(-4.9 * x));
	}

	/**
	 *
	 * @param x [-inf..+inf]
	 * @return [0..x]
	 */
	static relu(x: number): number {
		return Math.max(0, x);
	}
};

export class ArtificialNeuralNetwork {

	private _topology: ReadonlyArray<number>;
	private _totalWeights: number;
	private _hiddenLayers: number[][][];
	private _outputLayer: number[][];

	constructor(topology: ReadonlyArray<number>) {

		const lastLayerIndex = topology.length - 1;

		//
		// extra check here

		if (topology.length < 2)
			throw new Error("received invalid number of layer");

		if (topology[0] <= 0) // input
			throw new Error("received invalid number of inputs");

		for (let ii = 1; ii < lastLayerIndex; ++ii) // exclude first & last -> input & output
			if (topology[ii] <= 0)
				throw new Error("received invalid number of hidden neurons");

		if (topology[lastLayerIndex] <= 0) // input
			throw new Error("received invalid number of outputs");

		// save the topology for later checks

		this._topology = topology;

		// compute totalWeights

		this._totalWeights = 0;

		let prevLayerNeuronNumber = topology[0]; // inputs

		// exclude first & last -> input & output
		for (let ii = 1; ii < lastLayerIndex; ++ii) {

			const currLayerNeuronNumber = topology[ii];

			this._totalWeights += prevLayerNeuronNumber * currLayerNeuronNumber;
			this._totalWeights += currLayerNeuronNumber; // bias weights
			prevLayerNeuronNumber = currLayerNeuronNumber;
		}
		this._totalWeights += prevLayerNeuronNumber * topology[lastLayerIndex];
		this._totalWeights += topology[lastLayerIndex]; // bias weight

		//
		// build hidden layer(s)

		prevLayerNeuronNumber = topology[0]; // inputs

		this._hiddenLayers = [];

		// exclude first & last -> input & output
		for (let ii = 1; ii < lastLayerIndex; ++ii) {

			// number of neuron on the hidden layer
			const currLayerNeuronNumber = topology[ii];

			const layer: number[][] = [];

			this._buildLayer(layer, prevLayerNeuronNumber, currLayerNeuronNumber);

			this._hiddenLayers.push(layer);

			// save the size -> this layer feed the next one
			prevLayerNeuronNumber = currLayerNeuronNumber;
		}

		//
		// build output layer

		this._outputLayer = [];

		this._buildLayer(this._outputLayer, prevLayerNeuronNumber, topology[lastLayerIndex]);
	}

	private _buildLayer(layer: number[][], inputsNumber: number, totalNeuronsInLayer: number): void {

		const weightInitVal = (1.0 / inputsNumber) * 1.0;
		const _getNeuronWeight = () => {
			const randomVal = (Math.random() * 2 - 1); // [-1..1]
			return weightInitVal * 0.9 + randomVal * 0.1;
		}

		for (let ii = 0; ii < totalNeuronsInLayer; ++ii) {

			const connWeights: number[] = [];

			for (let jj = 0; jj < inputsNumber; ++jj) {
				// const randomValue = (Math.random() - Math.random()) * 1.0; // [-1..1]
				// connWeights.push(randomValue);
				connWeights.push(_getNeuronWeight());
			}

			{ // bias value weight
				// const randomValue = (Math.random() - Math.random()) * 1.0; // [-1..1]
				// connWeights.push(randomValue);
				connWeights.push(_getNeuronWeight());
			} // bias value weight

			layer.push(connWeights);
		}
	}

	process(inputs: ReadonlyArray<number>): number[] {

		if (inputs.length != this._topology[0])
			throw new Error(`invalid inputs, input=${inputs.length}, expected=${this._topology[0]}`);

		//
		// process hidden layer

		let	currentInputs: ReadonlyArray<number> = inputs;

		for (const hiddenLayer of this._hiddenLayers) {

			const currentOutput: number[] = [];

			this._processLayer(hiddenLayer, currentInputs, currentOutput);

			currentInputs = currentOutput;
		}

		//
		// process output layer

		const finalOutput: number[] = [];

		this._processLayer(this._outputLayer, currentInputs, finalOutput);

		return finalOutput;
	}

	private _processLayer(
    layer: ReadonlyArray<ReadonlyArray<number>>,
    inputs: ReadonlyArray<number>,
    output: number[]
  ) {

		// Cycle over all the connections and sum their weights against the inputs.
		for (const connWeights of layer) {

			let activation = 0.0;

			// Sum the weights to the activation value.
			for (let ii = 0; ii < inputs.length; ++ii)
				activation += inputs[ii] * connWeights[ii];

			// bias value
			activation += 1 * connWeights[connWeights.length - 1];

			output.push(MathUtils.steeperSigmoid(activation));
			// output.push(MathUtils.relu(activation));
		}
	}

	setWeights(newWeights: ReadonlyArray<number>) {

		if (newWeights.length != this._totalWeights)
			throw new Error(`invalid weights, input=${newWeights.length}, expected=${this._totalWeights}`);

		let weightsIndex = 0;

		for (const layer of this._hiddenLayers)
			for (const connections of layer)
				for (let ii = 0; ii < connections.length; ++ii)
					connections[ii] = newWeights[weightsIndex++];

		for (const connections of this._outputLayer)
			for (let ii = 0; ii < connections.length; ++ii)
				connections[ii] = newWeights[weightsIndex++];
	}

	getWeights(): ReadonlyArray<number> {

		const outWeights: number[] = [];

		for (const layer of this._hiddenLayers)
			for (const connections of layer)
				for (const weight of connections)
					outWeights.push(weight);

		for (const connections of this._outputLayer)
			for (const weight of connections)
				outWeights.push(weight);

		return outWeights;
	}

	set weights(newWeights: ReadonlyArray<number>) {
		this.setWeights(newWeights);
	}

	get weights(): ReadonlyArray<number> {
		return this.getWeights();
	}

	get totalWeights(): number {
		return this._totalWeights;
	}
}


export class Genome {
	public id: number;
	public fitness: number;
	public weights: number[];
	public userData: any;

	constructor(id: number, fitness: number = 0, weights: ReadonlyArray<number> = []) {
		this.id = id;
		this.fitness = fitness;
		this.weights = [...weights]; // copy
	}
};

export class GeneticAlgorithm {

	private _annTopology: ReadonlyArray<number>;

	private _currentId: number = 0;
	private _ANNs: ArtificialNeuralNetwork[] = [];
	private _genomes: Genome[] = [];
	// private _currentGeneration: number = 1;
	// private _bestGenome: Genome | null = null;
	private _bestGenomes: Genome[] = [];

	constructor(genomeSize: number, annTopology: ReadonlyArray<number>) {

		this._annTopology = annTopology;

		for (let ii = 0; ii < genomeSize; ++ii)
			this._ANNs.push(new ArtificialNeuralNetwork(annTopology));

		for (let ii = 0; ii < genomeSize; ++ii)
			this._genomes.push(new Genome(this._currentId++, 0, this._ANNs[ii].weights));

		for (let ii = 0; ii < 5; ++ii)
			this._bestGenomes.push(new Genome(-1));
	}

	breedPopulation(): boolean {

		const latestBestGenomes = this._genomes
			.slice(0)
			.sort((genA, genB) => {
				return genB.fitness - genA.fitness;
			});

		const isSmarter = (this._bestGenomes[0].fitness < latestBestGenomes[0].fitness);

		// if (isSmarter)
		// 	this._bestGenome = bestGenomes[0];

		for (let ii = 0; ii < this._bestGenomes.length; ++ii)
			if (this._bestGenomes[ii].fitness == 0 ||
				this._bestGenomes[ii].fitness < latestBestGenomes[ii].fitness) {

				this._bestGenomes[ii] = latestBestGenomes[ii];
			}

		const children: Genome[] = [];

		{ // elitism

			// const eliteGenome = new Genome(this._bestGenome.id);

			// // deep copy
			// for (let ii = 0; ii < this._bestGenome.weights.length; ++ii)
			// 	eliteGenome.weights.push(this._bestGenome.weights[ii]);

			// children.push(eliteGenome);

			for (let ii = 0; ii < this._bestGenomes.length; ++ii) {

				const eliteGenome = new Genome(this._bestGenomes[ii].id);

				// deep copy
				eliteGenome.weights.length = 0;
				for (let jj = 0; jj < this._bestGenomes[ii].weights.length; ++jj)
					eliteGenome.weights.push(this._bestGenomes[ii].weights[jj]);
				// eliteGenome.weights = this._bestGenomes[ii].weights.slice();

				children.push(eliteGenome);
			}

		} // elitism

		{ // breed best genomes

			const parentsPairs: number[][] = [];
			for (let ii = 0; ii < latestBestGenomes.length; ++ii)
				for (let jj = ii + 1; jj < latestBestGenomes.length; ++jj)
					parentsPairs.push([ ii, jj ]);

			// sort pair by summed fitness
			parentsPairs.sort((pairA, pairB) => {

				const fitnessA = latestBestGenomes[pairA[0]].fitness + latestBestGenomes[pairA[1]].fitness;
				const fitnessB = latestBestGenomes[pairB[0]].fitness + latestBestGenomes[pairB[1]].fitness;

				// the higher the better
				return fitnessB - fitnessA;
			});

			const maxOffsprings = this._genomes.length * 0.95 - children.length;

			for (let ii = 0; ii < maxOffsprings; ++ii) {

				const parentA = latestBestGenomes[parentsPairs[ii][0]];
				const parentB = latestBestGenomes[parentsPairs[ii][1]];
				const offspring = this._reproduce(parentA, parentB);
				this._mutate(offspring);
				children.push(offspring);
			}

		} // breed best genomes

		{ // random genomes

			const remainingChildren = (this._genomes.length - children.length);

			const totalWeights = this._ANNs[0].totalWeights;

			for (let ii = 0; ii < remainingChildren; ++ii) {

				const genome = new Genome(this._currentId++);

				// healthy start -> random base weight from a temporary neural network
				const newBaseWeights = (new ArtificialNeuralNetwork(this._annTopology)).getWeights();

				// ensure the genome has an empty weight array
				genome.weights.length = 0;

				// fill the genome weight array using the base weight with some mutation
				for (let jj = 0; jj < totalWeights; ++jj) {
					const randomVal = (Math.random() * 2) - 1; // [-1..1]
					const newWeightVal = newBaseWeights[jj] + randomVal * 0.3;
					genome.weights.push(newWeightVal);
				}

				// for (let jj = 0; jj < totalWeights; ++jj)

				// 	// const weightInitVal = (1.0 / inputsNumber) * 1.0;
				// 	// const _getNeuronWeight = () => {
				// 	// 	const randomVal = (Math.random() * 2 - 1); // [-1..1]
				// 	// 	return weightInitVal * 0.9 + randomVal * 0.1;
				// 	// }


				// 	genome.weights.push(Math.random() - Math.random());  // [-1..1]

				children.push(genome);
			}

		} // random genomes

		this._genomes = children;
		// ++this._currentGeneration;

		for (let ii = 0; ii < this._genomes.length; ++ii)
			this._ANNs[ii].weights = this._genomes[ii].weights;

		return isSmarter;
	}

	private _reproduce(parentA: Readonly<Genome>, parentB: Readonly<Genome>): Genome {

		const totalWeights = this._ANNs[0].totalWeights;

		const offspring = new Genome(this._currentId++);

		let chanceForParentA = 0.5; // 50%
		if (parentA.fitness > parentB.fitness)
			chanceForParentA = 0.7; // 70%
		else if (parentA.fitness < parentB.fitness)
			chanceForParentA = 0.3; // 30%

		for (let ii = 0; ii < totalWeights; ++ii) {

			const randomValue = Math.random(); // [0..1]

			if (randomValue < chanceForParentA)
				offspring.weights[ii] = parentA.weights[ii];
			else
				offspring.weights[ii] = parentB.weights[ii];
		}

		return offspring;
	}

	private _mutate(genome: Genome) {

		// const mutationChance = 0.1; // 10%
		const mutationChance = 0.2; // 20%
		// const mutationEffect = 0.1; // 10%
		const mutationEffect = 0.2; // 20%

		for (let ii = 0; ii < genome.weights.length; ++ii) {
			if (Math.random() < mutationChance) {

				const randomValue = (Math.random() * 2 - 1); // [-1..1]
				genome.weights[ii] += (randomValue * mutationEffect);
			}
		}
	}

	get genomes(): ReadonlyArray<Genome> {
		return this._genomes;
	}

	get bestGenomes(): ReadonlyArray<Genome> {
		return this._bestGenomes;
	}

	get ANNs(): ReadonlyArray<ArtificialNeuralNetwork> {
		return this._ANNs;
	}
};














interface ITrainingData {
	input: number[];
	output: number;
}

interface ITrainingPhase {
	name: string;
	topology: number[];
	trainingData: ITrainingData[];
}

const trainingPhases: ITrainingPhase[] = [

	{
		name: "YES",
		topology: [1, 2, 1],
		trainingData: [
			{ input: [0], output: 0 }, // YES
			{ input: [1], output: 1 }, // YES
		]
	},
	{
		name: "NOT",
		topology: [1, 2, 1],
		trainingData: [
			{ input: [0], output: 1 }, // NO
			{ input: [1], output: 0 }, // NO
		]
	},
	{
		name: "AND",
		topology: [2, 3, 1],
		trainingData: [
			{ input: [0, 0], output: 0 }, // AND
			{ input: [1, 0], output: 0 }, // AND
			{ input: [0, 1], output: 0 }, // AND
			{ input: [1, 1], output: 1 }, // AND
		]
	},
	{
		name: "NAND",
		topology: [2, 3, 1],
		trainingData: [
			{ input: [0, 0], output: 1 }, // NAND
			{ input: [1, 0], output: 1 }, // NAND
			{ input: [0, 1], output: 1 }, // NAND
			{ input: [1, 1], output: 0 }, // NAND
		]
	},
	{
		name: "NOR",
		topology: [2, 3, 1],
		trainingData: [
			{ input: [0, 0], output: 1 }, // NOR
			{ input: [1, 0], output: 0 }, // NOR
			{ input: [0, 1], output: 0 }, // NOR
			{ input: [1, 1], output: 0 }, // NOR
		]
	},
	{
		name: "OR",
		topology: [2, 3, 1],
		trainingData: [
			{ input: [0, 0], output: 0 }, // OR
			{ input: [1, 0], output: 1 }, // OR
			{ input: [0, 1], output: 1 }, // OR
			{ input: [1, 1], output: 1 }, // OR
		]
	},
	{
		name: "XOR",
		topology: [2, 3, 1],
		trainingData: [
			{ input: [0, 0], output: 0 }, // XOR
			{ input: [1, 0], output: 1 }, // XOR
			{ input: [0, 1], output: 1 }, // XOR
			{ input: [1, 1], output: 0 }, // XOR
		]
	},
	{
		name: "XNOR",
		topology: [2, 3, 1],
		trainingData: [
			{ input: [0, 0], output: 1 }, // XNOR
			{ input: [1, 0], output: 0 }, // XNOR
			{ input: [0, 1], output: 0 }, // XNOR
			{ input: [1, 1], output: 1 }, // XNOR
		]
	},
];

trainingPhases.forEach((currPhase) => {

	console.log(`####################################################################`);
	console.log(`####################################################################`);
	console.log(`####################################################################`);

	console.log(`currPhase.name ${currPhase.name}`);

	const k_genomeSize = 1000; // TODO: hardcoded
	// const k_annTopology = [2, 3, 1]; // TODO: hardcoded
	const k_maxGenerations = 300; // TODO: hardcoded

	const geneticAlgorithm = new GeneticAlgorithm(k_genomeSize, currPhase.topology);

	let generationNo = 1;
	let totalProgressiveGenerations = 0;
	for (; generationNo < k_maxGenerations; ++generationNo) {

		for (let ii = 0; ii < k_genomeSize; ++ii) {

			let currentFitness = currPhase.trainingData.length;

			const currAnn = geneticAlgorithm.ANNs[ii];
			const currGenome = geneticAlgorithm.genomes[ii];

			for (const currData of currPhase.trainingData) {
				const output = currAnn.process(currData.input);
				const delta = output[0] - currData.output;
				currentFitness -= delta * delta;
			}

			currGenome.fitness = currentFitness / currPhase.trainingData.length;
		}

		const progressWasMade = geneticAlgorithm.breedPopulation();
		if (progressWasMade) {
			totalProgressiveGenerations += 1;
		}
		// console.log(`progressWasMade (${generationNo} / ${k_maxGenerations})`, progressWasMade);

		if (geneticAlgorithm.bestGenomes.length) {

			if (generationNo % 10 == 0) {
				console.log(`generationNo [${generationNo}] (progress: ${totalProgressiveGenerations}) fitness`, geneticAlgorithm.bestGenomes[0].fitness.toFixed(3));
			}

			if (geneticAlgorithm.bestGenomes[0].fitness > 0.98) {
				console.log(` -> generationNo [${generationNo}] (progress: ${totalProgressiveGenerations}) fitness`, geneticAlgorithm.bestGenomes[0].fitness.toFixed(3));
				console.log('fitness is good enough -> stopping now');
				break;
			}
		}
	}

	// geneticAlgorithm.bestGenome?.weights

	if (geneticAlgorithm.bestGenomes.length > 0) {

		console.log(`total generations: ${generationNo} / ${k_maxGenerations}`);
		console.log('bestGenome.fitness', geneticAlgorithm.bestGenomes[0].fitness.toFixed(3));

		const currAnn = new ArtificialNeuralNetwork(currPhase.topology);
		currAnn.setWeights(geneticAlgorithm.bestGenomes[0].weights);


		for (const currData of currPhase.trainingData) {
			const output = currAnn.process(currData.input);

			console.log(
				` -> input(${JSON.stringify(currData.input)})`
				+ `-> (${output[0].toFixed(2)} / ${currData.output.toFixed(2)}) (output / expected)`
			)
		}

	}

});
