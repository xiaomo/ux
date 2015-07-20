LOCAL_PATH		 		:=	$(call my-dir)

#boost
BOOST_VERSION				:= 1_55
include $(CLEAR_VARS)
LOCAL_MODULE     			:= lib_boost_system 
LOCAL_SRC_FILES  			:= third_party/boost/lib/libboost_system-gcc-mt-$(BOOST_VERSION).a
LOCAL_EXPORT_C_INCLUDES 	:= $(LOCAL_PATH)/third_party/boost/include
include $(PREBUILT_STATIC_LIBRARY) 
	
include $(CLEAR_VARS)   
LOCAL_MODULE     			:= lib_boost_thread 
LOCAL_SRC_FILES  			:= third_party/boost/lib/libboost_thread-gcc-mt-$(BOOST_VERSION).a
LOCAL_EXPORT_C_INCLUDES 	:= $(LOCAL_PATH)/third_party/boost/include
include $(PREBUILT_STATIC_LIBRARY) 

#ffmpeg
include $(CLEAR_VARS)
LOCAL_MODULE				:= libavcodec
LOCAL_SRC_FILES				:= third_party/ffmpeg/lib/libavcodec.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/third_party/ffmpeg/include
include $(PREBUILT_STATIC_LIBRARY)
	 
include $(CLEAR_VARS)
LOCAL_MODULE				:= libavformat
LOCAL_SRC_FILES				:= third_party/ffmpeg/lib/libavformat.a
LOCAL_EXPORT_C_INCLUDES		:= $(LOCAL_PATH)/third_party/ffmpeg/include
include $(PREBUILT_STATIC_LIBRARY)
	 
include $(CLEAR_VARS)
LOCAL_MODULE				:= libavutil
LOCAL_SRC_FILES				:= third_party/ffmpeg/lib/libavutil.a
LOCAL_EXPORT_C_INCLUDES		:= $(LOCAL_PATH)/third_party/ffmpeg/include
include $(PREBUILT_STATIC_LIBRARY)
	 
include $(CLEAR_VARS)
LOCAL_MODULE				:= libavfilter
LOCAL_SRC_FILES				:= third_party/ffmpeg/lib/libavfilter.a
LOCAL_EXPORT_C_INCLUDES		:= $(LOCAL_PATH)/third_party/ffmpeg/include
include $(PREBUILT_STATIC_LIBRARY)
	 
include $(CLEAR_VARS)
LOCAL_MODULE				:= libswresample
LOCAL_SRC_FILES				:= third_party/ffmpeg/lib/libswresample.a
LOCAL_EXPORT_C_INCLUDES		:= $(LOCAL_PATH)/third_party/ffmpeg/include
include $(PREBUILT_STATIC_LIBRARY)
	
include $(CLEAR_VARS)
LOCAL_MODULE				:= libx264
LOCAL_SRC_FILES				:= third_party/ffmpeg/lib/libx264.a
LOCAL_EXPORT_C_INCLUDES		:= $(LOCAL_PATH)/third_party/ffmpeg/include
include $(PREBUILT_STATIC_LIBRARY)
		
include $(CLEAR_VARS)
LOCAL_MODULE				:= libspeex
LOCAL_SRC_FILES				:= third_party/ffmpeg/lib/libspeex.a
LOCAL_EXPORT_C_INCLUDES		:= $(LOCAL_PATH)/third_party/ffmpeg/include
include $(PREBUILT_STATIC_LIBRARY)
	
include $(CLEAR_VARS)
LOCAL_MODULE				:= libfreetype
LOCAL_SRC_FILES				:= third_party/ffmpeg/lib/libfreetype.a
LOCAL_EXPORT_C_INCLUDES 	:= $(LOCAL_PATH)/third_party/ffmpeg/include
include $(PREBUILT_STATIC_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE	 		:=  xchat
LOCAL_C_INCLUDES 		:=  $(LOCAL_PATH) 

SRC_FILES				:= $(wildcard $(LOCAL_PATH)/util/*.cc) \
						   $(wildcard $(LOCAL_PATH)/media/*.cc) \
						   $(wildcard $(LOCAL_PATH)/io/*.cc) \
						   $(wildcard $(LOCAL_PATH)/*.cc) \
						   
LOCAL_SRC_FILES 		:= $(SRC_FILES:$(LOCAL_PATH)/%=%) 
#LOCAL_SRC_FILES 		:= $(patsubst $(LOCAL_PATH)/test.cc,,$(LOCAL_SRC_FILES))

LOCAL_CFLAGS			:=  -D__STDC_CONSTANT_MACROS

LOCAL_STATIC_LIBRARIES  := 	lib_boost_system \
							lib_boost_thread \
							\
							libavformat \
							libavcodec \
							libavfilter \
							libavutil \
							libfdk-aac \
							libopus \
							libpostproc \
							libswresample \
							libswscale \
							libvo-aacenc \
							libx264 \
							libogg \
							libspeex \
							libfreetype 
							
LOCAL_SHARED_LIBRARIES	:= 
LOCAL_LDLIBS 	 		:= -ldl -llog -lz -lGLESv2 -lOpenSLES -landroid
LOCAL_CPPFLAGS			:= -std=c++11 -pthread -frtti -fexceptions -fpermissive -DBOOST_NO_CXX11_NULLPTR -DBOOST_ASIO_DISABLE_STD_ADDRESSOF
LOCAL_ARM_MODE 			:= arm


ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_ARM_NEON			:= true
endif

ifeq ($(APP_OPTIM),debug)
	LOCAL_CFLAGS			+=  -O0 -gstabs+ -UNDEBUG -D_DEBUG
	LOCAL_CPPFLAGS			+=  -O0 -gstabs+ -UNDEBUG -D_DEBUG
endif
include $(BUILD_SHARED_LIBRARY)