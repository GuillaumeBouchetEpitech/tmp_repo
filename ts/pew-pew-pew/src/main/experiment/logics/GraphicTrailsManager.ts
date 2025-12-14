
import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';

export class GraphicTrailsManager {

  private _allTrails: GraphicTrail[] = [];

  createTrail(...args: ConstructorParameters<typeof GraphicTrail>): GraphicTrail {
    const newTrail = new GraphicTrail(...args);
    this._allTrails.push(newTrail);
    return newTrail;
  }

  update() {

    // update all the trails
    for (let ii = 0; ii < this._allTrails.length; ) {

      this._allTrails[ii].update();

      if (!this._allTrails[ii].isAlive()) {
        this._allTrails.splice(ii, 1);
      } else {
        ++ii;
      }
    }

  }

  render(stackRenderers: graphics.renderers.IStackRenderers) {
    for (const currTrail of this._allTrails) {
      currTrail.render(stackRenderers);
    }
  }

}

export interface IGraphicTrailTarget {
  isAlive(): boolean;
  getPosition(): glm.ReadonlyVec3;
}

interface IGraphicTrailElem {
  pos: glm.vec3;
  timeLeftInFrames: number;
}

class GraphicTrail {

  private _target?: IGraphicTrailTarget;

  private _trailMaxLength: number;
  private _trailCoolDownInFrames: number;

  private _trail: IGraphicTrailElem[] = [];
  private _trailCountdownInFrames: number = 0;

  constructor(
    trailMaxLength: number = 10,
    trailCoolDownInFrames: number = 30,
    target: IGraphicTrailTarget
  ) {
    this._trailMaxLength = trailMaxLength;
    this._trailCoolDownInFrames = trailCoolDownInFrames;
    this._target = target;
  }

  isAlive(): boolean {
    return (
      (this._target && this._target.isAlive()) ||
      this._trail.length >= 2
    );
  }

  update() {

    // update internal time
    if (this._trailCountdownInFrames > 0) {
      this._trailCountdownInFrames--;
    }

    if (this._target && !this._target.isAlive()) {
      this._target = undefined;
    }

    if (this._target) {

      // update the trail with the last known location
      if (this._trail.length > 0) {
        glm.vec3.copy(this._trail[0].pos, this._target.getPosition());
      }

      // grow the trail
      if (this._trailCountdownInFrames <= 0) {
        const maxTime = this._trailMaxLength * this._trailCoolDownInFrames;

        this._trail.unshift({
          pos: glm.vec3.copy(glm.vec3.create(), this._target.getPosition()),
          timeLeftInFrames: maxTime,
        });
        this._trailCountdownInFrames = this._trailCoolDownInFrames;

        // ensure max length is respected
        while (this._trail.length > this._trailMaxLength) {
          this._trail.pop();
        }
      }
    }

    // update the trail elements internal time
    for (let ii = 0; ii < this._trail.length; ) {
      --this._trail[ii].timeLeftInFrames;
      if (this._trail[ii].timeLeftInFrames <= 0) {
        this._trail.splice(ii, 1);
      } else {
        ++ii;
      }
    }

  }

  render(stackRenderers: graphics.renderers.IStackRenderers) {

    // const maxTime = this._trailMaxLength * this._trailCoolDownInFrames;
    const maxTime = this._trailCoolDownInFrames;

    for (let ii = 0; ii + 1 < this._trail.length; ++ii) {

      const jj = ii + 1;

      const partA = this._trail[ii];
      const partB = this._trail[jj];

      const coefA = Math.max(0, 0.8 - maxTime / partA.timeLeftInFrames);
      const coefB = Math.max(0, 0.8 - maxTime / partB.timeLeftInFrames);

      const colorA: glm.ReadonlyVec3 = [coefA * 0.7,coefA * 0.7, 0];
      const colorB: glm.ReadonlyVec3 = [coefB * 0.7,coefB * 0.7, 0];

      // stackRenderers.pushLine(posA, posB, [coefficient,coefficient,0]);
      stackRenderers.push3dLine(partA.pos, partB.pos, 0.05*coefA, 0.05*coefB, colorA, colorB);
    }
  }

}


