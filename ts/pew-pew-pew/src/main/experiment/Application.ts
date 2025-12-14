import * as configuration from '../configuration';

import { system, graphics } from '@local-framework';

import { WebGLRenderer } from './graphics/WebGLRenderer';

import { Simulation } from './logics/Simulation';

import { FrameProfiler } from './utilities/FrameProfiler';

import * as glm from 'gl-matrix';

export class Application {
  private _canvasElement: HTMLCanvasElement;

  private _renderer: WebGLRenderer;

  private _freeFlyController: system.controllers.FreeFlyController;

  private _running: boolean;
  private _errorGraphicContext: boolean;

  private _simulation: Simulation;

  private _isPaused: boolean = false;
  private _debugMode: number = 0;

  private _currFrameTime: number = Date.now();
  private _frameProfiler = new FrameProfiler();

  private _time: number = 0;

  constructor(canvasElement: HTMLCanvasElement) {

    this._canvasElement = canvasElement;

    this._renderer = new WebGLRenderer({
      canvasDomElement: canvasElement
    });

    this._freeFlyController = new system.controllers.FreeFlyController({
      position: glm.vec3.fromValues(-20, 20, 20),
      coordinates: ['X', 'Y', 'Z'],
      theta: Math.PI * -0.25,
      phi: Math.PI * -0.2,
      mouseSensibility: configuration.controllerMouseSensibility,
      movingSpeed: configuration.controllerMovingSpeed,
      keyboardSensibility: configuration.controllerKeyboardSensibility,
      touchSensibility: configuration.controllerTouchSensibility
    });

    //
    //

    {
      system.browser.GlobalKeyboardManager.activate();
      system.browser.GlobalTouchManager.activate(this._canvasElement);

      system.browser.GlobalPointerLockManager.allowPointerLockedOnClickEvent(canvasElement);
      system.browser.GlobalPointerLockManager.addOnLockChange(() => {
        const isLocked =
          system.browser.GlobalPointerLockManager.isPointerLocked(canvasElement);

        if (isLocked) {
          // g_logger.log('The pointer lock status is now locked');

          system.browser.GlobalMouseManager.activate();
        } else {
          // g_logger.log('The pointer lock status is now unlocked');

          system.browser.GlobalMouseManager.deactivate();

          system.browser.GlobalPointerLockManager.allowPointerLockedOnClickEvent(
            canvasElement
          );
        }
      });

      system.browser.GlobalPointerLockManager.addOnLockError((event) => {
        // g_logger.log(
        //   `The pointer lock sent an error, event: "${JSON.stringify(event)}"`
        // );
      });
    }

    this._running = false;
    this._errorGraphicContext = false;

    this._renderer.setOnContextLost(() => {
      console.log('on_context_lost');

      this._errorGraphicContext = true;
      this.stop();
    });

    this._renderer.setOnContextRestored(() => {
      console.log('on_context_restored');

      this._errorGraphicContext = false;
      this.start();
    });

    //
    //
    //

    this._simulation = new Simulation();

  }

  async init() {
    await this._renderer.initialize();
  }

  start() {
    if (this.isRunning()) {
      return;
    }

    this._running = true;

    this._tick();
  }

  stop() {
    this._running = false;
  }

  isRunning() {
    return this._running && !this._errorGraphicContext;
  }

  resize(width: number, height: number) {
    this._renderer.resize(width, height);
  }

  //
  //
  //

  private _tick() {
    const tick = () => {
      if (!this._running || this._errorGraphicContext) return;

      this._mainLoop();

      // plan the next frame
      window.requestAnimationFrame(tick);
    };

    tick();
  }

  private _mainLoop() {
    const currentTime = Date.now();
    let deltaSecTime = Math.max(0, currentTime - this._currFrameTime);
    this._currFrameTime = currentTime;
    this._frameProfiler.pushDelta(deltaSecTime);

    const safeDelta = Math.min(deltaSecTime / 1000, 1/60); // max frame: 60ps

    this._freeFlyController.update(safeDelta);

    system.browser.GlobalMouseManager.resetDeltas();
    system.browser.GlobalTouchManager.resetDeltas();

    this._time += safeDelta;

    if (system.browser.GlobalKeyboardManager.wasPressed('H')) {
      this._isPaused = !this._isPaused;
    }
    if (system.browser.GlobalKeyboardManager.wasPressed('J')) {
      this._debugMode = (this._debugMode + 1) % 4;
    }

    system.browser.GlobalKeyboardManager.resetWasPressedKeys();



    this._renderer.lookAt(
      this._freeFlyController.getPosition(),
      this._freeFlyController.getTarget(),
      this._freeFlyController.getUpAxis()
    );



    if (!this._isPaused) {
      this._simulation.update(safeDelta, this._renderer);
    }


    this._renderer.update();

    //
    //
    ////// render 3d scene

    this._renderer.renderScene((camera: graphics.camera.ICamera, frustumCulling: graphics.camera.IFrustumCulling) => {



      // // scene "origin" axis
      // this._renderer.stackRenderers.clear();
      // this._renderer.stackRenderers.pushLine([0,0,0], [10,0,0], [1,0,0]);
      // this._renderer.stackRenderers.pushLine([0,0,0], [0,10,0], [0,1,0]);
      // this._renderer.stackRenderers.pushLine([0,0,0], [0,0,10], [0,0,1]);


      // render references wireframe
      {
        // render the horizontal "reference grid"
        const color: glm.ReadonlyVec3 = [0.25, 0.25, 0.25];
        for (let ii = -5; ii <= 5; ++ii) {
          const tmpVal = ii * 10;
          this._renderer.stackRenderers.pushLine([tmpVal, -50, -1], [tmpVal, +50, -1], color);
          this._renderer.stackRenderers.pushLine([-50, tmpVal, -1], [+50, tmpVal, -1], color);
        }

        // render the vertical entities "altitude line", as if "linked" to the "reference grid"
        for (const currAgent of this._simulation.getAllAgents()) {
          this._renderer.stackRenderers.pushLine(currAgent.getPosition(), [currAgent.getPosition()[0], currAgent.getPosition()[1], -1], color);
        }
      }

      this._simulation.render(this._renderer, this._debugMode);
    });

    //
    //
    ////// HUD

    this._renderer.renderHUD(() => {

      const simStats = this._simulation.getStats();

      this._renderer.textRenderer
        .clear()
        .pushText([
          `totalEntities: ${simStats.totalEntities}`,
          `totalVisible: ${simStats.totalVisible} (${(simStats.totalVisible/simStats.totalEntities*100).toFixed(1)}%)`,
        ].join('\n'), [20,20]);

      // {
      //   const keyEventsPos: glm.ReadonlyVec2 = [7 + 20, 165];
      //   const touchEventsPos: glm.ReadonlyVec2 = [7 + 20, 260];
      //   const boardPos: glm.ReadonlyVec2 = [7, 35];

      //   graphics.renderers.widgets.addKeyStrokesWidgets(
      //     keyEventsPos,
      //     this._renderer.stackRenderers,
      //     this._renderer.textRenderer
      //   );
      //   graphics.renderers.widgets.addArrowStrokesWidgets(
      //     touchEventsPos,
      //     this._renderer.stackRenderers,
      //     this._renderer.textRenderer
      //   );
      //   graphics.renderers.widgets.addKeysTouchesWidgets(
      //     this._canvasElement,
      //     boardPos,
      //     this._renderer.stackRenderers,
      //     this._renderer.textRenderer
      //   );
      // }

      graphics.renderers.widgets.renderFpsMeter(
        [10, this._canvasElement.height - 60, 0],
        [100, 50],
        this._frameProfiler,
        this._renderer.stackRenderers,
        this._renderer.textRenderer,
        true
      );
    });
  }
}
