
import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';

















const getRandomVec3 = (rng: system.math.DeterministicRng): glm.vec3 => {
  const newVec3 = glm.vec3.fromValues(
    rng.normalizedRandom() - rng.normalizedRandom(),
    rng.normalizedRandom() - rng.normalizedRandom(),
    rng.normalizedRandom() - rng.normalizedRandom(),
  );
  const length = glm.vec3.length(newVec3);
  if (length === 0) {
    return getRandomVec3(rng); // try again
  }
  return glm.vec3.scale(newVec3, newVec3, 1 / length);
};















const k_maxTime = 0.35;

class Explosions {

  private _pos = glm.vec3.create();

  private _timeLeft: number = k_maxTime;

  constructor(
    pos: glm.ReadonlyVec3
  ) {
    glm.vec3.copy(this._pos, pos);
  }

  update(
    deltaTimeSec: number
  ) {
    this._timeLeft -= deltaTimeSec;
    if (this._timeLeft < 0) {
      this._timeLeft = 0;
    }
  }

  render(
    stackRenderers: graphics.renderers.IStackRenderers
  ) {

    const rng = new system.math.DeterministicRng();
    rng.setSeed(0); // this makes it predictable

    const allDirs: glm.vec3[] = []
    for (let ii = 0; ii < 6; ++ii) {
      allDirs.push(getRandomVec3(rng));
    }

    const ratio = 1 - this._timeLeft / k_maxTime;

    for (const currDir of allDirs) {

      const newPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, currDir, ratio * 2);

      stackRenderers.pushCross(newPos, 0.5, [1,1,1]);
    }
  }

  isAlive() {
    return this._timeLeft > 0;
  }

};





















class ExplosionsManager {

  private _allExplosions: Explosions[] = [];

  constructor() {
  }

  push(pos: glm.ReadonlyVec3) {
    this._allExplosions.push(new Explosions(pos));
  }

  update(
    deltaTimeSec: number
  ) {

    for (let ii = 0; ii < this._allExplosions.length; ) {
      if (this._allExplosions[ii].isAlive()) {
        ++ii;
      } else {
        this._allExplosions.splice(ii, 1);
      }
    }

    for (const currAgent of this._allExplosions) {
      currAgent.update(deltaTimeSec);
    }
  }

  render(
    stackRenderers: graphics.renderers.IStackRenderers
  ) {
    for (const currAgent of this._allExplosions) {
      currAgent.render(stackRenderers);
    }
  }

};













class ExplosionsManagerSingleton {

  private _explosionsManager: ExplosionsManager | undefined;

  create(): void {
    if (this._explosionsManager) {
      throw new Error(`explosionsManager already created`);
    }
    this._explosionsManager = new ExplosionsManager();
  }

  destroy(): void {
    if (!this._explosionsManager) {
      throw new Error(`explosionsManager already destroyed`);
    }
    this._explosionsManager = undefined;
  }

  get(): ExplosionsManager {
    if (!this._explosionsManager) {
      throw new Error(`explosionsManager not created`);
    }
    return this._explosionsManager;
  }

}

export const GlobalExplosionsManager = new ExplosionsManagerSingleton();





