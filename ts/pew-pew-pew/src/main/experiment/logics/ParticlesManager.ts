
import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';

















// const getRandomVec3 = (rng: system.math.DeterministicRng): glm.vec3 => {
//   const newVec3 = glm.vec3.fromValues(
//     rng.normalizedRandom() - rng.normalizedRandom(),
//     rng.normalizedRandom() - rng.normalizedRandom(),
//     rng.normalizedRandom() - rng.normalizedRandom(),
//   );
//   const length = glm.vec3.length(newVec3);
//   if (length === 0) {
//     return getRandomVec3(rng); // try again
//   }
//   return glm.vec3.scale(newVec3, newVec3, 1 / length);
// };















// const k_maxTime = 0.35;
const k_maxTime = 0.1 * 1.0;

class Particle {

  private _posA = glm.vec3.create();
  private _posB = glm.vec3.create();
  private _dir = glm.vec3.create();

  private _timeLeft: number = k_maxTime;

  constructor(
    pos: glm.ReadonlyVec3,
    dir: glm.ReadonlyVec3,
  ) {
    glm.vec3.copy(this._posA, pos);
    glm.vec3.copy(this._posB, pos);
    glm.vec3.copy(this._dir, dir);
  }

  update(
    deltaTimeSec: number
  ) {
    glm.vec3.copy(this._posB, this._posA);
    glm.vec3.scaleAndAdd(this._posA, this._posA, this._dir, deltaTimeSec);

    this._timeLeft -= deltaTimeSec;
    if (this._timeLeft < 0) {
      this._timeLeft = 0;
    }
  }

  render(
    stackRenderers: graphics.renderers.IStackRenderers
  ) {

    // const rng = new system.math.DeterministicRng();
    // rng.setSeed(0); // this makes it predictable

    // const allDirs: glm.vec3[] = []
    // for (let ii = 0; ii < 6; ++ii) {
    //   allDirs.push(getRandomVec3(rng));
    // }

    // const ratio = 1 - this._timeLeft / k_maxTime;
    // const ratio = this._timeLeft / k_maxTime;

    // for (const currDir of allDirs) {

    //   const newPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, currDir, ratio * 2);

    //   stackRenderers.pushCross(newPos, 0.5, [1,1,1]);
    // }

    // stackRenderers.pushLine(this._posA, this._posB, [ratio,ratio,ratio]);
    stackRenderers.pushLine(this._posA, this._posB, [1,1,1]);
  }

  isAlive() {
    return this._timeLeft > 0;
  }

};





















class ParticlesManager {

  private _allParticles: Particle[] = [];

  constructor() {
  }

  push(pos: glm.ReadonlyVec3, dir: glm.ReadonlyVec3) {
    this._allParticles.push(new Particle(pos, dir));
  }

  update(
    deltaTimeSec: number
  ) {

    for (let ii = 0; ii < this._allParticles.length; ) {
      if (this._allParticles[ii].isAlive()) {
        ++ii;
      } else {
        this._allParticles.splice(ii, 1);
      }
    }

    for (const currParticle of this._allParticles) {
      currParticle.update(deltaTimeSec);
    }
  }

  render(stackRenderers: graphics.renderers.IStackRenderers) {
    for (const currParticle of this._allParticles) {
      currParticle.render(stackRenderers);
    }
  }

};













class ParticlesManagerSingleton {

  private _particlesManager: ParticlesManager | undefined;

  create(): void {
    if (this._particlesManager) {
      throw new Error(`particlesManager already created`);
    }
    this._particlesManager = new ParticlesManager();
  }

  destroy(): void {
    if (!this._particlesManager) {
      throw new Error(`particlesManager already destroyed`);
    }
    this._particlesManager = undefined;
  }

  get(): ParticlesManager {
    if (!this._particlesManager) {
      throw new Error(`particlesManager not created`);
    }
    return this._particlesManager;
  }

}

export const GlobalParticlesManager = new ParticlesManagerSingleton();





