
// import { system, graphics } from '@local-framework';

// import { WebGLRenderer } from '../graphics/WebGLRenderer';

// import * as glm from 'gl-matrix';

// class MyCamera {
//   private _camera = new graphics.camera.Camera();
//   private _frustumCulling = new graphics.camera.FrustumCulling();

//   // private _position = glm.vec3.fromValues(0, 0, 0);
//   private _target = glm.vec3.fromValues(0, 0, 0);
//   private _upAxis = glm.vec3.fromValues(0, 0, 1);

//   constructor() {

//   }

//   set(
//     mainCamera: Readonly<graphics.camera.ICamera>,
//     distance: number,
//   ) {
//     this._camera.setAsPerspective(mainCamera.getPerspectiveData()!);
//     const tmpPos1 = glm.vec3.create();
//     glm.vec3.copy(tmpPos1, this._target);
//     glm.vec3.add(tmpPos1, tmpPos1, [-5,distance+0,distance+0]);
//     this._camera.lookAt(tmpPos1, this._target, this._upAxis);
//     this._camera.computeMatrices();
//     this._frustumCulling.calculateFrustum(
//       this._camera.getProjectionMatrix(),
//       this._camera.getViewMatrix()
//     );
//   }

//   getTotalHidden(
//     allAgents: { getPosition(): glm.ReadonlyVec3; }[]
//   ): number {

//     let totalHidden: number = 0;

//     for (const currAgent of allAgents) {
//       const currPos = currAgent.getPosition();
//       if (!this._frustumCulling.pointInFrustum(currPos[0], currPos[1], currPos[2])) {
//         totalHidden += 1;
//       }
//     }

//     return totalHidden;
//   }

// }

// export class CinematicController {

//   private _position = glm.vec3.fromValues(0, 0, 0);
//   private _target = glm.vec3.fromValues(0, 0, 0);
//   private _upAxis = glm.vec3.fromValues(0, 0, 1);

//   private _distance: number = 15;

//   private _cameraNear = new MyCamera();
//   private _cameraFar = new MyCamera();

//   constructor() {
//   }

//   update(
//     allAgents: { getPosition(): glm.ReadonlyVec3; }[],
//     renderer: WebGLRenderer,
//   ) {

//     if (allAgents.length === 0) {
//       return;
//     }

//     const pData = renderer.mainCamera.getPerspectiveData();
//     if (!pData) {
//       throw new Error("LOL?");
//     }

//     this._cameraNear.set(renderer.mainCamera, this._distance - 1);
//     this._cameraFar.set(renderer.mainCamera, this._distance + 1);

//     const center = glm.vec3.fromValues(0,0,0);
//     // let totalHidden1_front = 0;
//     // let totalHidden2_back = 0;

//     for (const currAgent of allAgents) {

//       const currPos = currAgent.getPosition();

//       glm.vec3.add(center, center, currPos);

//       // if (!this._frustumCulling1.pointInFrustum(currPos[0], currPos[1], currPos[2])) {
//       //   totalHidden1_front += 1;
//       // }
//       // if (!this._frustumCulling2.pointInFrustum(currPos[0], currPos[1], currPos[2])) {
//       //   totalHidden2_back += 1;
//       // }
//     }

//     glm.vec3.scale(center, center, 1 / allAgents.length);



//     // console.log({totalHidden1_front, totalHidden2_back, distance: this._distance})

//     // if (totalHidden2_back > 0) {
//     //   this._distance += 0.1;
//     // }
//     // else if (totalHidden1_front > 0) {
//     //   this._distance = Math.max(5, this._distance - 0.01);
//     // }

//     /**
//      *
//      *    *     *     *
//      *   *     *     *
//      *  *     *     *
//      * <     <     <
//      *  *     *     *
//      *   *     *     *
//      *    *     *     *
//      *
//      */

//     const totalHiddenNear = this._cameraNear.getTotalHidden(allAgents);
//     const totalHiddenFar = this._cameraFar.getTotalHidden(allAgents);

//     if (totalHiddenFar > 0) {
//       this._distance += 0.2; // get farther, to see more
//     }
//     else if (totalHiddenNear === 0) {
//       this._distance -= 0.1; // get closer, to see better
//       this._distance = Math.max(3, this._distance); // clamp
//     }

//     glm.vec3.lerp(this._target, this._target, center, 0.01);
//     glm.vec3.copy(this._position, this._target);
//     glm.vec3.add(this._position, this._position, [-5,this._distance+5,this._distance+5]);

//     // use camera, frustum culling

//   }

//   getPosition(): glm.ReadonlyVec3 {
//     return this._position;
//   }

//   getTarget(): glm.ReadonlyVec3 {
//     return this._target;
//   }

//   getUpAxis(): glm.ReadonlyVec3 {
//     return this._upAxis;
//   }

// };
