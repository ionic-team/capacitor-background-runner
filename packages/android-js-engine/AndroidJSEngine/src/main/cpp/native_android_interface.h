#ifndef ANDROID_JS_ENGINE_NATIVE_ANDROID_INTERFACE_H
#define ANDROID_JS_ENGINE_NATIVE_ANDROID_INTERFACE_H

#include <jni.h>
#include "./js-engine/src/native.hpp"
#include "java.h"

class NativeAndroidInterface : public NativeInterface {
public:
    NativeAndroidInterface(JNIEnv *env);
    ~NativeAndroidInterface();

    virtual void logger(LoggerLevel level, const std::string& tag, const std::string& messages);
    virtual void register_native_function(const std::string& func_name, std::any func_obj);
    virtual bool has_native_function(const std::string& func_name);
    virtual JSValue invoke_native_function(const std::string& func_name, JSContext* ctx, JSValue args);
    virtual std::string crypto_get_random_uuid();
    virtual std::vector<uint8_t> crypto_get_random(size_t size);
    virtual int get_random_hash();
    virtual NativeResponse fetch(NativeRequest request);
    virtual std::string byte_array_to_str(uint8_t* arr, size_t size, const std::string& encoding);
    virtual std::vector<uint8_t> string_to_byte_array(std::string str);

    std::unordered_map<std::string, jobject> registered_functions;

private:
    Java *java;
    jobject native_request_to_native_js_fetch_options(JNIEnv *env, NativeRequest request);
    NativeResponse native_js_response_to_native_response(JNIEnv *env, jobject native_js_response);
};


#endif //ANDROID_JS_ENGINE_NATIVE_ANDROID_INTERFACE_H
