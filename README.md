welcome to ioc. ioc(information oritend compiler) is a compiler support ECMAScript.It can work in two modes, compile and JIT.So the first thing the IOC needs to do is to compiler the js to C.

This is so different from the V8, V8 only support JIT mode,This means that V8 require a minimum run-time kernel.the kernel includes GC and necessary basic built-in library.

IOC generate the corresponding pure C code, without having to depend on any third-party libraries(include ioc self).The biggest problem is the memory management,so ioc must analyze applications to determine each dominator of the flowchart graph.

This leads to the IOC has lost the dynamic characteristics of ECMAScript, the most important is eval. eval in IOC is more like #include in c or import in java.However, all other features in js,such as the prototype chain, flexible function variables will be retained.Especially function variables, very suitable for develop highly parallel applications.This mean we write a single-threaded program, IOC will generate code based on parallel environment (for example opencl).

Not so much the IOC is a compiler, as it is a SAA(Static Application Analysis).Merely in order to support the RAD, the IOC use LLVM JIT engine to act similar v8.

In structure, IOC attempt to allow dynamic extension the library usage analysis.So we can switch backend library automatically.
