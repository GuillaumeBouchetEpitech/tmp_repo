

import * as glm from 'gl-matrix';

import { FlockFaction, IFlockAgent, IFlockAgentDef } from "./FlockManager"

import { ProjectileManager, IProjectileInstanceDef } from './ProjectileManager';







//#region Weapon


export interface IWeaponDef {
  coolDownSec: number;
  warmUpSec: number;
  randomExtraCoolDownRange?: number;
  actionable: {
    type: 'projectile',
    projectileDef: IProjectileInstanceDef;
    sprayPercent?: number;
  } | {
    type: 'flocking-agent',
    flockAgentDef: IFlockAgentDef;
    maxDistance: number;
  };

};

export class Weapon {

  private _def: IWeaponDef;

  private _coolDownLeft: number = 0;
  private _warmUpLeft: number = 0;

  constructor(def: IWeaponDef) {
    this._def = def;
  }

  update(deltaTimeSec: number) {

    if (this._warmUpLeft > 0) {

      this._warmUpLeft -= deltaTimeSec;

      if (this._warmUpLeft < 0) {
        this._warmUpLeft = 0;
      }

    }
    else
    {

      if (this._coolDownLeft > 0) {
        this._coolDownLeft -= deltaTimeSec;
      }
      if (this._coolDownLeft < 0) {
        this._coolDownLeft = 0;
      }

    }

  }

  shoot(
    rootPos: glm.ReadonlyVec3,
    rootDir: glm.ReadonlyVec3,
    ownAgents: FlockFaction,
    projectileManager: ProjectileManager,
    target: IFlockAgent,
  ) {
    if (this._coolDownLeft > 0) {
      return;
    }

    //

    if (this._def.actionable.type === 'projectile') {

      const projDir = glm.vec3.copy(glm.vec3.create(), rootDir);

      if (this._def.actionable.sprayPercent !== undefined) {
        const spray = glm.vec3.fromValues(
          this._def.actionable.sprayPercent * (Math.random()-Math.random()),
          this._def.actionable.sprayPercent * (Math.random()-Math.random()),
          this._def.actionable.sprayPercent * (Math.random()-Math.random()),
        );
        glm.vec3.add(projDir, projDir, spray);
      }

      projectileManager.shoot(this._def.actionable.projectileDef, rootPos, projDir, target);
    }
    else if (this._def.actionable.type === 'flocking-agent') {

      // const desiredAim = glm.vec3.sub(glm.vec3.create(), target.getPosition(), rootPos);
      // const length = glm.vec3.length(desiredAim);
      // if (length > 0) {
      //   glm.vec3.scale(desiredAim, desiredAim, 1 / length); // normalize
      // }

      const newAgent = ownAgents.spawn(this._def.actionable.flockAgentDef);
      newAgent.setPosition(rootPos);
      // newAgent.setForwardAxis(desiredAim);
      newAgent.setForwardAxis(rootDir);

    }

    this._coolDownLeft = this._def.coolDownSec;

    if (this._def.randomExtraCoolDownRange) {
      const extraCollDown = Math.random() * this._def.randomExtraCoolDownRange;
      this._coolDownLeft += extraCollDown;
    }
  }

}


//#endregion


