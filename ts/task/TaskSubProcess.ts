
import * as child_process from "node:child_process"
// import * as fs from "node:fs"
// import * as path from "node:path"

//
//
//

// process.once('uncaughtException', () => {
// });

// interface ISubProcess {
//   promise: Promise<void>;
//   childProcess: child_process.ChildProcess;
// };

// const _asyncSpawn = (...args: Parameters<typeof child_process.spawn>): ISubProcess => {

//   const childProcess = child_process.spawn(...args);

//   const promise = new Promise<void>((resolve, reject) => {
//     childProcess.once('close', (code, signal) => {
//       if (code === 0 && signal === null) {
//         resolve();
//       } else {
//         reject();
//       }
//     });
//   });

//   return { promise, childProcess };
// };

// const _asyncRunCommand = (
//   fullCommand: [string, ...string[]],
//   options: child_process.SpawnOptionsWithoutStdio,
// ): ISubProcess => {

//   const [command, ...args] = fullCommand;

//   return _asyncSpawn(command, args, options);
// };

// const _asyncLoggedRunCommand = async (
//   opts: {
//     name: string,
//     fullCommand: [string, ...string[]],
//     options: child_process.SpawnOptionsWithoutStdio,
//     logPatterns?: RegExp[],
//   },
//   // fullCommand: [string, ...string[]],
//   // options: child_process.SpawnOptionsWithoutStdio,
// ): Promise<boolean> => {

//   let patternsLeft: RegExp[] = opts.logPatterns ? [...opts.logPatterns] : [];

//   // let patternResolve: undefined | (() => void);
//   // const patternPromise: Promise<void> = new Promise<void>((resolve) => {
//   //   patternResolve = resolve;
//   // });
//   // if (patternsLeft.length === 0) {
//   //   // no patterns -> end the promise now
//   //   patternResolve!();
//   // }

//   const _onLog = (data: any, checkPattern: boolean) => {
//     _ensureString(data)
//       .split('\n')
//       .filter(line => line.length > 0)
//       .forEach(line => {
//         console.log(`[${opts.name}]: ${line}`);

//         for (let ii = 0; ii < patternsLeft.length; ) {
//           if (patternsLeft[ii]!.test(line)) {
//             patternsLeft.splice(ii, 1);

//             console.log(`[${opts.name}]: {ONE PATTERN MATCHED}`);

//             if (patternsLeft.length === 0) {
//               console.log(`[${opts.name}]: {ALL PATTERNS MATCHED}`);
//               // patternResolve!();
//             }

//           } else {
//             ++ii;
//           }
//         }

//       });
//   };

//   try {
//     _onLog("start", false)

//     const subProcess = _asyncRunCommand(opts.fullCommand, opts.options);

//     subProcess.childProcess.stdout?.on('data', (data) => _onLog(data, true));
//     subProcess.childProcess.stderr?.on('data', (data) => _onLog(data, true));

//     await subProcess.promise;
//     // await patternPromise

//     if (patternsLeft.length > 0) {
//       throw new Error("not all patterns matched");
//     }

//     _onLog("stop", false);
//     return true;
//   }
//   catch (err: any) {
//     _onLog(`stop.err, msg="${err.message}"`, false);
//     // console.log(err.message)
//     return false;
//   }
// };

//
//
//

const _ensureString = (data: Buffer | string): string => {

  if (typeof(data) === 'string') {
    return data;
  }
  return data.toString('utf8')
}

const _getLines = (data: Buffer | string): string[] => {
  return _ensureString(data)
    .split('\n')
    .filter(line => line.length > 0);
}

//
//
//

export interface ITaskSubProcessDef {
  name: string,
  fullCommand: [string, ...string[]],
  options: child_process.SpawnOptionsWithoutStdio,
  logPatterns?: RegExp[],
};

export interface ISubProcess {
  promise: Promise<void>;
  childProcess: child_process.ChildProcess;
};

export class TaskSubProcess {

  private _def: ITaskSubProcessDef;

  private _subProcess?: ISubProcess;
  private _status: 'SETUP' | 'STARTED' | 'DONE' | 'TERMINATED' = 'SETUP';

  private _patternPromise?: Promise<void>;
  private _patternResolve?: () => void;
  private _patternReject?: (err: Error) => void;

  private constructor(def: ITaskSubProcessDef) {
    this._def = def;
  }

  private _launchProcess(
    fullCommand: [string, ...string[]],
    options: child_process.SpawnOptionsWithoutStdio,
  ) {
    console.log(`[${this._def.name}][---]: {START}`);
    // this._subProcess = _asyncRunCommand(fullCommand, options);

    const [command, ...args] = fullCommand;
    const childProcess = child_process.spawn(command, args, options);

    this._status = 'STARTED';

    const promise = new Promise<void>((resolve, reject) => {
      childProcess.once('close', (code, signal) => {
        if (code === 0 && signal === null || this._status === 'TERMINATED') {
          resolve();
        } else {
          reject();
        }
      });
    });

    this._subProcess = { promise, childProcess };

    this._subProcess.childProcess.stdout?.on('data', (data) => {
      _getLines(data).forEach(line => {
        console.log(`[${this._def.name}][OUT]: ${line}`);
      });
    });

    this._subProcess.childProcess.stderr?.on('data', (data) => {
      _getLines(data).forEach(line => {
        console.log(`[${this._def.name}][ERR]: ${line}`);
      });
    });

    this._subProcess.promise.then(() => {
      console.log(`[${this._def.name}][---]: {STOP}`);
      this._status = 'DONE';
    });
    this._subProcess.promise.catch((err) => {
      console.log(`[${this._def.name}][---]: {STOP.ERR="${err.message}"}`);
      this._status = 'DONE';
    });
  }

  private _handlePatterns(logPatterns?: RegExp[]): void {

    if (!logPatterns || logPatterns.length === 0) {
      return;
    }

    let patternsLeft: RegExp[] = logPatterns ? [...logPatterns] : [];

    this._patternPromise = new Promise<void>((resolve, reject) => {
      this._patternResolve = resolve;
      this._patternReject = reject;
    });

    this._subProcess?.promise.then(() => {
      if (patternsLeft.length > 0 && this._patternReject) {
        this._patternReject(new Error(`Process ended with ${patternsLeft.length} pattern(s) left.`));
      }
    });

    const _onLog = (data: Buffer | string) => {
      _getLines(data).forEach(line => {

        for (let ii = 0; ii < patternsLeft.length; ) {
          if (patternsLeft[ii]!.test(line)) {
            patternsLeft.splice(ii, 1);

            console.log(`[${this._def.name}][---]: {ONE PATTERN MATCHED}`);

            if (patternsLeft.length === 0) {
              console.log(`[${this._def.name}][---]: {ALL PATTERNS MATCHED}`);

              if (this._patternResolve) {
                this._patternResolve();
              }
            }

          } else {
            ++ii;
          }
        }

      });
    };

    this._subProcess!.childProcess.stdout?.on('data', _onLog);
    this._subProcess!.childProcess.stderr?.on('data', _onLog);
  }

  static create(def: ITaskSubProcessDef): TaskSubProcess {
    const newTask = new TaskSubProcess(def);
    // newTask._launchProcess(def.fullCommand, def.options);
    // newTask._handlePatterns(def.logPatterns);
    return newTask;
  }

  launch(): this {
    if (this._status !== 'SETUP') {
      throw new Error(`status for ${this._def.name} is ${this._status}`);
    }
    this._launchProcess(this._def.fullCommand, this._def.options);
    this._handlePatterns(this._def.logPatterns);
    return this;
  }

  terminate(): this {
    if (
      this._subProcess &&
      this._status !== 'TERMINATED'
    ) {
      this._status = 'TERMINATED';
      this._subProcess.childProcess.kill('SIGINT');
    }
    return this;
  }

  subProcessPromise(): Promise<void> {
    if (this._subProcess) {
      return this._subProcess?.promise
    }
    return Promise.resolve();
  }

  logPatternsPromise(): Promise<void> {
    if (this._patternPromise) {
      return this._patternPromise
    }
    return Promise.resolve();
  }

};


//
//
//
