//
// Created by Anton Spivak.
//

#include "tonlib.h"
#include "platform.h"

#include <iostream>
#include <napi.h>

static __dlfcn_type libtonlibjson;

typedef void *(*tonlib_client_json_create_t)();
static tonlib_client_json_create_t tonlib_client_json_create;

typedef void (*tonlib_client_json_destroy_t)(void *);
static tonlib_client_json_destroy_t tonlib_client_json_destroy;

typedef void (*tonlib_client_set_verbosity_level_t)(int);
static tonlib_client_set_verbosity_level_t tonlib_client_set_verbosity_level;

typedef void (*tonlib_client_json_send_t)(void *, const char *);
static tonlib_client_json_send_t tonlib_client_json_send;

typedef const char *(*tonlib_client_json_execute_t)(void *, const char *);
static tonlib_client_json_execute_t tonlib_client_json_execute;

typedef const char *(*tonlib_client_json_receive_t)(void *, double);
static tonlib_client_json_receive_t tonlib_client_json_receive;

Napi::Object Tonlib::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);

    // Load libtonlibjson depended on platform
    // Catch error only for library not for symbols

    libtonlibjson = __dlfcn_dlopen(__libtonlibjson);
    if (!libtonlibjson)
    {
        Napi::TypeError::New(env, "Can't load libtonlibjson for current platfrom").ThrowAsJavaScriptException();
        return Napi::Object(env, env.Null());
    }

    tonlib_client_json_create = (tonlib_client_json_create_t)__dlfcn_dlsym(libtonlibjson, "tonlib_client_json_create");
    tonlib_client_json_destroy = (tonlib_client_json_destroy_t)__dlfcn_dlsym(libtonlibjson, "tonlib_client_json_destroy");
    tonlib_client_set_verbosity_level = (tonlib_client_set_verbosity_level_t)__dlfcn_dlsym(libtonlibjson, "tonlib_client_set_verbosity_level");
    tonlib_client_json_send = (tonlib_client_json_send_t)__dlfcn_dlsym(libtonlibjson, "tonlib_client_json_send");
    tonlib_client_json_execute = (tonlib_client_json_execute_t)__dlfcn_dlsym(libtonlibjson, "tonlib_client_json_execute");
    tonlib_client_json_receive = (tonlib_client_json_receive_t)__dlfcn_dlsym(libtonlibjson, "tonlib_client_json_receive");

    // Load module

    Napi::Function constructor_function = DefineClass(env, "Tonlib", {
                                                                         InstanceMethod<&Tonlib::send>("send", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
                                                                         InstanceMethod<&Tonlib::execute>("execute", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
                                                                     });
    Napi::FunctionReference *constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(constructor_function);
    exports.Set("Tonlib", constructor_function);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

Tonlib::Tonlib(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Tonlib>(info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Number value = info[0].As<Napi::Number>();
    tonlib_client_set_verbosity_level(value.Int32Value());

    Napi::Function callback = info[1].As<Napi::Function>();

    this->client_ = tonlib_client_json_create();
    this->callback_ref_ = Napi::ThreadSafeFunction::New(env, callback, "callback", 0, 1);

    this->loop_ = std::thread(&Tonlib::make_loop, this);
    this->loop_.detach();
}

Tonlib::~Tonlib()
{
    tonlib_client_json_destroy(this->client_);
}

/*
 * Working thread
 */

void Tonlib::make_loop()
{
    while (true)
    {
        this->make_loop_iteration();
    }
}

void Tonlib::make_loop_iteration()
{
    const char *result = tonlib_client_json_receive(this->client_, 10);
    if (result == NULL || result[0] == '\0')
    {
        return;
    }

    std::string str(result);
    auto callback = [str](Napi::Env env, Napi::Function jsCallback)
    {
        Napi::String value = Napi::String::New(env, str);
        jsCallback.Call({value});
    };

    this->callback_ref_.NonBlockingCall(callback);
}

/*
 * JavaScript API
 */

void Tonlib::send(const Napi::CallbackInfo &info)
{
    Napi::String value = info[0].As<Napi::String>();
    tonlib_client_json_send(this->client_, value.Utf8Value().c_str());
}

Napi::Value Tonlib::execute(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::String value = info[0].As<Napi::String>();

    const char *result = tonlib_client_json_execute(this->client_, value.Utf8Value().c_str());
    std::string str(result);

    return Napi::String::New(env, str);
}