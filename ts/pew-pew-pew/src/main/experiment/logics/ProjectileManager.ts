


import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';


import { GraphicTrailsManager } from './GraphicTrailsManager';

import { FlockFaction, IFlockAgent } from "./FlockManager"
import { GlobalParticlesManager } from 'main/experiment/logics/ParticlesManager';


const _getRandomVec3 = (): glm.vec3 => {
  const newVec3 = glm.vec3.fromValues(
    Math.random() - Math.random(),
    Math.random() - Math.random(),
    Math.random() - Math.random(),
  );
  const length = glm.vec3.length(newVec3);
  if (length === 0) {
    return _getRandomVec3(); // try again
  }
  return glm.vec3.scale(newVec3, newVec3, 1 / length);
};







export interface IProjectileInstanceDef {
  maxDistance: number;
  damage: number;
  speed: number;
  homing: boolean;
};

export class ProjectileInstance {

  private _def: IProjectileInstanceDef

  private _pos = glm.vec3.fromValues(0,0,0);
  private _prevPos = glm.vec3.fromValues(0,0,0);
  private _dir = glm.vec3.fromValues(0,0,0);
  private _distanceLeft: number = 0;

  private _target: IFlockAgent;

  constructor(
    def: IProjectileInstanceDef,
    pos: glm.ReadonlyVec3,
    dir: glm.ReadonlyVec3,
    target: IFlockAgent,
  ) {
    this._def = def;
    glm.vec3.copy(this._prevPos, pos);
    glm.vec3.copy(this._pos, pos);
    glm.vec3.copy(this._dir, dir);

    this._distanceLeft = this._def.maxDistance;

    this._target = target;

    const length = glm.vec3.length(this._dir);
    if (length > 0) {
      glm.vec3.scale(this._dir, this._dir, 1 / length);
    } else {
      // throw new Error("wrong projectile dir length");
      this._distanceLeft = 0; // dead right away
    }
  }

  update(
    deltaTimeSec: number,
    otherFaction: FlockFaction
  ) {

    if (this._def.homing && this._target.isAlive()) {

      // THIS IS UNUSED
      // THIS IS UNUSED
      // THIS IS UNUSED
      // THIS IS UNUSED -> use Missile entity instead, as those can be "intercepted"

      const desiredAim = glm.vec3.sub(glm.vec3.create(), this._target.getPosition(), this._pos);
      const length = glm.vec3.length(desiredAim);
      if (length > 0) {
        glm.vec3.scale(desiredAim, desiredAim, 1 / length); // normalize
      }

      // dot product [-1..1]
      // -> closer to +1 if aligned
      // -> closer to -1 if opposite
      const dotVal = glm.vec3.dot(this._dir, desiredAim);

      // 0 = perfect aim
      // 1 = opposite aim
      const radialDistance = 1 - (dotVal + 1) * 0.5; // [0..1]

      const radialStep = 1.0 * deltaTimeSec;

      if (radialDistance <= radialStep) {
        // close enough -> set aim
        glm.vec3.copy(this._dir, desiredAim);
      } else {
        // too far -> slowly interpolate aim
        glm.vec3.lerp(this._dir, this._dir, desiredAim, radialStep);
      }

      glm.vec3.copy(this._dir, desiredAim);

      // THIS IS UNUSED -> use Missile entity instead, as those can be "intercepted"
      // THIS IS UNUSED
      // THIS IS UNUSED
      // THIS IS UNUSED
    }

    const totalLength = this._def.speed * deltaTimeSec;

    let bestAgent: IFlockAgent | undefined;
    let bestDistance: number = 999999999;

    const bestData: system.math.collisions.IIntersectTriangle_raw_res = {
      distance: -1,
      normal: glm.vec3.fromValues(0,0,0),
    }

    const subResult = otherFaction.searchByRadius(this._pos, totalLength);

    // collision
    for (const tmpAgent of subResult) {

      // const intersectionCoef = tmpAgent.testCollision(this._pos, this._dir);
      const intersectionCoef = tmpAgent.testCollision_exp(this._pos, this._dir, bestData);

      if (
        intersectionCoef > 0 &&
        intersectionCoef <= totalLength &&
        intersectionCoef < bestDistance
      ) {
        bestAgent = tmpAgent;
        bestDistance = intersectionCoef;
      }
    }


    if (bestAgent) {
      bestAgent.takeDamage(this._def.damage);
      this._distanceLeft = 0;


      {
        // use "bestData.distance" to calculate impact point -> "particle pos"
        const impactPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, this._dir, bestData.distance);

        // use "bestData.normal" + "glm::reflect" to calculate "particle dir"

        // TODO: to add to local-framework
        const _glm_reflect = (I: glm.ReadonlyVec3, N: glm.ReadonlyVec3): glm.vec3 => {
          const dotVal = glm.vec3.dot(N, I) * 2;
          const N2 = glm.vec3.scale(glm.vec3.create(), N, dotVal);
          return glm.vec3.sub(glm.vec3.create(), I, N2);
          // return I - N * dot(N, I) * static_cast<T>(2);
        }

        const normDir = glm.vec3.copy(glm.vec3.create(), this._dir);
        {
          // TODO: to add to local-framework
          const length = glm.vec3.length(normDir);
          if (length > 0) {
            glm.vec3.scale(normDir, normDir, 1/length);
          }
        }

        const reflectedDir = _glm_reflect(normDir, bestData.normal);

        for (let ii = 0; ii < 3; ++ii) {
          const particleDir = glm.vec3.scale(glm.vec3.create(), reflectedDir, 50);
          glm.vec3.scaleAndAdd(particleDir, particleDir, _getRandomVec3(), 0.1);
          GlobalParticlesManager.get().push(impactPos, particleDir);
        }
      }


    }



    // move
    glm.vec3.copy(this._prevPos, this._pos);
    glm.vec3.scaleAndAdd(this._pos, this._pos, this._dir, totalLength);

    this._distanceLeft -= totalLength;
  }

  render(
    stackRenderers: graphics.renderers.IStackRenderers,
    frustumCulling: graphics.camera.IFrustumCulling,
    color: glm.ReadonlyVec3,
  ) {
    if (!frustumCulling.sphereInFrustum(this._pos[0], this._pos[1], this._pos[2], 1)) {
      return;
    }

    // stackRenderers.pushLine(this._prevPos, this._pos, color);
    // stackRenderers.pushThickLine(this._prevPos, this._pos, 0.1, color);
    stackRenderers.push3dLine(this._prevPos, this._pos, 0.05, 0.05, color, color);
  }

  getPosition(): glm.ReadonlyVec3 {
    return this._pos;
  }

  isAlive(): boolean {
    return this._distanceLeft > 0;
  }

}

export class ProjectileManager {

  private _graphicTrailsManager: GraphicTrailsManager;

  private _allProjectiles: ProjectileInstance[] = [];

  constructor(GraphicTrailsManager: GraphicTrailsManager) {
    this._graphicTrailsManager = GraphicTrailsManager;
  }

  update(deltaTimeSec: number, otherFaction: FlockFaction): void {

    for (let ii = 0; ii < this._allProjectiles.length; ) {
      if (this._allProjectiles[ii].isAlive()) {
        ++ii;
      } else {
        this._allProjectiles.splice(ii, 1);
      }
    }

    for (const tmpProj of this._allProjectiles) {
      tmpProj.update(deltaTimeSec, otherFaction);
    }
  }

  render(
    stackRenderers: graphics.renderers.IStackRenderers,
    frustumCulling: graphics.camera.IFrustumCulling,
    color: glm.ReadonlyVec3,
  ): void {

    for (const tmpProj of this._allProjectiles) {
      tmpProj.render(stackRenderers, frustumCulling, color);
    }

  }

  shoot(
    def: IProjectileInstanceDef,
    pos: glm.ReadonlyVec3,
    dir: glm.ReadonlyVec3,
    target: IFlockAgent,
  ): ProjectileInstance {
    const newProj = new ProjectileInstance(def, pos, dir, target)
    this._allProjectiles.push(newProj);

    if (def.homing) {
      this._graphicTrailsManager.createTrail(5, 10, newProj);
    }

    return newProj;
  }

}










