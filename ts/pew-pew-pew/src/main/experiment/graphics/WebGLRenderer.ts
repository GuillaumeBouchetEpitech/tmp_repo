
import { system, graphics } from '@local-framework';

import * as glm from 'gl-matrix';

import { ILitTrianglesStackRenderer, LitTrianglesStackRenderer } from './renderers/triangles-renderers/LitTrianglesStackRenderer';

//

interface IDefinition {
  canvasDomElement: HTMLCanvasElement;
}

export class WebGLRenderer {
  private _def: IDefinition;

  private _viewportSize: glm.vec2;

  private _frustumCulling: graphics.camera.FrustumCulling;

  private _mainCamera = new graphics.camera.Camera();
  private _mainHudCamera = new graphics.camera.Camera();

  private onContextLost: (() => void) | null = null;
  private onContextRestored: (() => void) | null = null;

  private _textRenderer: graphics.renderers.TextRenderer;
  private _stackRenderers: graphics.renderers.StackRenderers;
  private _litTrianglesStackRenderer: LitTrianglesStackRenderer;
  private _geometryStackRenderer: graphics.renderers.GeometryRenderer;

  constructor(def: IDefinition) {
    this._def = def;

    this._viewportSize = [
      this._def.canvasDomElement.width,
      this._def.canvasDomElement.height
    ];

    this.resize(this._viewportSize[0], this._viewportSize[1]);

    graphics.webgl2.WebGLContext.initialize(this._def.canvasDomElement);

    this._def.canvasDomElement.addEventListener(
      'webglcontextlost',
      (event: Event) => {
        event.preventDefault();
        console.log('context is lost');

        if (this.onContextLost) this.onContextLost();
      },
      false
    );

    this._def.canvasDomElement.addEventListener(
      'webglcontextrestored',
      () => {
        console.log('context is restored');

        graphics.webgl2.WebGLContext.initialize(this._def.canvasDomElement);

        if (this.onContextRestored) this.onContextRestored();
      },
      false
    );

    this._frustumCulling = new graphics.camera.FrustumCulling();

    this._textRenderer = new graphics.renderers.TextRenderer();
    this._stackRenderers = new graphics.renderers.StackRenderers();
    this._litTrianglesStackRenderer = new LitTrianglesStackRenderer();

    this._geometryStackRenderer = new graphics.renderers.GeometryRenderer();
    const geoVertices = graphics.geometries.makeBox([2,2,5]);
    this._geometryStackRenderer.setGeometryVertices(geoVertices);
  }

  //

  async initialize() {
    const gl = graphics.webgl2.WebGLContext.getContext();

    //
    //
    // init

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clearDepth(1.0);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LESS);

    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_COLOR);

    gl.disable(gl.CULL_FACE);
  }

  resize(width: number, height: number) {
    this._viewportSize[0] = width;
    this._viewportSize[1] = height;

    this._mainCamera.setViewportSize(width, height);
    this._mainCamera.setAsPerspective({ fovy: 70, near: 0.1, far: 200 });
    this._mainCamera.computeMatrices();

    this._mainHudCamera.setViewportSize(width, height);
    this._mainHudCamera.setAsOrthogonal({
      left: -width * 0.5,
      right: +width * 0.5,
      top: -height * 0.5,
      bottom: +height * 0.5,
      near: -200,
      far: 200
    });
    this._mainHudCamera.setEye([+width * 0.5, +height * 0.5, 1]);
    this._mainHudCamera.setTarget([+width * 0.5, +height * 0.5, 0]);
    this._mainHudCamera.setUpAxis([0, 1, 0]);
    this._mainHudCamera.computeMatrices();
  }

  //

  toggleContextLoss() {
    const gl = graphics.webgl2.WebGLContext.getContext();
    const extensionLoseContext = graphics.webgl2.WebGLContext.getExtensionLoseContext();

    if (extensionLoseContext) {
      if (gl.isContextLost()) {
        extensionLoseContext.restoreContext(); // restores the context
      } else {
        extensionLoseContext.loseContext(); // trigger a context loss
      }
    }
  }

  contextIsLost() {
    const gl = graphics.webgl2.WebGLContext.getContext();

    return gl.isContextLost();
  }

  setOnContextLost(callback: () => void) {
    this.onContextLost = callback;
  }

  setOnContextRestored(callback: () => void) {
    this.onContextRestored = callback;
  }

  //

  lookAt(
    inEye: glm.ReadonlyVec3,
    inTarget: glm.ReadonlyVec3,
    inUpAxis: glm.ReadonlyVec3
  ) {
    this._mainCamera.lookAt(inEye, inTarget, inUpAxis);
    this._mainCamera.computeMatrices();
    this._frustumCulling.calculateFrustum(
      this._mainCamera.getProjectionMatrix(),
      this._mainCamera.getViewMatrix()
    );
  }

  update() {
    this._mainCamera.computeComposedMatrix();
    this._mainHudCamera.computeComposedMatrix();

    this._frustumCulling.calculateFrustum(
      this._mainCamera.getProjectionMatrix(),
      this._mainCamera.getViewMatrix()
    );
  }

  renderScene(callback: (camera: graphics.camera.ICamera, frustumCulling: graphics.camera.IFrustumCulling) => void) {
    const gl = graphics.webgl2.WebGLContext.getContext();
    const [width, height] = this._viewportSize;

    gl.viewport(0, 0, width, height);

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.enable(gl.CULL_FACE);

    gl.enable(gl.BLEND);
    // gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_DST_ALPHA);
    // gl.blendFunc(gl.ONE, gl.ONE_MINUS_SRC_ALPHA);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

    //
    //
    //

    callback(this._mainCamera, this._frustumCulling);

    //
    //
    //

    // gl.enable(gl.BLEND);
    // gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_DST_ALPHA);

    this.flushScene();
  }

  flushScene() {
    this._litTrianglesStackRenderer.flush(this._mainCamera);
    this._stackRenderers.flush(this._mainCamera.getComposedMatrix());
    this._geometryStackRenderer.flush(this._mainCamera);
  }

  renderHUD(callback: () => void) {
    const [width, height] = this._viewportSize;

    const gl = graphics.webgl2.WebGLContext.getContext();
    gl.viewport(0, 0, width, height);
    gl.clear(gl.DEPTH_BUFFER_BIT);
    gl.disable(gl.CULL_FACE);

    const hudProjectionMatrix = glm.mat4.create();
    glm.mat4.ortho(
      hudProjectionMatrix,
      -width * 0.5,
      +width * 0.5,
      -height * 0.5,
      +height * 0.5,
      -200,
      200
    );

    const hudViewMatrix = glm.mat4.create();
    glm.mat4.lookAt(
      hudViewMatrix,
      [+width * 0.5, +height * 0.5, 1],
      [+width * 0.5, +height * 0.5, 0],
      [0, 1, 0]
    );

    const hudComposedMatrix = glm.mat4.create();
    glm.mat4.multiply(hudComposedMatrix, hudProjectionMatrix, hudViewMatrix);

    gl.clear(gl.DEPTH_BUFFER_BIT);
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_DST_ALPHA);

    callback();

    this._stackRenderers.flush(hudComposedMatrix);
    this._textRenderer.flush(hudComposedMatrix);

    gl.disable(gl.BLEND);

    graphics.webgl2.ShaderProgram.unbind();
  }

  get size(): glm.ReadonlyVec2 {
    return this._viewportSize;
  }
  get mainCamera(): Readonly<graphics.camera.ICamera> {
    return this._mainCamera;
  }
  get hudCamera(): Readonly<graphics.camera.ICamera> {
    return this._mainHudCamera;
  }
  get stackRenderers(): graphics.renderers.IStackRenderers {
    return this._stackRenderers;
  }
  get litTrianglesStackRenderer(): ILitTrianglesStackRenderer {
    return this._litTrianglesStackRenderer;
  }
  get textRenderer(): graphics.renderers.ITextRenderer {
    return this._textRenderer;
  }
  get frustumCulling(): graphics.camera.IFrustumCulling {
    return this._frustumCulling;
  }
  get geometryStackRenderer(): graphics.renderers.IGeometryRenderer {
    return this._geometryStackRenderer;
  }
}
