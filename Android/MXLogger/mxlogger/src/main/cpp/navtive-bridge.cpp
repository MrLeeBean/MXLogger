//
// Created by 董家祎 on 2022/3/23.
//

#include <jni.h>
#include <string>
#include <cstdint>
#include "mxlogger_manager.hpp"
static JavaVM *g_currentJVM = nullptr;
static jclass g_cls = nullptr;

static int registerNativeMethods(JNIEnv *env, jclass cls);

extern "C" JNIEXPORT JNICALL jint  JNI_OnLoad(JavaVM *vm, void *reserved) {
   g_currentJVM = vm;
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    if (g_cls) {
        env->DeleteGlobalRef(g_cls);
    }
    static const char *clsName = "com/dongjiayi/mxlogger/MXLogger";

    jclass instance = env->FindClass(clsName);
    if (!instance) {

        return -2;
    }
    g_cls = reinterpret_cast<jclass>(env->NewGlobalRef(instance));
    if (!g_cls) {
        return -3;
    }
    int ret = registerNativeMethods(env, g_cls);
    if (ret != 0) {

        return -4;
    }
    return JNI_VERSION_1_6;
}

#define MXLOGGER_JNI static
namespace mxlogger{
    MXLOGGER_JNI  inline log_type _logType(jint logtype);
    MXLOGGER_JNI inline  level::level_enum _level(jint type);

    static jstring string2jstring(JNIEnv *env, const std::string &str) {
        return env->NewStringUTF(str.c_str());
    }
    static std::string jstring2string(JNIEnv *env, jstring str) {
        if (str) {
            const char *kstr = env->GetStringUTFChars(str, nullptr);
            if (kstr) {
                std:: string result(kstr);
                env->ReleaseStringUTFChars(str, kstr);
                return result;
            }
        }
        return "";
    }

    MXLOGGER_JNI jstring version(JNIEnv *env, jclass type){
        std::string v = "0.0.1";
        return string2jstring(env,v);
    }

    MXLOGGER_JNI void native_storagePolicy(JNIEnv *env, jobject obj,jstring policy){
        if (policy == nullptr) return;
        policy::storage_policy  storage_policy = policy::storage_policy::yyyy_MM_dd;

        const char * policy_str = jstring2string(env,policy).c_str();

        if (strcmp("yyyy_MM",policy_str) == 0){
            storage_policy = policy::storage_policy::yyyy_MM;
        }else if(strcmp("yyyy_MM_dd",policy_str)  == 0){
            storage_policy = policy::storage_policy::yyyy_MM_dd;
        }else if (strcmp("yyyy_ww",policy_str) == 0){
            storage_policy = policy::storage_policy::yyyy_ww;
        }else if (strcmp("yyyy_MM_dd_HH",policy_str) == 0){
            storage_policy =  policy::storage_policy::yyyy_MM_dd_HH;
        }
        mx_logger ::instance().set_file_policy(storage_policy);
    }
    /// 初始化MXLogger 日志目录
    MXLOGGER_JNI void jniInitialize(JNIEnv *env, jobject obj,jstring diskCachePath){
        if (diskCachePath == nullptr) return;

       std::string disk_cache_path =jstring2string(env,diskCachePath);

       mx_logger &logger = mx_logger ::instance();
        logger.set_file_dir(disk_cache_path);
    }
    MXLOGGER_JNI void log(JNIEnv *env, jobject obj,jint type,jstring name,jint level,jstring msg,jstring tag,jboolean mainThread){

       const char  * log_msg = msg == NULL ? nullptr :jstring2string(env,msg).c_str();

       const char  * log_tag = tag == NULL ? nullptr : jstring2string(env,tag).c_str();

       const char  * log_name = name == NULL ? nullptr : jstring2string(env,name).c_str();

        mx_logger::instance().log(_logType(type),_level(level),log_name,log_msg,log_tag,(bool)mainThread);
    }
    MXLOGGER_JNI inline log_type _logType(jint type){
        switch (type) {
            case 0:
                return log_type::all;
            case 1: 
                return log_type::console;
            case 2:
                return log_type::file;
            default:
                return log_type::all;
        }
    }
    MXLOGGER_JNI inline  level::level_enum _level(jint type){
        switch (type) {
            case 0:
                return level::level_enum::debug;
            case 1:
                return level::level_enum::info;
            case 2:
                return level::level_enum::warn;
            case 3:
                return level::level_enum::error;
            case 4:
                return level::level_enum::fatal;
            default:
                return level::level_enum::debug;
        }
    }

}
static JNINativeMethod g_methods[] = {
        {"version", "()Ljava/lang/String;", (void *) mxlogger::version},
        {"jniInitialize","(Ljava/lang/String;)V",(void *)mxlogger::jniInitialize},
        {"log","(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;Z)V",(void *)mxlogger::log},
        {"native_storagePolicy","(Ljava/lang/String;)V",(void *)mxlogger::native_storagePolicy}
};
static int registerNativeMethods(JNIEnv *env, jclass cls) {
    return env->RegisterNatives(cls, g_methods, sizeof(g_methods) / sizeof(g_methods[0]));
}
