
import* as glm from "gl-matrix"

import { BvhTreeNode, AABB, BvhEntryPool } from './BvhTreeNode';
import { ObjectPool } from './ObjectPool';

export class BvhTree<T extends AABB> {

  private _objectPool: BvhEntryPool<T>;
  private _rootNode?: BvhTreeNode<T>;

  constructor() {
    this._objectPool = new ObjectPool<BvhTreeNode<T>, [glm.ReadonlyVec3, glm.ReadonlyVec3]>({
      onCreateCallback: (
        min: glm.ReadonlyVec3,
        max: glm.ReadonlyVec3
      ): BvhTreeNode<T> => {
        return new BvhTreeNode<T>(min, max);
      },
      onInitCallback: (
        reused: BvhTreeNode<T>,
        min: glm.ReadonlyVec3,
        max: glm.ReadonlyVec3
      ): BvhTreeNode<T> => {
        reused.init(min, max);
        return reused
      }
    });

  }

  reset() {
    this._objectPool.releaseAll();
    this._rootNode = undefined;
  }

  synchronize(allEntries: ReadonlyArray<T>) {
    this.reset();
    this._rootNode = BvhTreeNode.buildBvhGraph(this._objectPool, allEntries);
  }

  getRootNode(): BvhTreeNode<T> | undefined {
    return this._rootNode;
  }

  searchByRadius(pos: glm.ReadonlyVec3, radius: number): T[] {
    if (!this._rootNode) {
      return [];
    }

    const min = glm.vec3.fromValues(pos[0] - radius, pos[1] - radius, pos[2] - radius);
    const max = glm.vec3.fromValues(pos[0] + radius, pos[1] + radius, pos[2] + radius);

    const results: T[] = [];
    this._rootNode.traverse(min, max, results);
    return results;
  }

};

