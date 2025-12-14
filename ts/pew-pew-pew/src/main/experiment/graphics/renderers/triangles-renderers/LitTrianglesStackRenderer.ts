
import { system, graphics } from '@local-framework';

// @ts-ignore
import stackRendererVertex from './shaders/stack-renderer.glsl.vert';
// @ts-ignore
import stackRendererFragment from './shaders/stack-renderer.glsl.frag';

import * as glm from 'gl-matrix';

export interface ILitTrianglesStackRenderer {
  pushTriangle(
    inPointA: glm.ReadonlyVec3,
    inPointB: glm.ReadonlyVec3,
    inPointC: glm.ReadonlyVec3,
    inColor: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    inNormal: glm.ReadonlyVec3,
  ): void;

  // safeRender(inCamera: graphics.camera.ICamera, inCallback: () => void): void;
  flush(inCamera: graphics.camera.ICamera): void;
  clear(): void;
}









const k_bufferSize = 30 * 1024 * 6; // 6 * 1000 triangles -> 180ko


class VertexBuffer {

  public buffer = new Float32Array(k_bufferSize);
  public currentSize: number = 0;

  pushTriangle(
    inPointA: glm.ReadonlyVec3,
    inPointB: glm.ReadonlyVec3,
    inPointC: glm.ReadonlyVec3,
    inColor: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    inNormal: glm.ReadonlyVec3,
  ) {
    if (this.currentSize + 10 * 3 >= this.buffer.length) {
      console.log("not enough buffer space for a triangle");
    }

    const alphaValue = inColor[3] ?? 1;

    // 0
    this.buffer[this.currentSize + 0] = inPointA[0];
    this.buffer[this.currentSize + 1] = inPointA[1];
    this.buffer[this.currentSize + 2] = inPointA[2];
    this.buffer[this.currentSize + 3] = inColor[0];
    this.buffer[this.currentSize + 4] = inColor[1];
    this.buffer[this.currentSize + 5] = inColor[2];
    this.buffer[this.currentSize + 6] = alphaValue;
    this.buffer[this.currentSize + 7] = inNormal[0];
    this.buffer[this.currentSize + 8] = inNormal[1];
    this.buffer[this.currentSize + 9] = inNormal[2];
    this.currentSize += 10;

    // 2
    this.buffer[this.currentSize + 0] = inPointB[0];
    this.buffer[this.currentSize + 1] = inPointB[1];
    this.buffer[this.currentSize + 2] = inPointB[2];
    this.buffer[this.currentSize + 3] = inColor[0];
    this.buffer[this.currentSize + 4] = inColor[1];
    this.buffer[this.currentSize + 5] = inColor[2];
    this.buffer[this.currentSize + 6] = alphaValue;
    this.buffer[this.currentSize + 7] = inNormal[0];
    this.buffer[this.currentSize + 8] = inNormal[1];
    this.buffer[this.currentSize + 9] = inNormal[2];
    this.currentSize += 10;

    // 3
    this.buffer[this.currentSize + 0] = inPointC[0];
    this.buffer[this.currentSize + 1] = inPointC[1];
    this.buffer[this.currentSize + 2] = inPointC[2];
    this.buffer[this.currentSize + 3] = inColor[0];
    this.buffer[this.currentSize + 4] = inColor[1];
    this.buffer[this.currentSize + 5] = inColor[2];
    this.buffer[this.currentSize + 6] = alphaValue;
    this.buffer[this.currentSize + 7] = inNormal[0];
    this.buffer[this.currentSize + 8] = inNormal[1];
    this.buffer[this.currentSize + 9] = inNormal[2];
    this.currentSize += 10;
  }

  reset() {
    this.currentSize = 0;
  }

};

export class LitTrianglesStackRenderer implements ILitTrianglesStackRenderer {

  private _shader: graphics.webgl2.IUnboundShader;
  private _geometry: graphics.webgl2.GeometryWrapper.Geometry;

  // private _buffer = new Float32Array(k_bufferSize);
  // private _currentSize: number = 0;

  private _bufferLight = new VertexBuffer();
  // private _bufferSorted = new VertexBuffer();

  constructor() {
    this._shader = new graphics.webgl2.ShaderProgram('LitTrianglesStackRenderer', {
      vertexSrc: stackRendererVertex,
      fragmentSrc: stackRendererFragment,
      attributes: ['a_vertex_position', 'a_vertex_color', 'a_vertex_normal'],
      uniforms: ['u_composedMatrix', 'u_lightPos']
    });

    const geoBuilder = new graphics.webgl2.GeometryWrapper.GeometryBuilder();
    geoBuilder
      .reset()
      .setPrimitiveType('triangles')
      .addVbo()
      .setVboAsDynamic()
      .addVboAttribute('a_vertex_position', 'vec3f')
      .addVboAttribute('a_vertex_color', 'vec4f')
      .addVboAttribute('a_vertex_normal', 'vec3f');

    this._geometry = new graphics.webgl2.GeometryWrapper.Geometry(this._shader, geoBuilder.getDef());
  }

  pushTriangle(
    inPointA: glm.ReadonlyVec3,
    inPointB: glm.ReadonlyVec3,
    inPointC: glm.ReadonlyVec3,
    inColor: glm.ReadonlyVec3 | glm.ReadonlyVec4,
    inNormal: glm.ReadonlyVec3,
  ) {
    this._bufferLight.pushTriangle(
      inPointA,
      inPointB,
      inPointC,
      inColor,
      inNormal,
    );
  }

  // pushSortedTriangle(
  //   inPointA: glm.ReadonlyVec3,
  //   inPointB: glm.ReadonlyVec3,
  //   inPointC: glm.ReadonlyVec3,
  //   inColor: glm.ReadonlyVec3 | glm.ReadonlyVec4,
  //   inNormal: glm.ReadonlyVec3,
  // ) {
  //   this._bufferSorted.pushTriangle(
  //     inPointA,
  //     inPointB,
  //     inPointC,
  //     inColor,
  //     inNormal,
  //   );
  // }

  flush(inCamera: graphics.camera.ICamera) {
    this._shader.bind((bound) => {
      bound.setMatrix4Uniform('u_composedMatrix', inCamera.getComposedMatrix());
      bound.setFloat3Uniform('u_lightPos', inCamera.getEye()[0], inCamera.getEye()[1], inCamera.getEye()[2]);

      this._flush();
    });
  }

  private _flush() {
    if (!this.canRender()) {
      return;
    }

    if (this._bufferLight.currentSize > 0) {

      this._geometry.allocateBuffer(0, this._bufferLight.buffer, this._bufferLight.currentSize);
      this._geometry.setPrimitiveCount(this._bufferLight.currentSize / 10);

      this._geometry.render();

      this._bufferLight.currentSize = 0;
    }

    // if (this._bufferSorted.currentSize > 0) {

    //   // sort here

    //   this._geometry.allocateBuffer(0, this._bufferSorted.buffer, this._bufferSorted.currentSize);
    //   this._geometry.setPrimitiveCount(this._bufferSorted.currentSize / 10);

    //   this._geometry.render();

    //   this._bufferSorted.currentSize = 0;
    // }

    this.clear();
  }

  canRender() {
    return (
      this._bufferLight.currentSize > 0 //||
      // this._bufferSorted.currentSize > ||
    );
  }

  // safeRender(inCamera: graphics.camera.ICamera, inCallback: () => void) {
  //   this._shader.bind((bound) => {
  //     bound.setMatrix4Uniform('u_composedMatrix', inCamera.getComposedMatrix());
  //     bound.setFloat3Uniform('u_lightPos', inCamera.getEye()[0], inCamera.getEye()[1], inCamera.getEye()[2]);

  //     inCallback();

  //     this._flush();
  //   });
  // }

  clear(): void {
    this._bufferLight.reset();
  }
}
