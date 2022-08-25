//
// Created by Anton Spivak.
//

#include <napi.h>
#include <thread>

class Tonlib : public Napi::ObjectWrap<Tonlib>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Tonlib(const Napi::CallbackInfo &info);
    ~Tonlib();

private:
    void *client_;
    std::thread loop_;
    Napi::ThreadSafeFunction callback_ref_;

    void send(const Napi::CallbackInfo &info);
    Napi::Value execute(const Napi::CallbackInfo &info);

    void make_loop();
    void make_loop_iteration();
};