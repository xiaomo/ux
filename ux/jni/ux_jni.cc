#include "ux.h"

#ifdef _WIN32
#else
#include <jni.h>
#include "util/jni_helpers.h"
#include "util/system_tools.h"
#include "util/trace.h"

#define JOWW(rettype, name) extern "C" rettype JNIEXPORT JNICALL Java_com_xchat_communication_Communication_##name

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	//UX::g_vm = vm;
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

JOWW(jboolean, init)(JNIEnv* jni, jobject jobj, jobject context, jobject jconfig)
{
	UX * me = GetUX(jni, jobj);
	if (!me) {
		return false;
	}
	//wokan::Config config;
	//jclass classParam = jni->FindClass("com/hm/wokan/media/Config");
	//if (classParam) {
	//	jfieldID field = jni->GetFieldID(classParam, "audioSampleRate", "I");
	//	if (field) {
	//		config.audio_sample_rate = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "audioBitrate", "I");
	//	if (field) {
	//		config.audio_bitrate = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "audioChannel", "I");
	//	if (field) {
	//		config.audio_channel = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "audioCodec", "I");
	//	if (field) {
	//		config.audio_codec = static_cast<wokan::CodecID>((uint32)jni->GetIntField(jconfig, field));
	//	}
	//	field = jni->GetFieldID(classParam, "videoCode", "I");
	//	if (field) {
	//		config.video_codec = static_cast<wokan::CodecID>((uint32)jni->GetIntField(jconfig, field));
	//	}
	//	field = jni->GetFieldID(classParam, "videoWidth", "I");
	//	if (field) {
	//		config._video_width = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "videoHeight", "I");
	//	if (field) {
	//		config._video_height = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "videoBitrate", "I");
	//	if (field) {
	//		config._video_bitrate = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "videoFps", "I");
	//	if (field) {
	//		config._video_fps = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "videoIntervalIframe", "I");
	//	if (field) {
	//		config._video_interval_iframe = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "debug", "Z");
	//	if (field) {
	//		config.debug = (bool)jni->GetBooleanField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "needAudioProcess", "Z");
	//	if (field)
	//	{
	//		config.audio_process = (bool)jni->GetBooleanField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "fastest", "Z");
	//	if (field)
	//	{
	//		config.fastest = (bool)jni->GetBooleanField(jconfig, field);
	//	}
	//	field = jni->GetFieldID(classParam, "displayRotation", "I");
	//	if (field) {
	//		config.displayRotation = (uint32)jni->GetIntField(jconfig, field);
	//	}
	//}

	//if (config.displayRotation == 0 || config.displayRotation == 180)
	//{
	//	int temp = config._video_width;
	//	config._video_width = config._video_height;
	//	config._video_height = temp;
	//}

	//return me->Init(jni, jobj, context, config);
}

//JOWW(jboolean, authorize)(JNIEnv* jni, jobject jobj)
//{
//	UX * me = GetUX(jni, jobj);
//	if (!me)
//	{
//		return false;
//	}
//	return me->Authorize();
//}
//
//JOWW(jboolean, addChannel)(JNIEnv* jni, jobject jobj, jint type, jstring server, jshort port, jstring sn, jstring username, jstring pwd,
//	jboolean request_video, jboolean direct_nvs)
//{
//	char* ip = JString2Char(jni, server);
//	char* localSN = JString2Char(jni, sn);
//	char* localUsername = JString2Char(jni, username);
//	char* localPwd = JString2Char(jni, pwd);
//	UX * me = GetUX(jni, jobj);
//	if (!me)
//	{
//		return false;
//	}
//
//	boost::shared_ptr<NvsChannel> channel = boost::make_shared<NvsChannel>(static_cast<ChannelType>(type), ip, port, localSN, localUsername, localPwd, request_video, direct_nvs);
//	bool res = me->AddChannel(channel);
//	free(ip);
//	ip = NULL;
//	free(localSN);
//	localSN = NULL;
//	free(localUsername);
//	localUsername = NULL;
//	free(localPwd);
//	localPwd = NULL;
//	return res;
//}
//
//JOWW(jboolean, delChannel)(JNIEnv* jni, jobject jobj, jstring sn)
//{
//	char* localSN = JString2Char(jni, sn);
//	UX * me = GetUX(jni, jobj);
//	if (!me)
//	{
//		return false;
//	}
//	return me->DelChannel(localSN);
//}
//
JOWW(jboolean, start)(JNIEnv* jni, jobject jobj)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return false;
	}
	return me->Start();
}

JOWW(jboolean, stop)(JNIEnv* jni, jobject jobj)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return false;
	}
	return me->Stop();
}

JOWW(jboolean, dispose)(JNIEnv* jni, jobject jobj)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return false;
	}
	me->Dispose(jni, jobj);
	return true;
}

JOWW(jboolean, destroy)(JNIEnv* jni, jobject jobj)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return false;
	}
	delete me;
	me = NULL;
	return true;
}

JOWW(jlong, getNativeVideoRender)(JNIEnv* jni, jobject jobj)
{
	UX * me = GetUX(jni, jobj);
	if (!me)
	{
		return 0;
	}
	return (jlong)(me->GetVideoRender().get());
}

//JOWW(void, registerExternalVideoEncoder)(JNIEnv* jni, jobject jobj, jobject videoencoder)
//{
//	UX * me = GetUX(jni, jobj);
//	if (!me)
//	{
//		return;
//	}
//	boost::shared_ptr<VideoEncoderHardware> encoder = boost::make_shared<VideoEncoderHardware>(wokan::CodecID::CODEC_VIDEO_H264, 200 * 1000, 10, 5, 240, 320, UX::g_vm, videoencoder);
//	me->RegisterExternalVideoEncoder(encoder);
//}
//
//JOWW(jlong, getNativeAudioRecoder)(JNIEnv* jni, jobject jobj)
//{
//	UX * me = GetUX(jni, jobj);
//	if (!me)
//	{
//		return 0;
//	}
//	return (jlong)(me->GetAudioRecoder().get());
//}
//
//JOWW(void, registerOnNetworkChangeCallback)(JNIEnv* jni, jobject jobj, jobject callback)
//{
//	UX * me = GetUX(jni, jobj);
//	if (!me || !callback)
//	{
//		return;
//	}
//	if (UX::network_callback != NULL)
//	{
//		jni->DeleteGlobalRef(UX::network_callback);
//		UX::network_callback = NULL;
//	}
//	UX::network_callback = jni->NewGlobalRef(callback);
//}
#endif //_WIN32