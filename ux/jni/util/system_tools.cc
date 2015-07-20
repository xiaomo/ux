#include "util/system_tools.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>


// 返回的字符串需要外面释放
char* JString2Char(JNIEnv* env, jstring jstr)
{
    if (env == NULL || jstr == NULL) return NULL;
    char* localRet = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen >= 0)
    {
        localRet = (char*)malloc(alen + 1);
        memcpy(localRet, ba, alen);
        localRet[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    env->DeleteLocalRef(clsstring);
    env->DeleteLocalRef(barr);
    env->DeleteLocalRef(strencode);
    return localRet;
}

jstring Char2JString(JNIEnv* env, const char* cstr)
{
    if (env == NULL || cstr == NULL) return NULL;
    jsize str_len = strlen(cstr);
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(str_len);
    env->SetByteArrayRegion(bytes, 0, str_len, (jbyte*)cstr);
    jstring encoding = env->NewStringUTF("utf-8");
    jstring localRet = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
    env->DeleteLocalRef(strClass);
    env->DeleteLocalRef(bytes);
    env->DeleteLocalRef(encoding);
    return localRet;
}

jobject CreateHandle(JNIEnv* env, int val)
{
    jclass cls = env->FindClass("com/huamaitel/api/HMDefines$Handle");
    jmethodID initMethod = env->GetMethodID(cls, "<init>", "()V");
    jobject newObj = env->NewObject(cls, initMethod, NULL);
    jfieldID f = env->GetFieldID(cls, "handle", "I");
    env->SetIntField(newObj, f, val);
    env->DeleteLocalRef(cls);
    return newObj;
}

void SetHandle(JNIEnv* env, jobject jhandle, int chandle)
{
    if (0 == jhandle)
    {
        return;
    }
    jclass cls = env->FindClass("com/huamaitel/api/HMDefines$Handle");
    jfieldID f = env->GetFieldID(cls, "handle", "I");
    env->SetIntField(jhandle, f, chandle);
    env->DeleteLocalRef(cls);
}

int GetHandle(JNIEnv* env, jobject jhandle)
{
    if (0 == jhandle)
    {
        return 0;
    }
    jclass cls = env->FindClass("com/huamaitel/api/HMDefines$Handle");
    jfieldID f = env->GetFieldID(cls, "handle", "I");
    int chandle = env->GetIntField(jhandle, f);
    env->DeleteLocalRef(cls);
    return chandle;
}

// Throw an exception with the specified class and an optional message.
int jniThrowException(JNIEnv* env, const char* className, const char* msg)
{
    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass == NULL)
        return -1;

    if (env->ThrowNew(exceptionClass, msg) != JNI_OK) {
    }

    return 0;
}


//////////////////////////////////////////// File operation ////////////////////////////////////////

int file_open(const char *filename, int flags)
{
    int fd;

    fd = open(filename, flags, 0666);
    if (fd == -1)
        return -1;

    return fd;
}

int file_read(int fd, unsigned char *buf, int size)
{
    return read(fd, buf, size);
}

int file_write(int fd, const unsigned char *buf, int size)
{
    return write(fd, buf, size);
}

int64_t file_seek(int fd, int64_t pos, int whence)
{
    if (whence == 0x10000) {
        struct stat st;
        int ret = fstat(fd, &st);
        return ret < 0 ? -1 : st.st_size;
    }
    return lseek(fd, pos, whence);
}

int file_close(int fd)
{
    return close(fd);
}
