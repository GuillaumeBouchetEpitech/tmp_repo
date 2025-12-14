

import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';






interface ITrianglesStackRenderer {
  pushTriangle(
    inPointA: glm.ReadonlyVec3,
    inPointB: glm.ReadonlyVec3,
    inPointC: glm.ReadonlyVec3,
    inColor: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    inNormal: glm.ReadonlyVec3,
  ): void;
}






export interface ITriangle {
  v0: glm.vec3;
  v1: glm.vec3;
  v2: glm.vec3;
  normal: glm.vec3;
  hurtTimeLeft: number;
}

export interface GraphicModelDef {
  triangles: ITriangle[];
  maxHealth: number;
};

export class GraphicModelBuilder {

  private _triangles: ITriangle[] = [];

  reset() {
    this._triangles.length = 0;
  }

  pushTriangle(
    v0: glm.ReadonlyVec3,
    v1: glm.ReadonlyVec3,
    v2: glm.ReadonlyVec3,
    normal: glm.ReadonlyVec3,
  ): void {

    const diff1 = glm.vec3.sub(glm.vec3.create(), v0, v1);
    const diff2 = glm.vec3.sub(glm.vec3.create(), v0, v2);

    const actualNormal = glm.vec3.cross(glm.vec3.create(), diff1, diff2);
    const length = glm.vec3.length(actualNormal);
    if (length > 0) {
      glm.vec3.scale(actualNormal, actualNormal, 1 / length) // normalize
    }

    // dot product [-1..1]
    // -> closer to +1 if aligned
    // -> closer to -1 if opposite
    const dotProd = glm.vec3.dot(normal, actualNormal);

    if (dotProd > 0) {
      this._triangles.push({
        v0: glm.vec3.copy(glm.vec3.create(), v0),
        v1: glm.vec3.copy(glm.vec3.create(), v1),
        v2: glm.vec3.copy(glm.vec3.create(), v2),
        normal: actualNormal,
        hurtTimeLeft: 0,
      });
    }
    else {
      // flip the triangle
      this._triangles.push({
        v0: glm.vec3.copy(glm.vec3.create(), v0),
        v1: glm.vec3.copy(glm.vec3.create(), v2),
        v2: glm.vec3.copy(glm.vec3.create(), v1),
        normal: glm.vec3.negate(actualNormal, actualNormal),
        hurtTimeLeft: 0
      });
    }

  }

  pushQuad(
    v0: glm.ReadonlyVec3,
    v1: glm.ReadonlyVec3,
    v2: glm.ReadonlyVec3,
    v3: glm.ReadonlyVec3,
    normal: glm.ReadonlyVec3,
  ) {
    this.pushTriangle(v0,v1,v2, normal);
    this.pushTriangle(v0,v2,v3, normal);
  }

  getDef(maxHealth: number): GraphicModelDef {
    return {
      maxHealth,
      triangles: [...this._triangles],
    };
  }

}


export class GraphicModel {

  private _pos = glm.vec3.fromValues(0, 0, 0);
  private _quat = glm.quat.identity(glm.quat.create());
  private _realTriangles: ITriangle[] = [];

  private _radius: number = -1;

  private _def: GraphicModelDef
  private _health: number;

  constructor(def: GraphicModelDef) {
    this._def = def;
    this._health = this._def.maxHealth;

    // this._radius
    for (const currTri of this._def.triangles) {
      const length0 = glm.vec3.length(currTri.v0);
      const length1 = glm.vec3.length(currTri.v1);
      const length2 = glm.vec3.length(currTri.v2);
      this._radius = Math.max(this._radius, length0, length1, length2);
    }

  }

  computeAABB(aabbMin: glm.vec3, aabbMax: glm.vec3) {
    glm.vec3.set(aabbMin, Number.MAX_SAFE_INTEGER, Number.MAX_SAFE_INTEGER, Number.MAX_SAFE_INTEGER);
    glm.vec3.set(aabbMax, Number.MIN_SAFE_INTEGER, Number.MIN_SAFE_INTEGER, Number.MIN_SAFE_INTEGER);
    for (const currTri of this._realTriangles) {
      for (let ii = 0; ii < 3; ++ii) {
        aabbMin[ii] = Math.min(aabbMin[ii], currTri.v0[ii], currTri.v1[ii], currTri.v2[ii]);
        aabbMax[ii] = Math.max(aabbMax[ii], currTri.v0[ii], currTri.v1[ii], currTri.v2[ii]);
      }
    }
  }

  testCollision(
    rayOrigin: glm.ReadonlyVec3,
    rayVelocity: glm.ReadonlyVec3,
  ): number {

    let intersectionCoef = system.math.collisions.intersectRaySphere(
      rayOrigin,
      rayVelocity,
      this._pos,
      this._radius
    );

    if (intersectionCoef < 0) {
      return -1;
    }

    let bestCoef = -1;
    let bestTriangle: ITriangle | undefined;

    for (const realTriangles of this._realTriangles) {

      let intersectionCoef = system.math.collisions.intersectTriangle(
        rayOrigin,
        rayVelocity,
        realTriangles.v0,
        realTriangles.v1,
        realTriangles.v2,
      );

      if (
        intersectionCoef >= 0 &&
        (
          bestCoef < 0 ||
          bestCoef > intersectionCoef
        )
      ) {
        bestCoef = intersectionCoef;
        bestTriangle = realTriangles;
      }
    }

    if (bestTriangle) {
      bestTriangle.hurtTimeLeft = 0.05;
    }

    return bestCoef;
  }

  testCollision_exp(
    rayOrigin: glm.ReadonlyVec3,
    rayVelocity: glm.ReadonlyVec3,
    outData: system.math.collisions.IIntersectTriangle_raw_res,
  ): number {

    let intersectionCoef = system.math.collisions.intersectRaySphere(
      rayOrigin,
      rayVelocity,
      this._pos,
      this._radius
    );

    if (intersectionCoef < 0) {
      return -1;
    }

    let bestCoef = -1;
    let bestTriangle: ITriangle | undefined;
    const bestOutData: system.math.collisions.IIntersectTriangle_raw_res = {
      distance: -1,
      normal: glm.vec3.fromValues(0,0,0),
    }
    const tmpOutData: system.math.collisions.IIntersectTriangle_raw_res = {
      distance: -1,
      normal: glm.vec3.fromValues(0,0,0),
    }

    for (const realTriangles of this._realTriangles) {

      let intersectionCoef = system.math.collisions.intersectTriangle_raw(
        rayOrigin,
        rayVelocity,
        realTriangles.v0,
        realTriangles.v1,
        realTriangles.v2,
        tmpOutData,
      );

      if (
        intersectionCoef >= 0 &&
        (
          bestCoef < 0 ||
          bestCoef > intersectionCoef
        )
      ) {
        bestCoef = intersectionCoef;
        bestTriangle = realTriangles;

        bestOutData.distance = tmpOutData.distance;
        glm.vec3.copy(bestOutData.normal, tmpOutData.normal);
      }
    }

    if (bestTriangle) {
      bestTriangle.hurtTimeLeft = 0.05;
    }

    if (
      bestOutData.distance > 0 && (
        outData.distance < 0 ||
        bestOutData.distance < outData.distance
      )
    ) {
      outData.distance = bestOutData.distance;
      glm.vec3.copy(outData.normal, bestOutData.normal);
    }

    return bestCoef;
  }

  takeDamage(damage: number) {
    if (this._health > 0) {
      this._health -= damage;
      // this._hurtTime = 0.1;
    }
  }

  isAlive(): boolean {
    return this._health > 0;
  }

  update(
    deltaTimeSec: number,
    pos: glm.ReadonlyVec3,
    quat: glm.ReadonlyQuat,
  ) {
    glm.vec3.copy(this._pos, pos);
    glm.quat.copy(this._quat, quat);

    for (let ii = 0; ii < this._def.triangles.length; ++ii) {

      const triangle = this._def.triangles[ii];

      if (triangle.hurtTimeLeft > 0) {
        triangle.hurtTimeLeft -= deltaTimeSec;
        if (triangle.hurtTimeLeft < 0) {
          triangle.hurtTimeLeft = 0;
        }
      }

      // ensure real triangle data
      while (this._realTriangles.length - 1 < ii) {
        this._realTriangles.push({
          v0: glm.vec3.create(),
          v1: glm.vec3.create(),
          v2: glm.vec3.create(),
          normal: glm.vec3.create(),
          hurtTimeLeft: 0,
        });
      }

      const realTriangle = this._realTriangles[ii];

      glm.vec3.add(realTriangle.v0, glm.vec3.transformQuat(realTriangle.v0, triangle.v0, this._quat), this._pos);
      glm.vec3.add(realTriangle.v1, glm.vec3.transformQuat(realTriangle.v1, triangle.v1, this._quat), this._pos);
      glm.vec3.add(realTriangle.v2, glm.vec3.transformQuat(realTriangle.v2, triangle.v2, this._quat), this._pos);
      glm.vec3.transformQuat(realTriangle.normal, triangle.normal, this._quat);

      realTriangle.hurtTimeLeft = triangle.hurtTimeLeft;
    }

  }

  renderWireframe(
    stackRenderers: graphics.renderers.IStackRenderers,
    color: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    debugMode?: boolean,
  ) {
    if (!this.isAlive()) {
      return;
    }

    for (const realTriangles of this._realTriangles) {

      if (!debugMode && realTriangles.hurtTimeLeft === 0) {
        continue;
      }

      stackRenderers.pushLine(realTriangles.v0, realTriangles.v1, color as glm.ReadonlyVec3);
      stackRenderers.pushLine(realTriangles.v1, realTriangles.v2, color as glm.ReadonlyVec3);
      stackRenderers.pushLine(realTriangles.v2, realTriangles.v0, color as glm.ReadonlyVec3);

    }
  }

  renderPoly(
    trianglesStackRenderer: ITrianglesStackRenderer,
    color: glm.ReadonlyVec3 | glm.ReadonlyVec4,
  ) {
    if (!this.isAlive()) {
      return;
    }

    for (const realTriangles of this._realTriangles) {

      // if (realTriangles.hurtTimeLeft > 0) {
      //   continue;
      // }

      const alpha = color[3] || 1;
      if (alpha <= 0) {
        continue;
      }

      trianglesStackRenderer.pushTriangle(
        realTriangles.v0,
        realTriangles.v1,
        realTriangles.v2,
        color,
        realTriangles.normal
      );

    }
  }

  renderPolyHurtOnly(
    trianglesStackRenderer: ITrianglesStackRenderer,
    color: glm.ReadonlyVec3 | glm.ReadonlyVec4,
  ) {
    if (!this.isAlive()) {
      return;
    }

    for (const realTriangles of this._realTriangles) {

      if (realTriangles.hurtTimeLeft === 0) {
        continue;
      }

      const alpha = color[3] || 1;
      if (alpha <= 0) {
        continue;
      }

      trianglesStackRenderer.pushTriangle(
        realTriangles.v0,
        realTriangles.v1,
        realTriangles.v2,
        color,
        realTriangles.normal
      );

    }
  }

};

