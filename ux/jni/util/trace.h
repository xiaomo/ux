#ifndef UX_UTIL_TRACE_HPP_
#define UX_UTIL_TRACE_HPP_
#ifdef _WIN32

#include <stdio.h>
#define LOGV(...) {printf(__VA_ARGS__);printf("\n");}
#define LOGD(...) {printf(__VA_ARGS__);printf("\n");}
#define LOGI(...) {printf(__VA_ARGS__);printf("\n");}
#define LOGW(...) {printf(__VA_ARGS__);printf("\n");}
#define LOGE(...) {printf(__VA_ARGS__);printf("\n");}
#else

#define TAG "ux-native"
#include <android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG ,TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  ,TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  ,TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR ,TAG, __VA_ARGS__)
#endif 

#endif /* UX_UTIL_TRACE_HPP_ */