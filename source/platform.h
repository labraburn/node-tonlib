//
// Created by Anton Spivak.
//

#pragma once

#if defined(__APPLE__)
#include <TargetConditionals.h>
#if defined(__arm64__)
#define __libtonlibjson "./node_modules/node-tonlib/libtonlibjson/darwin/arm64/libtonlibjson.dylib"
#else
#define __libtonlibjson "./node_modules/node-tonlib/libtonlibjson/darwin/x86-64/libtonlibjson.dylib"
#endif
#elif defined(__linux__)
#if defined(__arm__)
#define __libtonlibjson "./node_modules/node-tonlib/libtonlibjson/linux/aarch64/libtonlibjson.so"
#else
#define __libtonlibjson "./node_modules/node-tonlib/libtonlibjson/linux/x86-64/libtonlibjson.so"
#endif
#else
// TODO: Add windows support
#error "Unsupported platfrom"
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>
#define __dlfcn_type void *
#define __dlfcn_dlopen(libname) dlopen((libname), RTLD_LAZY)
#define __dlfcn_dlsym(lib, symbol) dlsym((lib), (symbol))
#else
// TODO: Add windows support
// #define __dlfcn_type HINSTANCE
// #define __dlfcn_dlopen(libname) LoadLibraryW(L##libname)
// #define __dlfcn_dlsym(lib, symbol) GetProcAddress((lib), (symbol))
#endif