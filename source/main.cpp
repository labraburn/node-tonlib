//
// Created by Anton Spivak.
//

#include <napi.h>
#include "tonlib.h"

Napi::Object MODULE(Napi::Env env, Napi::Object exports)
{
    return Tonlib::Init(env, exports);
}

NODE_API_MODULE(libtonlibjson, MODULE);