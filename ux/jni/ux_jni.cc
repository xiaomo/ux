#include "ux.h"

#ifdef _WIN32
#else
#include <string>
#include <jni.h>
#include "util/jni_helpers.h"
#include "util/system_tools.h"
#include "util/trace.h"

#define JOWW(rettype, name) extern "C" rettype JNIEXPORT JNICALL Java_com_xchat_communication_Communication_##name

JavaVM* g_vm = NULL;
jobject g_hall_data_callback = NULL;
jobject g_room_data_callback = NULL;

void HallDataCallback(char *data)
{
	if (g_hall_data_callback == NULL) return;

	JNIEnv* env = NULL;
	g_vm->AttachCurrentThread(&env, 0);

	jclass javaClass = env->GetObjectClass(g_hall_data_callback);
	if (NULL == javaClass)
	{
		LOGE("Find javaClass fail.");
		return;
	}

	jmethodID javaCallback = env->GetMethodID(javaClass, "onHallData", "(Ljava/lang/String;)V");
	if (NULL == javaCallback)
	{
		LOGE("Find method onNetworkChange() fail.");
		return;
	}
	jstring jstr = Char2JString(env, data);
	env->CallVoidMethod(g_hall_data_callback, javaCallback, jstr);

	env->DeleteLocalRef(javaClass);
	g_vm->DetachCurrentThread();
}

void RoomDataCallback(int roomid, char *data)
{
	if (g_room_data_callback == NULL) return;

	JNIEnv* env = NULL;
	g_vm->AttachCurrentThread(&env, 0);

	jclass javaClass = env->GetObjectClass(g_room_data_callback);
	if (NULL == javaClass)
	{
		LOGE("Find javaClass fail.");
		return;
	}

	jmethodID javaCallback = env->GetMethodID(javaClass, "onRoomData", "(ILjava/lang/String;)V");
	if (NULL == javaCallback)
	{
		LOGE("Find method onNetworkChange() fail.");
		return;
	}
	jstring jstr = Char2JString(env, data);
	env->CallVoidMethod(g_room_data_callback, javaCallback, roomid,jstr);

	env->DeleteLocalRef(javaClass);
	g_vm->DetachCurrentThread();

}

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	g_vm = vm;
	return JNI_VERSION_1_4;
}

UX* GetUX(JNIEnv* jni, jobject jobj)
{
	jclass jcls = jni->GetObjectClass(jobj);
	jfieldID native_voe_id = jni->GetFieldID(jcls, "nativeUX", "J");
	jlong j_p = jni->GetLongField(jobj, native_voe_id);
	if (j_p == 0)
	{
		return NULL;
	}
	return reinterpret_cast<UX*>(j_p);
}

JOWW(jlong, create)(JNIEnv* jni, jobject jobj)
{
	UX * me = new UX();
	if (!me)
	{
		return 0;
	}
	return jlongFromPointer(me);
}

JOWW(void, exit)(JNIEnv* jni, jobject jobj)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return ;
	}
	me->Exit();
}

JOWW(void, registerOnHallDataCallback)(JNIEnv* jni, jobject jobj, jobject callback)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return;
	}
	if (!g_hall_data_callback)
	{
		jni->DeleteGlobalRef(g_hall_data_callback);
	}
	g_hall_data_callback = jni->NewGlobalRef(callback);
	me->RegisterHallDataCallback(boost::bind(HallDataCallback, _1));
}

JOWW(void, registerOnRoomDataCallback)(JNIEnv* jni, jobject jobj, jobject callback)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return;
	}
	if (!g_room_data_callback)
	{
		jni->DeleteGlobalRef(g_room_data_callback);
	}
	g_room_data_callback = jni->NewGlobalRef(callback);
	me->RegisterRoomDataCallback(boost::bind(RoomDataCallback, _1, _2));
}

JOWW(void, startHall)(JNIEnv* jni, jobject jobj, jstring ip, int port)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return;
	}
	char * data = JString2Char(jni, ip);
	std::string ipstr = data;
	me->StartHall(ipstr, port);
	free(data);
}

JOWW(void, startRoom)(JNIEnv* jni, jobject jobj, jstring ip, int port)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return;
	}
	char * data = JString2Char(jni, ip);
	std::string ipstr = data;
	me->StartRoom(ipstr, port);
	free(data);
}

JOWW(void, addMedia)(JNIEnv* jni, jobject jobj, jstring ip, int port, int user_id, jobject video_render, jobject audio_track)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return;
	}
	char * data = JString2Char(jni, ip);
	std::string ipstr = data;
	me->AddMedia(ipstr, port, user_id, boost::shared_ptr<VideoRender>(new VideoRender()), boost::shared_ptr<AudioTrack>(new AudioTrack()));
	free(data);
}


JOWW(void, sendHallData)(JNIEnv* jni, jobject jobj,jstring json_data)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return;
	}
	char * data = JString2Char(jni,json_data);
	me->SendHallData(data, strlen(data));
	free(data);
}

JOWW(void, sendRoomData)(JNIEnv* jni, jobject jobj, jstring json_data)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return;
	}
	char * data = JString2Char(jni, json_data);
	me->SendRoomData(data, strlen(data));
	free(data);
}

#endif //_WIN32