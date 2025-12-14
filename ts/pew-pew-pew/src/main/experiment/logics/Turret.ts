



import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';


import { ProjectileManager } from './ProjectileManager';

import { IWeaponDef, Weapon } from './Weapon';

import { FlockFaction, IFlockAgent } from "./FlockManager"

import { GraphicModel, GraphicModelBuilder, GraphicModelDef } from './GraphicModel';









interface ITrianglesStackRenderer {
  pushTriangle(
    inPointA: glm.ReadonlyVec3,
    inPointB: glm.ReadonlyVec3,
    inPointC: glm.ReadonlyVec3,
    inColor: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    inNormal: glm.ReadonlyVec3,
  ): void;
}












export interface ITurretDef {
  offsetVec3: glm.ReadonlyVec3;
  offsetQuat: glm.ReadonlyQuat;

  radialAimingSpeed: number;

  /**
   * "up axis dot range" -> limit the turret vertical axis angle (dot value)
   * * +1 is aligned to anchor up axis
   * * -1 is opposite to anchor up axis
   */
  upAxisDotRange: [number, number];

  weaponDef: IWeaponDef;
  debug?: boolean,

  basketModel: GraphicModelDef;
  gunMantlet: GraphicModelDef;
  gunBarrel: GraphicModelDef;
};


//
//
//

interface IAxises {
  forward: glm.vec3;
  left: glm.vec3;
  up: glm.vec3;
}

//
//
//

export class Turret {

  private _def: ITurretDef;

  private _pos = glm.vec3.fromValues(0,0,0);
  private _quat = glm.quat.identity(glm.quat.create());

  private _anchorAxises: IAxises = {
    forward: glm.vec3.fromValues(1,0,0),
    left: glm.vec3.fromValues(0,1,0),
    up: glm.vec3.fromValues(0,0,1),
  };
  private _aimAxises: IAxises = {
    forward: glm.vec3.fromValues(1,0,0),
    left: glm.vec3.fromValues(0,1,0),
    up: glm.vec3.fromValues(0,0,1),
  };

  private _desiredAimAxis = glm.vec3.fromValues(1,0,0);

  private _targetAgent: IFlockAgent | undefined;

  // 0 = perfect aim
  // 1 = opposite aim
  private _radialDistance: number = -1; // [0..1]

  private _weapons: Weapon[] = [];

  private _timeLeftToNewTarget: number = 0;

  private _basketGraphicModel: GraphicModel;
  private _gunMantletGraphicModel: GraphicModel;
  private _gunBarrelGraphicModel: GraphicModel;

  constructor(def: ITurretDef) {
    this._def = def;

    this._basketGraphicModel = new GraphicModel(def.basketModel);
    this._gunMantletGraphicModel = new GraphicModel(def.gunMantlet);
    this._gunBarrelGraphicModel = new GraphicModel(def.gunBarrel);

    this._weapons.push(new Weapon(this._def.weaponDef));
  }

  update(
    deltaTimeSec: number,
    rootPos: glm.ReadonlyVec3,
    rootQuat: glm.ReadonlyQuat,
    projectileManager: ProjectileManager,
    ownAgents: FlockFaction,
    otherFaction: FlockFaction,
  ) {

    // calculate forward/left/up "anchor" axises
    this._computeAnchorAxises(rootPos, rootQuat);

    // find a target
    this._findClosestTarget(deltaTimeSec, otherFaction);

    this._predictDesiredAimAxis();

    this._computeAimAxises(deltaTimeSec);

    for (const currWeapon of this._weapons) {
      currWeapon.update(deltaTimeSec);
    }

    if (
      // do we have a target?
      this._targetAgent &&
      // is the turret aligned?
      // this._radialDistance <= 0.003
      this._radialDistance === 0.0
    ) {

      // use weapons here

      for (const currWeapon of this._weapons) {
        currWeapon.shoot(
          this._pos,
          this._aimAxises.forward,
          ownAgents,
          projectileManager,
          this._targetAgent
        );
      }

    }
  }

  renderLogicDebug(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    // debugMode: boolean = false,
  ) {

    // if (this._def.debug || debugMode) {

      {
        const tmpPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, this._desiredAimAxis, 10);
        stackRenderers.pushLine(this._pos, tmpPos, [0.0,0.5,0.0]);
      }
      {
        const tmpPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, this._aimAxises.forward, 10);
        stackRenderers.pushLine(this._pos, tmpPos, [0.5,0.5,0.5]);
      }

      if (this._targetAgent) {
        const tmpPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, this._aimAxises.forward, 10);
        stackRenderers.pushLine(this._targetAgent.getPosition(), tmpPos, [0.5,0.0,0.0]);
      }

    // }
  }

  render(
    stackRenderers: graphics.renderers.IStackRenderers,
    trianglesStackRenderer: ITrianglesStackRenderer,
    debugMode: boolean = false,
  ) {

    // if (this._def.debug || debugMode) {

    //   {
    //     const tmpPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, this._desiredAimAxis, 10);
    //     stackRenderers.pushLine(this._pos, tmpPos, [0.0,0.5,0.0]);
    //   }
    //   {
    //     const tmpPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, this._aimAxises.forward, 10);
    //     stackRenderers.pushLine(this._pos, tmpPos, [0.5,0.5,0.5]);
    //   }

    //   if (this._targetAgent) {
    //     const tmpPos = glm.vec3.scaleAndAdd(glm.vec3.create(), this._pos, this._aimAxises.forward, 10);
    //     stackRenderers.pushLine(this._targetAgent.getPosition(), tmpPos, [0.5,0.0,0.0]);
    //   }

    // }

    //

    { // anchor


      this._basketGraphicModel.update(0, this._pos, this._quat);
      if (debugMode) {
        const color: glm.ReadonlyVec3 = [0,1,0];
        this._basketGraphicModel.renderWireframe(stackRenderers, color, true);
      } else {
        const color: glm.ReadonlyVec3 = this._targetAgent ? [1,1,1] : [1,0,0];
        this._basketGraphicModel.renderPoly(trianglesStackRenderer, color);
      }
      // this._basketGraphicModel.renderPolyHurtOnly(trianglesStackRenderer, color);

    } // anchor

    {
      const invQuat = glm.quat.invert(glm.quat.create(), this._quat);

      const invBaseLeft = glm.vec3.create();
      const invAimLeft = glm.vec3.create();
      const invAimForward = glm.vec3.create();

      glm.vec3.transformQuat(invBaseLeft, this._anchorAxises.left, invQuat);
      glm.vec3.transformQuat(invAimLeft, this._aimAxises.left, invQuat);
      glm.vec3.transformQuat(invAimForward, this._aimAxises.forward, invQuat);

      const theta = Math.atan2(invAimLeft[1], invAimLeft[0]) - Math.PI * 0.5;
      const phi = Math.atan2(-invAimForward[2], Math.sqrt(invAimForward[1]*invAimForward[1] + invAimForward[0]*invAimForward[0]));

      const tmpQuatH = glm.quat.rotateZ(glm.quat.create(), this._quat, theta);
      const tmpQuatV = glm.quat.rotateY(glm.quat.create(), tmpQuatH, phi);

      //
      //
      //

      { // turret base


        this._gunMantletGraphicModel.update(0, this._pos, tmpQuatH);

        if (debugMode) {
          const color: glm.ReadonlyVec3 = [0,1,0];
          this._gunMantletGraphicModel.renderWireframe(stackRenderers, color, true);
        } else {
          const color: glm.ReadonlyVec3 = [1,1,0];
          this._gunMantletGraphicModel.renderPoly(trianglesStackRenderer, color);
        }

      } // turret base

      //
      //
      //

      { // turret barrel

        this._gunBarrelGraphicModel.update(0, this._pos, tmpQuatV);

        if (debugMode) {
          const color: glm.ReadonlyVec3 = [0,1,0];
          this._gunBarrelGraphicModel.renderWireframe(stackRenderers, color, true);
        } else {
          const color: glm.ReadonlyVec3 = [1,1,1];
          this._gunBarrelGraphicModel.renderPoly(trianglesStackRenderer, color);
        }

        // if (debugMode) {
        //   this._gunBarrelGraphicModel.renderWireframe(stackRenderers, color);
        //   // this._gunBarrelGraphicModel.renderPolyHurtOnly(trianglesStackRenderer, color);
        // }

      } // turret barrel

      // if (debugMode) {
      //   const color: glm.ReadonlyVec3 = [0,1,0];
      //   this._basketGraphicModel.renderWireframe(stackRenderers, color);
      //   this._gunMantletGraphicModel.renderWireframe(stackRenderers, color);
      //   this._gunBarrelGraphicModel.renderWireframe(stackRenderers, color);
      // }

    }

  }

  private _computeAnchorAxises(
    rootPos: glm.ReadonlyVec3,
    rootQuat: glm.ReadonlyQuat,
  ): void {

    //
    // get the axises at the 'root'
    //

    // initialize
    glm.vec3.set(this._anchorAxises.forward, 1,0,0);
    glm.vec3.set(this._anchorAxises.left, 0,1,0);
    glm.vec3.set(this._anchorAxises.up, 0,0,1);

    // const realOffsetVec3 = glm.vec3.create();
    const realOffsetVec3 = this._pos; // hack: reusing _pos -> avoid allocation

    // get the real/scene-space offset position
    glm.vec3.transformQuat(realOffsetVec3, this._def.offsetVec3, rootQuat);

    // apply input+offset pos
    glm.vec3.add(this._pos, rootPos, realOffsetVec3);

    // apply input+offset quat
    glm.quat.multiply(this._quat, rootQuat, this._def.offsetQuat);

    // now safe to rotate
    glm.vec3.transformQuat(this._anchorAxises.forward, this._anchorAxises.forward, this._quat);
    glm.vec3.transformQuat(this._anchorAxises.left, this._anchorAxises.left, this._quat);
    glm.vec3.transformQuat(this._anchorAxises.up, this._anchorAxises.up, this._quat);
  }

  private _findClosestTarget(
    deltaTimeSec: number,
    otherFaction: FlockFaction,
  ): void {

    glm.vec3.copy(this._desiredAimAxis, this._anchorAxises.forward); //default

    const maxDistance = this._getMaxDistance();

    if (
      this._targetAgent &&
      this._isValidTarget(this._targetAgent, maxDistance) &&
      this._timeLeftToNewTarget > 0
    ) {
      this._timeLeftToNewTarget -= deltaTimeSec;
      return;
    }

    this._timeLeftToNewTarget = 1.5

    const subResults = otherFaction.searchByRadius(this._pos, maxDistance);

    this._targetAgent = undefined;
    this._radialDistance = -1; // opposite alignment -> turret state = not aligned
    for (const tmpAgent of subResults) {

      if (!this._isValidTarget(tmpAgent, maxDistance)) {
        continue;
      }

      this._targetAgent = tmpAgent;
    }
  }

  private _getMaxDistance(): number {
    if (this._def.weaponDef.actionable.type === 'projectile') {
      return this._def.weaponDef.actionable.projectileDef.maxDistance
    }
    if (this._def.weaponDef.actionable.type === 'flocking-agent') {
      return this._def.weaponDef.actionable.maxDistance;
    }
    return 0;
  }

  private _isValidTarget(
    tmpAgent: IFlockAgent,
    maxDistance: number
  ): boolean {

    if (!tmpAgent.isAlive()) {
      return false;
    }

    const distance = glm.vec3.dist(this._pos, tmpAgent.getPosition());

    if (
      // too close
      distance < 1 ||
      // out of range
      distance > maxDistance
    ) {
      return false;
    }

    const diff = glm.vec3.sub(glm.vec3.create(), tmpAgent.getPosition(), this._pos);
    glm.vec3.scale(diff, diff, 1 / distance); // normalize

    const dotVal = glm.vec3.dot(this._anchorAxises.up, diff);
    return (
      dotVal >= this._def.upAxisDotRange[0] &&
      dotVal <= this._def.upAxisDotRange[1]
    );
  }


  private _predictDesiredAimAxis() {

    //
    // shot prediction here
    //

    if (!this._targetAgent) {
      return;
    }

    const diff = glm.vec3.sub(glm.vec3.create(), this._targetAgent.getPosition(), this._pos);
    const length = glm.vec3.length(diff);
    if (length > 0) {
      glm.vec3.scale(diff, diff, 1 / length); // normalize
    } else {
      return;
    }

    // default: no prediction
    glm.vec3.copy(this._desiredAimAxis, diff);

    // can predict?
    if (this._def.weaponDef.actionable.type !== 'projectile') {
      return; // no -> skip
    }


    // can predict

    // determine this._desiredAimAxis
    // must determine future position(s)

    const projPosition = this._pos;
    const projSpeed = this._def.weaponDef.actionable.projectileDef.speed;
    const targetPosition = this._targetAgent.getPosition();
    const targetForward = this._targetAgent.getForwardAxis();
    const targetSpeed = glm.vec3.length(this._targetAgent.getForwardAxis());
    const targetDir = glm.vec3.scale(glm.vec3.create(), targetForward, targetSpeed > 0 ? 1 / targetSpeed : 1); // safe normalize

    let bestDistance: number = 99999999;

    const k_steps = 5;
    for (let ii = 0; ii < k_steps * 3; ++ii) {
      const ratio = ii / k_steps; // [0..3]

      // get future target position
      const futureTargetPos = glm.vec3.scaleAndAdd(glm.vec3.create(), targetPosition, targetDir, ratio * targetSpeed);

      // get future projectile direction
      const distanceToTarget = glm.vec3.distance(projPosition, futureTargetPos);
      if (distanceToTarget === 0) {
        continue; // cannot be normalized -> division by 0
      }
      const dirToFutureTargetPos = glm.vec3.sub(glm.vec3.create(), futureTargetPos, projPosition);
      glm.vec3.scale(dirToFutureTargetPos, dirToFutureTargetPos, 1 / distanceToTarget); // safe normalize

      // get future projectile position
      const futureProjPos = glm.vec3.scaleAndAdd(glm.vec3.create(), projPosition, dirToFutureTargetPos, ratio * projSpeed);

      // calculate if any closer from target
      const futureProfDistanceToFutureTarget = glm.vec3.distance(futureTargetPos, futureProjPos);

      if (bestDistance > futureProfDistanceToFutureTarget) {
        bestDistance = futureProfDistanceToFutureTarget;
        glm.vec3.copy(this._desiredAimAxis, dirToFutureTargetPos); // copy
      }
    }

  }

  private _computeAimAxises(
    deltaTimeSec: number,
  ): void {

    // dot product [-1..1]
    // -> closer to +1 if aligned
    // -> closer to -1 if opposite
    const dotVal = glm.vec3.dot(this._aimAxises.forward, this._desiredAimAxis);

    // 0 = perfect aim
    // 1 = opposite aim
    this._radialDistance = 1 - (dotVal + 1) * 0.5; // [0..1]

    const radialStep = this._def.radialAimingSpeed * deltaTimeSec;

    // update aim forward axis
    if (this._radialDistance <= radialStep) {
      // close enough -> set
      glm.vec3.copy(this._aimAxises.forward, this._desiredAimAxis);
      this._radialDistance = 0;
    } else {
      // too far -> interpolate
      glm.vec3.lerp(this._aimAxises.forward, this._aimAxises.forward, this._desiredAimAxis, radialStep);
    }

    // safe normalize the aim forward axis
    const length = glm.vec3.length(this._aimAxises.forward);
    if (length > 0) {
      glm.vec3.scale(this._aimAxises.forward, this._aimAxises.forward, 1 / length); // normalize
    }

    // update aim left axis
    glm.vec3.cross(this._aimAxises.left, this._anchorAxises.up, this._aimAxises.forward);
    // update aim up axis
    glm.vec3.cross(this._aimAxises.up, this._aimAxises.forward, this._aimAxises.left);
  }

};













