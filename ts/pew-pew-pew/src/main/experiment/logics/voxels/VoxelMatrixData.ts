
import * as glm from 'gl-matrix';

export class VoxelMatrixData {

  // private _scale: number = 8.0;
  private _scale: number = 2.0;
  private _size: glm.vec3 = glm.vec3.fromValues(0,0,0);
  private _data: Uint8Array | undefined;

  clear() {
    this._size[0] = 0;
    this._size[1] = 0;
    this._size[2] = 0;
    this._data = undefined;
  }

  allocate(sizeX: number, sizeY: number, sizeZ: number) {

    this.clear();
    if (sizeX <= 0 || sizeY <= 0 || sizeZ <= 0) {
      throw new Error(`invalid size: [${sizeX}, ${sizeY}, ${sizeZ}]`);
    }

    this._size[0] = sizeX;
    this._size[1] = sizeY;
    this._size[2] = sizeZ;
    this._data = new Uint8Array(sizeX * sizeY * sizeZ);
    this._data.fill(0);
    return true;
  }

  fillRectangles(x: number, y: number, z: number, sizeX: number, sizeY: number, sizeZ: number) {
    for (let zz = 0; zz < sizeZ; ++zz)
    for (let yy = 0; yy < sizeY; ++yy)
    for (let xx = 0; xx < sizeX; ++xx)
      this.set(x + xx, y + yy, z + zz, true);
  }

  digRectangles(x: number, y: number, z: number, sizeX: number, sizeY: number, sizeZ: number) {
    for (let zz = 0; zz < sizeZ; ++zz)
    for (let yy = 0; yy < sizeY; ++yy)
    for (let xx = 0; xx < sizeX; ++xx)
      this.set(x + xx, y + yy, z + zz, false);
  }

  get(x: number, y: number, z: number): boolean {
    if (this.isOutOfRange(x, y, z)) {
      return false;
    }

    return this._data![this._getIndex(x, y, z)] !== 0;
  }

  isColliding(
    x: number, y: number, z: number,
    minX: number, maxX: number,
    minY: number, maxY: number,
    minZ: number, maxZ: number
  ): boolean {

    const sqMinX = Math.floor((x - minX) / this._scale);
    const sqMinY = Math.floor((y - minY) / this._scale);
    const sqMinZ = Math.floor((z - minZ) / this._scale);
    const sqMaxX = Math.ceil((x + maxX) / this._scale);
    const sqMaxY = Math.ceil((y + maxY) / this._scale);
    const sqMaxZ = Math.ceil((z + maxZ) / this._scale);

    for (let zz = sqMinZ; zz < sqMaxZ; ++zz)
    for (let yy = sqMinY; yy < sqMaxY; ++yy)
    for (let xx = sqMinX; xx < sqMaxX; ++xx)
      if (this.get(xx, yy, zz))
        return true;

    return false;
  }

  set(x: number, y: number, z: number, value: boolean): void {
    if (this.isOutOfRange(x, y, z)) {
      return;
    }

    this._data![this._getIndex(x, y, z)] = (value ? 1 : 0);
  }

  getScale(): number { return this._scale; }
  getSize(): glm.ReadonlyVec3 { return this._size; }
  isValid(): boolean { return this._data !== undefined; }
  isOutOfRange(x: number, y: number, z: number): boolean {
    return (
      this.isValid() === false ||
      x < 0 || x >= this._size[0] ||
      y < 0 || y >= this._size[1] ||
      z < 0 || z >= this._size[2]
    );
  }

  private _getIndex(x: number, y: number, z: number): number {
    return z * this._size[0] * this._size[1] + y * this._size[0] + x;
  }
};

