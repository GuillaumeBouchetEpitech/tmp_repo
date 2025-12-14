

import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';

import { ProjectileManager } from './ProjectileManager';

import { IWeaponDef } from './Weapon';

import { FlockFaction, IFlockAgent, IFlockAgentDef } from './FlockManager';

import { GlobalExplosionsManager } from './ExplosionsManager';
import { GlobalParticlesManager } from './ParticlesManager';


// import { CinematicController } from './CinematicController';

import { WebGLRenderer } from '../graphics/WebGLRenderer';
import { GraphicModelBuilder } from 'main/experiment/logics/GraphicModel';
import { GraphicTrailsManager } from 'main/experiment/logics/GraphicTrailsManager';




interface ISimulationStats {
  totalEntities: number;
  totalVisible: number;
}

export class Simulation {

  private _graphicTrailsManager = new GraphicTrailsManager();

  private _factionA: FlockFaction = new FlockFaction(this._graphicTrailsManager);
  private _factionB: FlockFaction = new FlockFaction(this._graphicTrailsManager);

  private _projectileManagerA = new ProjectileManager(this._graphicTrailsManager);
  private _projectileManagerB = new ProjectileManager(this._graphicTrailsManager);

  private _stats: ISimulationStats = {
    totalEntities: 0,
    totalVisible: 0,
  };

  // private _controller = new CinematicController();

  constructor() {

    GlobalExplosionsManager.create();
    GlobalParticlesManager.create();

    //
    //
    //

    //
    //
    //

    //
    //
    //

    const builder = new GraphicModelBuilder();

    builder.reset();
    {
      const vertices = graphics.geometries.makeSphere(1, 2);

      const offset: glm.ReadonlyVec3 = [0,0,-0.25];

      for (let ii = 0; ii < vertices.length; ii += 3) {

        // scale the sphere into an "ellipsoid" (<- 3d ellipse)
        const v0 = glm.vec3.multiply(glm.vec3.create(), vertices[ii + 0].pos, [1.4,0.6,0.6]);
        const v1 = glm.vec3.multiply(glm.vec3.create(), vertices[ii + 1].pos, [1.4,0.6,0.6]);
        const v2 = glm.vec3.multiply(glm.vec3.create(), vertices[ii + 2].pos, [1.4,0.6,0.6]);

        // move it down a bit to match the ship center
        glm.vec3.add(v0, v0, offset);
        glm.vec3.add(v1, v1, offset);
        glm.vec3.add(v2, v2, offset);

        // const normal = vertices[ii + 2].normal;

        // need to recompute the normal since we scaled the sphere
        const v01 = glm.vec3.sub(glm.vec3.create(), v0, v1);
        const v02 = glm.vec3.sub(glm.vec3.create(), v0, v2);
        const normal = glm.vec3.cross(glm.vec3.create(), v01, v02);

        builder.pushTriangle(v0, v1, v2, normal)
      }
    }
    const capitalShieldDef = builder.getDef(200);

    builder.reset();
    {
      const frontNose = glm.vec3.fromValues(2, 0, 0.05);
      const backNose = glm.vec3.fromValues(-2, 0, 0.05);

      const frontUp = glm.vec3.fromValues(0.75, 0, 0.3);
      const frontLeft = glm.vec3.fromValues(0.75,+0.4, -0.4);
      const frontRight = glm.vec3.fromValues(0.75,-0.4, -0.4);

      const backUp = glm.vec3.fromValues(-0.75, 0, 0.3);
      const backLeft = glm.vec3.fromValues(-0.75,+0.4, -0.4);
      const backRight = glm.vec3.fromValues(-0.75,-0.4, -0.4);

      const frontDown = glm.vec3.fromValues(+0.5, 0, -0.9);
      const backDown = glm.vec3.fromValues(-0.5, 0, -0.9);

      builder.pushTriangle(frontNose, frontLeft, frontUp, [0,1,0]);
      builder.pushTriangle(frontNose, frontRight, frontUp, [0,-1,0]);
      builder.pushTriangle(frontNose, frontLeft, frontRight, [0,0,-1]);

      builder.pushTriangle(backNose, backLeft, backUp, [0,1,0]);
      builder.pushTriangle(backNose, backRight, backUp, [0,-1,0]);
      builder.pushTriangle(backNose, backLeft, backRight, [0,0,-1]);

      builder.pushQuad(frontUp, backUp, backRight, frontRight, [0,-1, 0]);
      builder.pushQuad(frontUp, backUp, backLeft, frontLeft, [0,+1, 0]);

      builder.pushQuad(frontDown, backDown, backRight, frontRight, [0,0,-1]);
      builder.pushQuad(frontDown, backDown, backLeft, frontLeft, [0,0,-1]);

      builder.pushTriangle(frontDown, frontRight, frontLeft, [1,0,0]);
      builder.pushTriangle(backDown, backRight, backLeft, [-1,0,0]);
    }
    const capitalHullDef = builder.getDef(100);

    builder.reset();
    {
      const frontNose = glm.vec3.fromValues(0.4, 0, 0);
      const backUp = glm.vec3.fromValues(-0.35, 0, +0.15);
      const backDown = glm.vec3.fromValues(-0.35, 0, -0.15);
      const backLeft = glm.vec3.fromValues(-0.35,+0.15, 0);
      const backRight = glm.vec3.fromValues(-0.35,-0.15, 0);

      builder.pushTriangle(frontNose, backLeft, backUp, [0,0,1]);
      builder.pushTriangle(frontNose, backRight, backUp, [0,0,1]);
      builder.pushTriangle(frontNose, backLeft, backDown, [0,0,-1]);
      builder.pushTriangle(frontNose, backRight, backDown, [0,0,-1]);
      builder.pushTriangle(backDown, backLeft, backUp, [-1,0,0]);
      builder.pushTriangle(backDown, backRight, backUp, [-1,0,0]);
    }
    const missileHullDef = builder.getDef(5);

    builder.reset();
    {
      const frontNose = glm.vec3.fromValues(0.5, 0, 0);
      const backUp = glm.vec3.fromValues(-0.25, 0, +0.25);
      const backDown = glm.vec3.fromValues(-0.25, 0, -0.25);
      const backLeft = glm.vec3.fromValues(-0.45,+0.75, 0);
      const backRight = glm.vec3.fromValues(-0.45,-0.75, 0);

      builder.pushTriangle(frontNose, backLeft, backUp, [0,0,1]);
      builder.pushTriangle(frontNose, backRight, backUp, [0,0,1]);
      builder.pushTriangle(frontNose, backLeft, backDown, [0,0,-1]);
      builder.pushTriangle(frontNose, backRight, backDown, [0,0,-1]);
      builder.pushTriangle(backDown, backLeft, backUp, [-1,0,0]);
      builder.pushTriangle(backDown, backRight, backUp, [-1,0,0]);
    }
    const fighterHullDef = builder.getDef(30);

    // [Turret Basket] -> Gun Mantlet -> Gun Barrel
    builder.reset();
    builder.pushTriangle(
      [+0.25, +0.00, -0.10],
      [-0.13, +0.25, -0.10],
      [-0.13, -0.25, -0.10],
      [0,0,1],
    );
    const turretBasketDef = builder.getDef(1);

    // Turret Basket -> [Gun Mantlet] -> Gun Barrel
    builder.reset();
    builder.pushQuad(
      [+0.06, -0.13, -0.08],
      [+0.06, +0.13, -0.08],
      [-0.06, +0.13, -0.08],
      [-0.06, -0.13, -0.08],
      [0,0,1],
    );
    // left wall
    builder.pushQuad(
      [+0.06, +0.13, -0.08],
      [+0.06, +0.06, +0.13],
      [-0.06, +0.06, +0.13],
      [-0.06, +0.13, -0.08],
      [0,1,0],
    );
    // right wall
    builder.pushQuad(
      [+0.06, -0.13, -0.08],
      [+0.06, -0.06, +0.13],
      [-0.06, -0.06, +0.13],
      [-0.06, -0.13, -0.08],
      [0,-1,0],
    );
    // arrowhead
    builder.pushTriangle(
      [+0.06, -0.13, -0.08],
      [+0.20, +0.00, -0.08],
      [+0.06, +0.13, -0.08],
      [0,0,1],
    );
    // Turret Basket -> [Gun Mantlet] -> Gun Barrel
    const gunMantletDef = builder.getDef(1);

    // Turret Basket -> Gun Mantlet -> [Gun Barrel]
    builder.reset();
    // left
    builder.pushQuad(
      [+0.45, +0.06, +0.00],
      [+0.45, +0.00, +0.13],
      [+0.00, +0.00, +0.13],
      [+0.00, +0.06, +0.00],
      [0,1,-1],
    );
    // right
    builder.pushQuad(
      [+0.45, -0.06, +0.00],
      [+0.45, -0.00, +0.13],
      [+0.00, -0.00, +0.13],
      [+0.00, -0.06, +0.00],
      [0,-1,-1],
    );
    // bottom
    builder.pushQuad(
      [+0.45, -0.06, +0.00],
      [+0.45, +0.06, +0.00],
      [+0.00, +0.06, +0.00],
      [+0.00, -0.06, +0.00],
      [0,0,-1],
    );
    // Turret Basket -> Gun Mantlet -> [Gun Barrel]
    const gunBarrelDef = builder.getDef(1);

    //
    //
    //

    //
    //
    //

    //
    //
    //

    // jet fighter ballistic turret
    const smallBallisticWeaponDef: IWeaponDef = {
      warmUpSec: 0,
      coolDownSec: 0.06125,
      randomExtraCoolDownRange: 0.001,
      actionable: {
        type: 'projectile',
        sprayPercent: 0.01,
        projectileDef: {
          maxDistance: 15,
          damage: 0.5,
          speed: 50,
          homing: false,
        }
      }

    };

    // capital ballistic turret
    const superBallisticWeaponDef: IWeaponDef = {
      warmUpSec: 0,
      coolDownSec: 0.06125,
      randomExtraCoolDownRange: 0.001,
      actionable: {
        type: 'projectile',
        sprayPercent: 0.01,
        projectileDef: {
          maxDistance: 25,
          damage: 0.5,
          speed: 50,
          homing: false,
        }
      }
    };

    const missileAgentDef: IFlockAgentDef = {
      speed: 3 * 4,
      type: 'homing-missile',
      range: 0.5,
      battlefieldSize: 60,
      separateRadius: 1,

      turrets: [],

      hull: missileHullDef,

      deathTrigger: {
        maxDistance: 60,
        proximityRadius: 0.5
      }
    };

    const fighterDef: IFlockAgentDef = {
      speed: 3 * 2,
      type: 'jet-fighter',
      range: 10,
      battlefieldSize: 50,
      separateRadius: 2,

      hull: fighterHullDef,

      turrets: [
        {
          offsetVec3: [0,+0.0,0.25],
          // offsetQuat: glm.quat.identity(glm.quat.create()),
          offsetQuat: glm.quat.setAxisAngle(glm.quat.create(), [0,1,0], Math.PI * +0.1),
          radialAimingSpeed: 15,
          upAxisDotRange: [-0.5, 0.9],
          weaponDef: smallBallisticWeaponDef,
          basketModel: turretBasketDef,
          gunMantlet: gunMantletDef,
          gunBarrel: gunBarrelDef,
        }
      ],
    };

    // capital missile turret
    const missileWeaponDef2: IWeaponDef = {
      warmUpSec: 0.0,
      coolDownSec: 6.0,
      actionable: {
        type: 'flocking-agent',
        // flockAgentDef: fighterDef,
        flockAgentDef: missileAgentDef,
        maxDistance: 50,
      },
    };

    const capitalDef: IFlockAgentDef = {
      speed: 1 * 1.5,
      type: 'capital',
      range: 20,
      battlefieldSize: 40,
      separateRadius: 4,

      shield: capitalShieldDef,
      hull: capitalHullDef,

      turrets: [
        {
          offsetVec3: [+0.40,-0.25,0.15],
          offsetQuat: glm.quat.setAxisAngle(glm.quat.create(), [1,0,0], Math.PI * 0.35),
          radialAimingSpeed: 5,
          upAxisDotRange: [-0.1, 0.9],
          weaponDef: superBallisticWeaponDef,
          basketModel: turretBasketDef,
          gunMantlet: gunMantletDef,
          gunBarrel: gunBarrelDef,
          // debug: true,
        },
        {
          offsetVec3: [+0.40,+0.25,0.15],
          offsetQuat: glm.quat.setAxisAngle(glm.quat.create(), [1,0,0], Math.PI * -0.35),
          radialAimingSpeed: 5,
          upAxisDotRange: [-0.1, 0.9],
          weaponDef: superBallisticWeaponDef,
          basketModel: turretBasketDef,
          gunMantlet: gunMantletDef,
          gunBarrel: gunBarrelDef,
          // debug: true,
        },

        {
          offsetVec3: [-0.00,-0.35,-0.65],
          offsetQuat: glm.quat.setAxisAngle(glm.quat.create(), [1,0,0], Math.PI * +0.75),
          radialAimingSpeed: 5,
          upAxisDotRange: [-0.1, 0.9],
          weaponDef: superBallisticWeaponDef,
          basketModel: turretBasketDef,
          gunMantlet: gunMantletDef,
          gunBarrel: gunBarrelDef,
          // debug: true,
        },
        {
          offsetVec3: [-0.00,+0.35,-0.65],
          offsetQuat: glm.quat.setAxisAngle(glm.quat.create(), [1,0,0], Math.PI * -0.75),
          radialAimingSpeed: 5,
          upAxisDotRange: [-0.1, 0.9],
          weaponDef: superBallisticWeaponDef,
          basketModel: turretBasketDef,
          gunMantlet: gunMantletDef,
          gunBarrel: gunBarrelDef,
          // debug: true,
        },

        {
          offsetVec3: [-0.40,-0.45,-0.05],
          offsetQuat: glm.quat.setAxisAngle(glm.quat.create(), [1,0,0], Math.PI * 0.35),
          radialAimingSpeed: 5,
          upAxisDotRange: [-0.1, 0.9],
          weaponDef: missileWeaponDef2,
          basketModel: turretBasketDef,
          gunMantlet: gunMantletDef,
          gunBarrel: gunBarrelDef,
        },
        {
          offsetVec3: [-0.40,+0.45,-0.05],
          offsetQuat: glm.quat.setAxisAngle(glm.quat.create(), [1,0,0], Math.PI * -0.35),
          radialAimingSpeed: 5,
          upAxisDotRange: [-0.1, 0.9],
          weaponDef: missileWeaponDef2,
          basketModel: turretBasketDef,
          gunMantlet: gunMantletDef,
          gunBarrel: gunBarrelDef,
        },

      ],
    };

    //
    //
    //

    //
    //
    //

    //
    //
    //

    for (let zz = 0; zz < 1; ++zz)
    for (let yy = 0; yy < 2; ++yy)
    for (let xx = 0; xx < 1; ++xx) {

      const newFighterCapitalA = this._factionA.spawn(capitalDef);
      newFighterCapitalA.setPosition([1+xx*2 - 40,1+yy*2,1+zz*2+5]);
      newFighterCapitalA.setForwardAxis([+1,0,0]);

      const newFighterCapitalB = this._factionB.spawn(capitalDef);
      newFighterCapitalB.setPosition([1+xx + 40,1+yy,1+zz+5]);
      newFighterCapitalB.setForwardAxis([-1,0,0]);
    }

    for (let zz = 0; zz < 3; ++zz)
    for (let yy = 0; yy < 3; ++yy)
    for (let xx = 0; xx < 3; ++xx) {

      const newFighterAgentA = this._factionA.spawn(fighterDef);
      newFighterAgentA.setPosition([1+xx - 35,1+yy,1+zz+5]);
      newFighterAgentA.setForwardAxis([+1,0,0]);

      const newFighterAgentB = this._factionB.spawn(fighterDef);
      newFighterAgentB.setPosition([1+xx + 35,1+yy,1+zz+5]);
      newFighterAgentB.setForwardAxis([-1,0,0]);
    }

    //
    //
    //

    //
    //
    //

    //
    //
    //

  }

  update(
    deltaTimeSec: number,
    renderer: WebGLRenderer,
  ) {

    this._factionA.update(
      deltaTimeSec,
      this._factionB,
      this._projectileManagerA
    );

    this._factionB.update(
      deltaTimeSec,
      this._factionA,
      this._projectileManagerB
    );

    this._projectileManagerA.update(deltaTimeSec, this._factionB);
    this._projectileManagerB.update(deltaTimeSec, this._factionA);

    GlobalExplosionsManager.get().update(deltaTimeSec);
    GlobalParticlesManager.get().update(deltaTimeSec);

    // renderer.lookAt(
    //   this._controller.getPosition(),
    //   this._controller.getTarget(),
    //   this._controller.getUpAxis()
    // );

    // this._controller.update(
    //   [...this._factionA.allAgents, ...this._factionB.allAgents],
    //   renderer
    // );

    // renderer.lookAt(
    //   this._controller.getPosition(),
    //   this._controller.getTarget(),
    //   this._controller.getUpAxis()
    // );

    this._graphicTrailsManager.update();
  }

  render(
    renderer: WebGLRenderer,
    debugMode: number = 0,
  ): void {

    const debugBvh = debugMode === 1;
    const debugTurrets = debugMode === 3;
    const debugAgents = debugTurrets || debugMode === 2;

    this._stats.totalEntities = this._factionA.allAgents.length + this._factionB.allAgents.length;
    this._stats.totalVisible = 0;

    const colorA: glm.ReadonlyVec3 = [0.4,0.4,1.0];
    const colorAHurt: glm.ReadonlyVec3 = [0.8,0.8,1.0];
    const colorB: glm.ReadonlyVec3 = [1.0,0.4,0.4];
    const colorBHurt: glm.ReadonlyVec3 = [1.0,0.8,0.8];

    const factionA_visibleAgents: IFlockAgent[] = [];
    for (const currAgent of this._factionA.allAgents) {
      if (renderer.frustumCulling.cubeInFrustumFromAABB(currAgent.min, currAgent.max)) {
        factionA_visibleAgents.push(currAgent);
      }
    }

    const factionB_visibleAgents: IFlockAgent[] = [];
    for (const currAgent of this._factionB.allAgents) {
      if (renderer.frustumCulling.cubeInFrustumFromAABB(currAgent.min, currAgent.max)) {
        factionB_visibleAgents.push(currAgent);
      }
    }

    this._stats.totalVisible = factionA_visibleAgents.length + factionB_visibleAgents.length;

    const gl = graphics.webgl2.WebGLContext.getContext();
    gl.enable(gl.CULL_FACE);

    if (debugTurrets) {

      for (const currAgent of this._factionA.allAgents) {
        currAgent.renderTurretsLogicDebug(renderer.stackRenderers, renderer.litTrianglesStackRenderer);
      }

      renderer.flushScene();

      for (const currAgent of this._factionB.allAgents) {
        currAgent.renderTurretsLogicDebug(renderer.stackRenderers, renderer.litTrianglesStackRenderer);
      }

      renderer.flushScene();
    }

    for (const currAgent of factionA_visibleAgents) {
      currAgent.renderHull(renderer.stackRenderers, renderer.litTrianglesStackRenderer, colorA, colorAHurt, debugAgents);
      currAgent.renderTurrets(renderer.stackRenderers, renderer.litTrianglesStackRenderer, debugAgents);
    }

    renderer.flushScene();

    for (const currAgent of factionB_visibleAgents) {
      currAgent.renderHull(renderer.stackRenderers, renderer.litTrianglesStackRenderer, colorB, colorBHurt, debugAgents);
      currAgent.renderTurrets(renderer.stackRenderers, renderer.litTrianglesStackRenderer, debugAgents);
    }

    renderer.flushScene();

    this._projectileManagerA.render(renderer.stackRenderers, renderer.frustumCulling, colorA);

    renderer.flushScene();

    this._projectileManagerB.render(renderer.stackRenderers, renderer.frustumCulling, colorB);

    renderer.flushScene();

    this._graphicTrailsManager.render(renderer.stackRenderers);

    renderer.flushScene();

    gl.disable(gl.CULL_FACE);

    // this is NOT transparent -> no need to sort the rendering order by depth

    for (const currAgent of factionA_visibleAgents) {
      currAgent.renderShieldImpact(renderer.stackRenderers, renderer.litTrianglesStackRenderer, [1,1,1, 1.0]);
    }
    for (const currAgent of factionB_visibleAgents) {
      const pos = currAgent.getPosition();
      if (!renderer.frustumCulling.sphereInFrustum(pos[0], pos[1], pos[2], 3)) {
        continue;
      }
      currAgent.renderShieldImpact(renderer.stackRenderers, renderer.litTrianglesStackRenderer, [1,1,1, 1.0]);
    }

    renderer.flushScene();

    gl.enable(gl.CULL_FACE);

    // this IS transparent -> MUST sort the rendering order by depth

    const rawListA: { depth: number; agent: IFlockAgent; }[] = [];

    for (const currAgent of factionA_visibleAgents) {

      const screenCoord = graphics.camera.sceneToScreenCoordinates(
        currAgent.getPosition(),
        renderer.mainCamera.getViewMatrix(),
        renderer.mainCamera.getProjectionMatrix(),
        [0,0, renderer.size[0], renderer.size[1]],
      );

      if (!screenCoord) {
        continue;
      }

      rawListA.push({
        depth: screenCoord[2],
        agent: currAgent,
      });
    }

    const rawListB: { depth: number; agent: IFlockAgent; }[] = [];

    for (const currAgent of factionB_visibleAgents) {

      const screenCoord = graphics.camera.sceneToScreenCoordinates(
        currAgent.getPosition(),
        renderer.mainCamera.getViewMatrix(),
        renderer.mainCamera.getProjectionMatrix(),
        [0,0, renderer.size[0], renderer.size[1]],
      );

      if (!screenCoord) {
        continue;
      }

      rawListB.push({
        depth: screenCoord[2],
        agent: currAgent,
      });
    }

    rawListA.sort((a, b) => a.depth - b.depth)
    rawListB.sort((a, b) => a.depth - b.depth)


    for (const currAgent of rawListA) {
      currAgent.agent.renderShieldIdle(
        renderer.stackRenderers,
        renderer.litTrianglesStackRenderer, [1,1,1, 0.2], debugAgents);
    }

    for (const currAgent of rawListB) {
      currAgent.agent.renderShieldIdle(
        renderer.stackRenderers,
        renderer.litTrianglesStackRenderer, [1,1,1, 0.2], debugAgents);
    }

    renderer.flushScene();

    GlobalExplosionsManager.get().render(renderer.stackRenderers);

    renderer.flushScene();

    GlobalParticlesManager.get().render(renderer.stackRenderers);

    renderer.flushScene();

    if (debugBvh) {
      this._factionA.renderDebug(renderer.stackRenderers);
      renderer.flushScene();
      this._factionB.renderDebug(renderer.stackRenderers);
      renderer.flushScene();
    }
  }

  getAllAgents(): IFlockAgent[] {
    return [...this._factionA.allAgents, ...this._factionB.allAgents];
  }

  getStats(): ISimulationStats {
    return this._stats;
  }

};