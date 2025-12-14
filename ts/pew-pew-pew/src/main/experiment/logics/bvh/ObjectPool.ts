
export interface ObjectPoolDef<T, TArgs extends any[]> {
  onCreateCallback: (...args: TArgs) => T;
  onInitCallback: (instance: T, ...args: TArgs) => void,
}

export class ObjectPool<T, TArgs extends any[]> {
// class ObjectPool<T extends ClassDecoratorContext, TArgs extends ConstructorParameters<typeof T>> {

  private _poolFree: T[] = [];
  private _poolUsed: T[] = [];

  private _def: ObjectPoolDef<T, TArgs>

  constructor(def: ObjectPoolDef<T, TArgs>) {
    this._def = def;
  }

  acquire(...args: TArgs): T {
    // try to acquire a free item
    if (this._poolFree.length > 0) {
      const reusedNode = this._poolFree.pop()!;
      this._def.onInitCallback(reusedNode, ...args);
      return reusedNode;
    }

    // could not acquire -> create item instead (will grow the pool)
    const newNode = this._def.onCreateCallback(...args);
    this._poolUsed.push(newNode);
    return newNode;
  }

  releaseAll() {
    // add the used node to the pool of free
    for (const currNode of this._poolUsed) {
      this._poolFree.push(currNode);
    }
    // clear the pool of used ones
    this._poolUsed.length = 0;
  }

};
