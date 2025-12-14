import * as glm from 'gl-matrix';

enum FrustumSide {
  Right = 0,
  Left = 1,
  Bottom = 2,
  Top = 3,
  Back = 4,
  Front = 5
}

export interface IFrustumCulling {
  calculateFrustum(proj: glm.ReadonlyMat4, view: glm.ReadonlyMat4): void;
  sphereInFrustum(x: number, y: number, z: number, radius: number): boolean;
  pointInFrustum(x: number, y: number, z: number): boolean;
  cubeInFrustum(inX: number, inY: number, inZ: number, inSize: number): boolean;
  cubeInFrustumVec3(center: glm.ReadonlyVec3, inSize: number): boolean;
  cubeInFrustumFromAABB(min: glm.ReadonlyVec3, max: glm.ReadonlyVec3): boolean;
}

export class FrustumCulling implements IFrustumCulling {
  private _frustum = new Float32Array(24); // 6 * 4 values

  private _setPlane(
    side: FrustumSide,
    left: glm.ReadonlyVec4,
    right: glm.ReadonlyVec4,
    coef: number
  ) {
    const index = side * 4;

    this._frustum[index + 0] = left[0] + right[0] * coef;
    this._frustum[index + 1] = left[1] + right[1] * coef;
    this._frustum[index + 2] = left[2] + right[2] * coef;
    this._frustum[index + 3] = left[3] + right[3] * coef;

    const magnitude = Math.sqrt(
      this._frustum[index + 0] * this._frustum[index + 0] +
        this._frustum[index + 1] * this._frustum[index + 1] +
        this._frustum[index + 2] * this._frustum[index + 2]
    );

    if (magnitude === 0) return;

    this._frustum[index + 0] /= magnitude;
    this._frustum[index + 1] /= magnitude;
    this._frustum[index + 2] /= magnitude;
    this._frustum[index + 3] /= magnitude;
  }

  calculateFrustum(proj: glm.ReadonlyMat4, view: glm.ReadonlyMat4) {
    const clip = glm.mat4.multiply(glm.mat4.create(), proj, view);

    ///

    const row0 = glm.vec4.fromValues(clip[0], clip[4], clip[8], clip[12]);
    const row1 = glm.vec4.fromValues(clip[1], clip[5], clip[9], clip[13]);
    const row2 = glm.vec4.fromValues(clip[2], clip[6], clip[10], clip[14]);
    const row3 = glm.vec4.fromValues(clip[3], clip[7], clip[11], clip[15]);

    this._setPlane(FrustumSide.Right, row3, row0, -1);
    this._setPlane(FrustumSide.Left, row3, row0, +1);
    this._setPlane(FrustumSide.Bottom, row3, row1, +1);
    this._setPlane(FrustumSide.Top, row3, row1, -1);
    this._setPlane(FrustumSide.Back, row3, row2, -1);
    this._setPlane(FrustumSide.Front, row3, row2, +1);
  }

  sphereInFrustum(x: number, y: number, z: number, radius: number) {
    for (let ii = 0; ii < 6; ++ii) {
      const index = ii * 4;
      if (
        this._frustum[index + 0] * x +
          this._frustum[index + 1] * y +
          this._frustum[index + 2] * z +
          this._frustum[index + 3] <=
        -radius
      ) {
        return false;
      }
    }

    return true;
  }

  pointInFrustum(x: number, y: number, z: number) {
    // sphere of radius 0 => point
    return this.sphereInFrustum(x, y, z, 0);
  }

  cubeInFrustumVec3(center: glm.ReadonlyVec3, inSize: number) {
    return this.cubeInFrustum(center[0], center[1], center[2], inSize);
  }

  cubeInFrustum(inX: number, inY: number, inZ: number, inSize: number) {
    return this.rawCubeInFrustum(inX, inY, inZ, inSize, inSize, inSize);
  }

  cubeInFrustumFromAABB(min: glm.ReadonlyVec3, max: glm.ReadonlyVec3) {
    const sizeX = max[0] - min[0];
    const sizeY = max[1] - min[1];
    const sizeZ = max[2] - min[2];
    const centerX = min[0] + sizeX * 0.5;
    const centerY = min[1] + sizeY * 0.5;
    const centerZ = min[2] + sizeZ * 0.5;

    return this.rawCubeInFrustum(centerX, centerY, centerZ, sizeX, sizeY, sizeZ);
  }

  rawCubeInFrustum(inX: number, inY: number, inZ: number, inSizeX: number, inSizeY: number, inSizeZ: number) {
    const hSizeX = inSizeX * 0.5;
    const hSizeY = inSizeY * 0.5;
    const hSizeZ = inSizeZ * 0.5;
    const minX = inX - hSizeX;
    const minY = inY - hSizeY;
    const minZ = inZ - hSizeZ;
    const maxX = inX + hSizeX;
    const maxY = inY + hSizeY;
    const maxZ = inZ + hSizeZ;

    for (let ii = 0; ii < 6; ++ii) {
      const index = ii * 4;
      const planA = this._frustum[index + 0];
      const planB = this._frustum[index + 1];
      const planC = this._frustum[index + 2];
      const planD = this._frustum[index + 3];

      if (
        planA * minX + planB * minY + planC * minZ + planD > 0 ||
        planA * maxX + planB * minY + planC * minZ + planD > 0 ||
        planA * minX + planB * maxY + planC * minZ + planD > 0 ||
        planA * maxX + planB * maxY + planC * minZ + planD > 0 ||
        planA * minX + planB * minY + planC * maxZ + planD > 0 ||
        planA * maxX + planB * minY + planC * maxZ + planD > 0 ||
        planA * minX + planB * maxY + planC * maxZ + planD > 0 ||
        planA * maxX + planB * maxY + planC * maxZ + planD > 0
      ) {
        continue;
      }

      return false;
    }

    return true;
  }
}
