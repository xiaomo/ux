#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <stdlib.h>
#include <jni.h>
#include "util/trace.h"

//#define max(x,y)  ( x>y?x:y )
//#define min(x,y)  ( x>y?y:x )

/********************************   JNI 字符操作    ************************************************/

char* JString2Char(JNIEnv* env, jstring jstr);
jstring Char2JString(JNIEnv* env, const char* cstr);
jobject CreateHandle(JNIEnv* env, int val = 0);
void SetHandle(JNIEnv* env, jobject jhandle, int chandle);
int GetHandle(JNIEnv* env, jobject jhandle);
int jniThrowException(JNIEnv* env, const char* className, const char* msg);

/********************************   文件读写操作  ************************************************/

int file_open(const char *filename, int flags);
int file_read(int fd, unsigned char *buf, int size);
int file_write(int fd, const unsigned char *buf, int size);
int64_t file_seek(int fd, int64_t pos, int whence);
int file_close(int fd);

#endif //__TOOLS_H__
