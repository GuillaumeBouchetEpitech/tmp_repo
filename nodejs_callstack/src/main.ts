
const getStack = () => {

    // hacky replace
    const orig = Error.prepareStackTrace;
    Error.prepareStackTrace = (_, stack) => stack;

    // get stack
    const err = new Error();
    Error.captureStackTrace(err, undefined);
    const { stack } = err;

    // hacky reset
    Error.prepareStackTrace = orig;

    const actualStack = stack as unknown as NodeJS.CallSite[];

    return actualStack;
}


const sayHello1 = () => {
    console.log("HELLO!");

    const stack = getStack();
    for (let ii = 0; ii < stack.length; ++ii) {
        console.log(` => [${ii}]`);
        console.log(` ---> getLineNumber:   ${stack[ii].getLineNumber()}`);
        console.log(` ---> getFunctionName: ${stack[ii].getFunctionName()}`);
        console.log(` ---> getFileName:     ${stack[ii].getFileName()}`);
    }
};

const sayHello2 = () => sayHello1();
const sayHello3 = () => sayHello2();
const sayHello4 = () => sayHello3();
const sayHello5 = () => sayHello4();

console.log("start");

sayHello5();

console.log("stop");
