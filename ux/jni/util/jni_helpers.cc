#include "util/jni_helpers.h"

//#include "media_engine/media_engine.h"
#include <limits>

//#include "third_party/icu/source/common/unicode/unistr.h"

//using icu::UnicodeString;

jmethodID GetMethodID(JNIEnv* jni, jclass c, const std::string& name,
                      const char* signature) {
  jmethodID m = jni->GetMethodID(c, name.c_str(), signature);
  CHECK_EXCEPTION(jni, "error during GetMethodID");
  return m;
}

jlong jlongFromPointer(void* ptr) {
  CHECK(sizeof(intptr_t) <= sizeof(jlong), "Time to rethink the use of jlongs");
  // Going through intptr_t to be obvious about the definedness of the
  // conversion from pointer to integral type.  intptr_t to jlong is a standard
  // widening by the COMPILE_ASSERT above.
  jlong ret = reinterpret_cast<intptr_t>(ptr);
  CHECK(reinterpret_cast<void*>(ret) == ptr,
        "jlong does not convert back to pointer");
  return ret;
}

// Given a (UTF-16) jstring return a new UTF-8 native string.
std::string JavaToStdString(JNIEnv* jni, const jstring& j_string) {
  const jchar* jchars = jni->GetStringChars(j_string, NULL);
  CHECK_EXCEPTION(jni, "Error during GetStringChars");
//  UnicodeString ustr(jchars, jni->GetStringLength(j_string));
  CHECK_EXCEPTION(jni, "Error during GetStringLength");
  jni->ReleaseStringChars(j_string, jchars);
  CHECK_EXCEPTION(jni, "Error during ReleaseStringChars");
  std::string ret;
  return NULL;
  //  return ustr.toUTF8String(ret);

}

ClassReferenceHolder::ClassReferenceHolder(JNIEnv* jni, const char** classes,
                                           int size) {
  for (int i = 0; i < size; ++i) {
    LoadClass(jni, classes[i]);
  }
}
ClassReferenceHolder::~ClassReferenceHolder() {
  CHECK(classes_.empty(), "Must call FreeReferences() before dtor!");
}

void ClassReferenceHolder::FreeReferences(JNIEnv* jni) {
  for (std::map<std::string, jclass>::const_iterator it = classes_.begin();
       it != classes_.end(); ++it) {
    jni->DeleteGlobalRef(it->second);
  }
  classes_.clear();
}

jclass ClassReferenceHolder::GetClass(const std::string& name) {
  std::map<std::string, jclass>::iterator it = classes_.find(name);
  CHECK(it != classes_.end(), "Could not find class");
  return it->second;
}

void ClassReferenceHolder::LoadClass(JNIEnv* jni, const std::string& name) {
  jclass localRef = jni->FindClass(name.c_str());
  CHECK_EXCEPTION(jni, "Could not load class");
  CHECK(localRef, name.c_str());
  jclass globalRef = reinterpret_cast<jclass>(jni->NewGlobalRef(localRef));
  CHECK_EXCEPTION(jni, "error during NewGlobalRef");
  CHECK(globalRef, name.c_str());
  bool inserted = classes_.insert(std::make_pair(name, globalRef)).second;
  CHECK(inserted, "Duplicate class name");
}

void throwIllegalArgumentException(char * msg)
{
	throwException("java/lang/IllegalArgumentException",msg);
}

void throwOutOfMemoryError(char * msg)
{
	throwException("java/lang/OutOfMemoryError",msg);
}

void throwRuntimeException(char * msg)
{
	throwException("java/lang/RuntimeException",msg);
}

void throwException(char * classSignature,char * msg)
{
	//if (MediaEngine::g_vm)
	//{
	//	JNIEnv *env = NULL;
	//	MediaEngine::g_vm->AttachCurrentThread(&env, NULL);
	//	if (env)
	//	{
	//		jclass newExcCls = env->FindClass(classSignature);
	//		if (newExcCls == 0)
	//		{
	//			return;
	//		}
	//		env->ThrowNew(newExcCls, msg);
	//	}
	//}
}
