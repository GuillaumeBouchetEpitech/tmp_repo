


import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';


import * as flocking from './flocking-logic';

import { ProjectileManager } from './ProjectileManager';

import { GraphicTrailsManager } from './GraphicTrailsManager';

import { ITurretDef, Turret } from './Turret';

import { GlobalExplosionsManager } from './ExplosionsManager';

import { GraphicModel, GraphicModelDef } from './GraphicModel';

import { BvhTree } from './bvh/BvhTree';
import { BvhTreeNode } from './bvh/BvhTreeNode';
import { BvhDebug } from './bvh/BvhDebug';


interface ITrianglesStackRenderer {
  pushTriangle(
    inPointA: glm.ReadonlyVec3,
    inPointB: glm.ReadonlyVec3,
    inPointC: glm.ReadonlyVec3,
    inColor: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    inNormal: glm.ReadonlyVec3,
  ): void;
}




export interface IFlockAgentDef {
  speed: number;
  type: 'jet-fighter' | 'capital' | 'homing-missile';
  range: number;
  battlefieldSize: number;
  separateRadius: number;
  // maxHealth: number;

  shield?: GraphicModelDef;
  hull: GraphicModelDef;

  turrets: ITurretDef[];

  deathTrigger?: {
    maxDistance?: number;
    proximityRadius?: number;
  }
};



export interface IFlockAgent {

  update(
    deltaTimeSec: number,
    ownAgents: FlockFaction,
    otherFaction: FlockFaction,
    projectileManager: ProjectileManager,
  ): void;

  renderHull(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    color: glm.ReadonlyVec3,
    colorHurt: glm.ReadonlyVec3,
    debugMode?: boolean,
  ): void;

  renderTurrets(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    debugMode?: boolean,
  ): void;

  renderTurretsLogicDebug(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
  ): void;

  renderShieldIdle(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    color: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    debugMode?: boolean,
  ): void;
  renderShieldImpact(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    color: glm.ReadonlyVec3 | glm.ReadonlyVec4,
  ): void;

  isAlive(): boolean;

  // testCollision(
  //   rayOrigin: glm.ReadonlyVec3,
  //   rayVelocity: glm.ReadonlyVec3,
  // ): number;

  testCollision_exp(
    rayOrigin: glm.ReadonlyVec3,
    rayVelocity: glm.ReadonlyVec3,
    outData: system.math.collisions.IIntersectTriangle_raw_res,
  ): number;

  takeDamage(damage: number): void;

  setPosition(pos: glm.ReadonlyVec3): void;
  getPosition(): glm.ReadonlyVec3;
  getQuat(): glm.ReadonlyQuat;
  setForwardAxis(forwardAxis: glm.ReadonlyVec3): void;
  getForwardAxis(): glm.ReadonlyVec3;
  // getSpeed(): number;

  isTrackable(): boolean;

  min: glm.ReadonlyVec3;
  max: glm.ReadonlyVec3;
};

//MARK: AbstractFlockAgent

abstract class AbstractFlockAgent {

  protected _def: IFlockAgentDef

  protected _prevPos = glm.vec3.fromValues(0,0,0);
  protected _pos = glm.vec3.fromValues(0,0,0);
  protected _quat = glm.quat.identity(glm.quat.create());

  protected _forwardAxis = glm.vec3.fromValues(1,0,0);

  protected _hullGraphicModel: GraphicModel;
  protected _shieldGraphicModel?: GraphicModel;

  protected _turrets: Turret[] = [];

  protected _totalDistance: number = 0;

  protected _min = glm.vec3.fromValues(0, 0, 0);
  protected _max = glm.vec3.fromValues(0, 0, 0);

  constructor(def: IFlockAgentDef) {

    this._def = def;

    this._hullGraphicModel = new GraphicModel(def.hull);
    if (def.shield) {
      this._shieldGraphicModel = new GraphicModel(def.shield);
    }

    for (const turretDef of this._def.turrets) {
      this._turrets.push(new Turret(turretDef));
    }
  }

  protected _applyCorrection(
    deltaTimeSec: number,
    ownAgents: FlockFaction,
  ): void {

    const k_speed = this._def.speed;
    // const k_correctionSpeed = k_speed * 0.2; // too low!
    const k_correctionSpeed = k_speed * 0.4;
    // const k_correctionSpeed = k_speed * 1.2; // too high!

    const correctedAcceleration = glm.vec3.fromValues(0, 0, 0);

    const separationAccelerationClose = glm.vec3.fromValues(0,0,0);
    const stayInBattleAccelerationClose = glm.vec3.fromValues(0,0,0);

    // flocking._separation(this, ownAgents.allAgents, 0.0, 4.0, 8.0, separationAccelerationClose);
    flocking._separation(this, ownAgents, 0.0, this._def.separateRadius, 2.0, separationAccelerationClose);

    // flocking._stayInBattle(this, [0,0,10], [this._def.battlefieldSize,this._def.battlefieldSize,20], 4.0, stayInBattleAccelerationClose);
    flocking._stayInBattle(this, [0,0,5], [this._def.battlefieldSize,this._def.battlefieldSize,20], 4.0, stayInBattleAccelerationClose);
    // flocking._stayInBattle(this, [0,0,5.0*0.25], [this._def.battlefieldSize,this._def.battlefieldSize,10], 100.0, stayInBattleAccelerationClose);
    // flocking._stayInBattle(this, [0,0,10], [this._def.battlefieldSize,20,20], 10000.0, stayInBattleAccelerationClose);

    glm.vec3.add(correctedAcceleration, correctedAcceleration, separationAccelerationClose);
    glm.vec3.add(correctedAcceleration, correctedAcceleration, stayInBattleAccelerationClose);

    //

    flocking._limitVec3(correctedAcceleration, 1.0);
    glm.vec3.scaleAndAdd(this._pos, this._pos, correctedAcceleration, k_correctionSpeed * deltaTimeSec);
  }

  // testCollision(
  //   rayOrigin: glm.ReadonlyVec3,
  //   rayVelocity: glm.ReadonlyVec3,
  // ): number {

  //   if (this._shieldGraphicModel && this._shieldGraphicModel.isAlive()) {

  //     const intersectionCoef = this._shieldGraphicModel.testCollision(
  //       rayOrigin,
  //       rayVelocity,
  //     );

  //     if (intersectionCoef > 0) {
  //       return intersectionCoef;
  //     }
  //   }

  //   return this._hullGraphicModel.testCollision(
  //     rayOrigin,
  //     rayVelocity,
  //   );
  // }

  testCollision_exp(
    rayOrigin: glm.ReadonlyVec3,
    rayVelocity: glm.ReadonlyVec3,
    outData: system.math.collisions.IIntersectTriangle_raw_res,
  ): number {

    if (this._shieldGraphicModel && this._shieldGraphicModel.isAlive()) {

      const intersectionCoef = this._shieldGraphicModel.testCollision_exp(
        rayOrigin,
        rayVelocity,
        outData,
      );

      if (intersectionCoef > 0) {
        return intersectionCoef;
      }
    }

    return this._hullGraphicModel.testCollision_exp(
      rayOrigin,
      rayVelocity,
      outData,
    );
  }

  takeDamage(damage: number): void {
    if (
      this._shieldGraphicModel &&
      this._shieldGraphicModel.isAlive()
    ) {
      this._shieldGraphicModel.takeDamage(damage);
    } else {
      this._hullGraphicModel.takeDamage(damage);
    }
    // if (this._health > 0) {
    //   this._health -= damage;
    //   // this._hurtTime = 0.1;
    // }
  }

  isAlive(): boolean {
    // return this._health > 0;
    return this._hullGraphicModel.isAlive();
  }

  setPosition(pos: glm.ReadonlyVec3): void {
    glm.vec3.copy(this._pos, pos);
  }
  getPosition(): glm.ReadonlyVec3 {
    return this._pos;
  }
  getQuat(): glm.ReadonlyQuat {
    return this._quat;
  }
  setForwardAxis(forwardAxis: glm.ReadonlyVec3): void {
    glm.vec3.copy(this._forwardAxis, forwardAxis);
  }
  getForwardAxis(): glm.ReadonlyVec3 {
    return this._forwardAxis;
  }

  protected _internalUpdate(
    deltaTimeSec: number,
    ownAgents: FlockFaction,
    otherFaction: FlockFaction,
    projectileManager: ProjectileManager,
  ): void {
  }

  update(
    deltaTimeSec: number,
    ownAgents: FlockFaction,
    otherFaction: FlockFaction,
    projectileManager: ProjectileManager,
  ): void {


    const k_speed = this._def.speed;
    const k_correctionSpeed = k_speed * 0.2;

    glm.vec3.copy(this._prevPos, this._pos);

    //
    //
    //

    this._applyCorrection(deltaTimeSec, ownAgents);

    //
    //
    //

    {

      this._internalUpdate(
        deltaTimeSec,
        ownAgents,
        otherFaction,
        projectileManager,
      );

    }

    //
    //
    //

    for (const currTurret of this._turrets) {
      currTurret.update(
        deltaTimeSec,
        this._pos,
        this._quat,
        projectileManager,
        ownAgents,
        otherFaction,
      );
    }

    //
    //
    //

    {

      // // clamp position to battlefield space
      // this._pos[0] = system.math.clamp(this._pos[0], -this._def.battlefieldSize, +this._def.battlefieldSize);
      // this._pos[1] = system.math.clamp(this._pos[1], -this._def.battlefieldSize, +this._def.battlefieldSize);
      // this._pos[2] = system.math.clamp(this._pos[2], 1, +20);

    }

    //
    //
    //

    if (this._def.deathTrigger?.maxDistance !== undefined) {
      this._totalDistance += glm.vec3.distance(this._prevPos, this._pos);

      if (this._totalDistance > this._def.deathTrigger?.maxDistance) {

        this._hullGraphicModel.takeDamage(999999); // kamikaze

        this.onDeath(otherFaction);
      }
    }

    //
    //
    //

    this._hullGraphicModel.update(deltaTimeSec, this._pos, this._quat);
    this._shieldGraphicModel?.update(deltaTimeSec, this._pos, this._quat);

    //
    //
    //

    if (this._shieldGraphicModel) {
      this._shieldGraphicModel.computeAABB(this._min, this._max);
    } else {
      this._hullGraphicModel.computeAABB(this._min, this._max);
    }

    // console.log(this._min, this._max);

  }

  renderShieldIdle(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    color: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    debugMode?: boolean,
  ): void {
    if (debugMode) {
      this._shieldGraphicModel?.renderWireframe(stackRenderers, color, true);
    } else {
      // this._shieldGraphicModel?.renderWireframe(stackRenderers, color, true);
      this._shieldGraphicModel?.renderPoly(trianglesStackRenderer, color);
    }
  }

  renderShieldImpact(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    color: glm.ReadonlyVec3 | glm.ReadonlyVec4,
  ): void {
    this._shieldGraphicModel?.renderWireframe(stackRenderers, color);
    this._shieldGraphicModel?.renderPolyHurtOnly(trianglesStackRenderer, color);
  }

  renderHull(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    color: glm.ReadonlyVec3,
    colorHurt: glm.ReadonlyVec3,
    debugMode?: boolean,
  ): void {

    if (debugMode) {
      this._hullGraphicModel.renderWireframe(stackRenderers, color, true);
    } else {
      this._hullGraphicModel.renderPoly(trianglesStackRenderer, color);
    }
    this._hullGraphicModel.renderPolyHurtOnly(trianglesStackRenderer, colorHurt);
  }

  renderTurrets(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    debugMode?: boolean,
  ): void {
    for (const currTurret of this._turrets) {
      currTurret.render(stackRenderers, trianglesStackRenderer, debugMode);
    }
  }

  renderTurretsLogicDebug(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
  ): void {
    for (const currTurret of this._turrets) {
      currTurret.renderLogicDebug(stackRenderers, trianglesStackRenderer);
    }
  }

  protected onDeath(
    otherFaction: FlockFaction
  ) {

    const proxRadius = this._def.deathTrigger?.proximityRadius

    if (proxRadius === undefined) {
      return;
    }

    const closestResults = flocking._findClosestAgents(this, otherFaction, proxRadius);

    for (const currResult of closestResults) {
      if (currResult.distance < proxRadius) {
        currResult.agent.takeDamage(40);
      }
    }

  }

  isTrackable(): boolean {
    return this._def.type !== 'homing-missile';
  }

  get min(): glm.ReadonlyVec3 { return this._min; }
  get max(): glm.ReadonlyVec3 { return this._max; }

}





//MARK: CapitalFlockAgent


class CapitalFlockAgent extends AbstractFlockAgent implements IFlockAgent {

  protected _internalUpdate(
    deltaTimeSec: number,
    ownAgents: FlockFaction,
    otherFaction: FlockFaction,
    projectileManager: ProjectileManager,
  ) {

    const k_speed = this._def.speed;
    const k_correctionSpeed = k_speed * 0.2;

    //
    //
    //

    {

      const lookAcceleration = glm.vec3.fromValues(0,0,0);

      const closestResult = flocking._stayInRangeFlock(this, otherFaction, this._def.range - 5, this._def.range + 5, k_speed, lookAcceleration);
      if (!closestResult) {
        // no more targets -> strafe in the middle
        flocking._strafe(this, [0,0,10], 15, 1, lookAcceleration);
      }

      flocking._limitVec3(lookAcceleration, k_speed); // capital ship movements
      glm.vec3.scaleAndAdd(this._pos, this._pos, lookAcceleration, deltaTimeSec);

      const lookAxis = glm.vec3.copy(glm.vec3.create(), this._forwardAxis);

      // try to face the target
      if (closestResult) {

        if (closestResult.distance > 0) {

          const desiredLook = glm.vec3.create();
          glm.vec3.scale(desiredLook, closestResult.diff, 1 / closestResult.distance); // normalize

          // apply look
          glm.vec3.lerp(this._forwardAxis, this._forwardAxis, desiredLook, deltaTimeSec * 0.25);

          glm.vec3.scaleAndAdd(this._pos, this._pos, desiredLook, k_correctionSpeed * deltaTimeSec);
        }
      } else {

        // apply look
        glm.vec3.lerp(this._forwardAxis, this._forwardAxis, lookAcceleration, deltaTimeSec * 0.25);

        glm.vec3.scaleAndAdd(this._pos, this._pos, lookAcceleration, k_correctionSpeed * deltaTimeSec);

      }

      const yaw = Math.atan2(lookAxis[1], lookAxis[0]);
      const pitch = Math.atan2(-lookAxis[2], Math.sqrt(lookAxis[0]*lookAxis[0] + lookAxis[1]*lookAxis[1]));
      const safePitch = system.math.clamp(pitch, -Math.PI * 0.3, +Math.PI * 0.3);

      const tmpQuatYaw = glm.quat.setAxisAngle(glm.quat.create(), [0,0,1], yaw);
      const tmpQuatPitch = glm.quat.setAxisAngle(glm.quat.create(), [0,1,0], safePitch);

      glm.quat.identity(this._quat);
      glm.quat.multiply(this._quat, this._quat, tmpQuatYaw);
      glm.quat.multiply(this._quat, this._quat, tmpQuatPitch);

    }

    //
    //
    //

  }

}




//MARK: JetFighterFlockAgent

class JetFighterFlockAgent extends AbstractFlockAgent implements IFlockAgent {

  _internalUpdate(
    deltaTimeSec: number,
    ownAgents: FlockFaction,
    otherFaction: FlockFaction,
    projectileManager: ProjectileManager,
  ) {

    const k_speed = this._def.speed;

    //
    //
    //

    {

      const lookAcceleration = glm.vec3.fromValues(0,0,0);

      const closestResult = flocking._strafeFlock(this, otherFaction, this._def.range - 1, this._def.range + 1, k_speed, lookAcceleration);
      if (!closestResult) {
        // no more targets -> strafe in the middle
        flocking._strafe(this, [0,0,10], 15, 1, lookAcceleration);
      }

      // flocking._maximizeVec3(lookAcceleration, k_speed); // jet fighter movements

      // apply look
      glm.vec3.lerp(this._forwardAxis, this._forwardAxis, lookAcceleration, deltaTimeSec * 3.0);

      // limit look
      // flocking._maximizeVec3(this._forwardAxis, k_speed);

      const yaw = Math.atan2(this._forwardAxis[1], this._forwardAxis[0]);
      const pitch = Math.atan2(-this._forwardAxis[2], Math.sqrt(this._forwardAxis[0]*this._forwardAxis[0] + this._forwardAxis[1]*this._forwardAxis[1]));

      const tmpQuatYaw = glm.quat.setAxisAngle(glm.quat.create(), [0,0,1], yaw);
      const tmpQuatPitch = glm.quat.setAxisAngle(glm.quat.create(), [0,1,0], pitch);

      glm.quat.identity(this._quat);
      glm.quat.multiply(this._quat, this._quat, tmpQuatYaw);
      glm.quat.multiply(this._quat, this._quat, tmpQuatPitch);

      const tmpSpeed = glm.vec3.length(this._forwardAxis);

      // actually move
      const forwardAxis = glm.vec3.fromValues(1,0,0);
      glm.vec3.transformQuat(forwardAxis, forwardAxis, this._quat);
      glm.vec3.scaleAndAdd(this._pos, this._pos, forwardAxis, tmpSpeed * deltaTimeSec);

    }

    //
    //
    //

  }

}






//MARK: HomingMissileFlockAgent

class HomingMissileFlockAgent extends AbstractFlockAgent implements IFlockAgent {

  _internalUpdate(
    deltaTimeSec: number,
    ownAgents: FlockFaction,
    otherFaction: FlockFaction,
    projectileManager: ProjectileManager,
  ) {

    const k_speed = this._def.speed;

    //
    //
    //

    {

      const lookAcceleration = glm.vec3.fromValues(0,0,0);

      const closestResult = flocking._findClosestAgent(this, otherFaction);

      if (closestResult) {

        if (
          this._def.deathTrigger?.proximityRadius !== undefined &&
          // this can miss if the target separation radius is larger than the proximity radius
          closestResult.distance < this._def.deathTrigger.proximityRadius
        ) {
          this._hullGraphicModel.takeDamage(999999); // kamikaze
          this.onDeath(otherFaction);
        }
        else {

          const totalLength = this._def.speed * deltaTimeSec;

          let bestAgent: IFlockAgent | undefined;
          let bestDistance: number = 999999999;

          const bestData: system.math.collisions.IIntersectTriangle_raw_res = {
            distance: -1,
            normal: glm.vec3.fromValues(0,0,0),
          };

          const subResult = otherFaction.searchByRadius(this._pos, totalLength);
          for (const tmpAgent of subResult) {

            // collision using raycast
            // -> the forward axis is used for the ray direction
            const intersectionCoef = tmpAgent.testCollision_exp(this._pos, this._forwardAxis, bestData);

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
            this._hullGraphicModel.takeDamage(999999); // kamikaze
            this.onDeath(otherFaction);
          }


        }








        // get closer, straight line
        flocking._seek(this, closestResult.agent.getPosition(), k_speed, lookAcceleration);
      }
      else {
        // no more targets -> strafe in the middle
        flocking._strafe(this, [0,0,10], 10, 1, lookAcceleration);
      }

      flocking._maximizeVec3(lookAcceleration, k_speed); // jet fighter movements

      // apply acceleration
      glm.vec3.lerp(this._forwardAxis, this._forwardAxis, lookAcceleration, deltaTimeSec * 3.0);

      // maximize speed
      flocking._maximizeVec3(this._forwardAxis, k_speed);

      const yaw = Math.atan2(this._forwardAxis[1], this._forwardAxis[0]);
      const pitch = Math.atan2(-this._forwardAxis[2], Math.sqrt(this._forwardAxis[0]*this._forwardAxis[0] + this._forwardAxis[1]*this._forwardAxis[1]));

      const tmpQuatYaw = glm.quat.setAxisAngle(glm.quat.create(), [0,0,1], yaw);
      const tmpQuatPitch = glm.quat.setAxisAngle(glm.quat.create(), [0,1,0], pitch);

      glm.quat.identity(this._quat);
      glm.quat.multiply(this._quat, this._quat, tmpQuatYaw);
      glm.quat.multiply(this._quat, this._quat, tmpQuatPitch);

      const tmpSpeed = glm.vec3.length(this._forwardAxis);

      // actually move
      const forwardAxis = glm.vec3.fromValues(1,0,0);
      glm.vec3.transformQuat(forwardAxis, forwardAxis, this._quat);
      glm.vec3.scaleAndAdd(this._pos, this._pos, forwardAxis, tmpSpeed * deltaTimeSec);

    }

  }

}

















//MARK: FlockFaction

export type FlockingAgentBvhTreeNode = BvhTreeNode<IFlockAgent>;

export class FlockFaction {

  private _GraphicTrailsManager: GraphicTrailsManager;

  private _allAgents: IFlockAgent[] = [];

  private _bvhTree = new BvhTree<IFlockAgent>();

  constructor(GraphicTrailsManager: GraphicTrailsManager) {
    this._GraphicTrailsManager = GraphicTrailsManager;
  }

  spawn(def: IFlockAgentDef): IFlockAgent {
    switch (def.type) {
      case 'capital': {
        const newAgent = new CapitalFlockAgent(def)
        this._allAgents.push(newAgent);
        this._GraphicTrailsManager.createTrail(10, 30, newAgent);
        return newAgent;
      }
      case 'jet-fighter': {
        const newAgent = new JetFighterFlockAgent(def)
        this._allAgents.push(newAgent);
        this._GraphicTrailsManager.createTrail(10, 30, newAgent);
        return newAgent;
      }
      case 'homing-missile': {
        const newAgent = new HomingMissileFlockAgent(def)
        this._allAgents.push(newAgent);
        this._GraphicTrailsManager.createTrail(10, 30, newAgent);
        return newAgent;
      }
      default: {
        throw new Error('not implemented')
      }
    }
  }

  update(deltaTimeSec: number, otherFaction: FlockFaction, projectileManager: ProjectileManager) {

    for (let ii = 0; ii < this._allAgents.length; ) {
      if (this._allAgents[ii].isAlive()) {
        ++ii;
      } else {

        GlobalExplosionsManager.get().push(this._allAgents[ii].getPosition());

        this._allAgents.splice(ii, 1);
      }
    }

    for (const currAgent of this._allAgents) {
      currAgent.update(deltaTimeSec, this, otherFaction, projectileManager);
    }

    this._bvhTree.synchronize(this._allAgents);
  }

  get allAgents(): ReadonlyArray<IFlockAgent> {
    return this._allAgents;
  }

  searchByRadius(pos: glm.ReadonlyVec3, radius: number): IFlockAgent[] {
    return this._bvhTree.searchByRadius(pos, radius);
  }

  renderDebug(renderer: graphics.renderers.IStackRenderers) {
    BvhDebug.renderDebugWireframe(this._bvhTree.getRootNode(), renderer);
  }

};




