
import { system } from '@local-framework';

import { Application } from './experiment/Application';

import * as utilities from './utilities';

const _queryDomElement = <T extends Element>(inName: string): T => {
  const newElement = document.querySelector<T>(inName);
  if (!newElement) {
    throw new Error(`html element "${inName}" not found`);
  }
  return newElement;
};

window.addEventListener('load', async () => {

  let demo: Application | null = null;

  //
  // HTML elements check
  //

  const canvasElement = _queryDomElement<HTMLCanvasElement>('#main-canvas');
  const errorText = _queryDomElement<HTMLParagraphElement>('#error-text')!;

  //
  // Error Handling
  //

  window.addEventListener('error', (err: WindowEventMap['error']) => {
    if (demo) {
      demo.stop();
    }

    // setup the error message
    errorText.style.width = '800px';
    errorText.style.height = '600px';
    errorText.innerHTML = err.message;

    // swap the canvas with the error message
    canvasElement.style.display = 'none';
    errorText.style.display = 'block';

    document.title += ' (ERR)';
  });

  //
  // resize
  //

  const resizeAll = (canvasElement: HTMLCanvasElement, demo: Application) => {
    canvasElement.width = window.innerWidth;
    canvasElement.height = window.innerHeight;
    canvasElement.style.width = `${window.innerWidth}px`;
    canvasElement.style.height = `${window.innerHeight}px`;
    demo.resize(window.innerWidth, window.innerHeight);
  }

  //
  // browser features check
  //

  if (!system.browser.isWebGL2Supported()) {
    throw new Error('missing WebGL2 feature (unsupported)');
  }

  //
  // setup application
  //

  demo = new Application(canvasElement);
  resizeAll(canvasElement, demo);

  await demo.init();
  demo.start();

  window.addEventListener('resize', () => {
    if (demo && canvasElement) {
      resizeAll(canvasElement, demo);
    }
  });

  const pageMaxTimeInvisible = 60 * 1000; // 60sec
  utilities.setupOutdatedPage(pageMaxTimeInvisible, () => {
    throw new Error(
      '<br/><br/><br/>The page was inactive for too long<br/><br/>please reload'
    );
  });
});
