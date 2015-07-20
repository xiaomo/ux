#include "media/video_render.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <jni.h>

#include "util/trace.h"

VideoRender::VideoRender()
{
	buf = (uint8_t *) malloc(width * height * 3 / 2);
	memset(buf, 0, width * height * 3 / 2);
	getFirstYuv = false;
}

VideoRender::~VideoRender()
{
	getFirstYuv = false;
	free(buf);
	buf = NULL;
}
void VideoRender::PrintGLString(const char *name, GLenum s)
{
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}

void VideoRender::CheckGlError(const char* op)
{
	GLint error;
	for (error = glGetError(); error; error = glGetError())
	{
		LOGE("error::after %s() glError (0x%x)\n", op, error);
	}
}

GLuint VideoRender::BindTexture(GLuint texture, const char *buffer, GLuint w, GLuint h)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	CheckGlError("glBindTexture");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
	CheckGlError("glTexImage2D");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	CheckGlError("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CheckGlError("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CheckGlError("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	CheckGlError("glTexParameteri");

	return texture;
}

void VideoRender::RenderFrame()
{
	// First vertice.
	GLfloat squareVertices1[] =
	{ -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };

	GLfloat coordVertices1[] =
	{ 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };

	// Second vertice.
	GLfloat squareVertices2[] =
	{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

	GLfloat coordVertices2[] =
	{ -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f };

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	CheckGlError("glClearColor");
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CheckGlError("glClear");

	RelinkProgram(g_program);
	glUseProgram(g_program);
	CheckGlError("glUseProgram");

	GLint tex_y = glGetUniformLocation(g_program, "SamplerY");
	CheckGlError("glGetUniformLocation");
	GLint tex_u = glGetUniformLocation(g_program, "SamplerU");
	CheckGlError("glGetUniformLocation");
	GLint tex_v = glGetUniformLocation(g_program, "SamplerV");
	CheckGlError("glGetUniformLocation");

	glActiveTexture(GL_TEXTURE0);
	CheckGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, g_texYId);
	CheckGlError("glBindTexture");
	glUniform1i(tex_y, 0);
	CheckGlError("glUniform1i");

	glActiveTexture(GL_TEXTURE1);
	CheckGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, g_texUId);
	CheckGlError("glBindTexture");
	glUniform1i(tex_u, 1);
	CheckGlError("glUniform1i");

	glActiveTexture(GL_TEXTURE2);
	CheckGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, g_texVId);
	CheckGlError("glBindTexture");
	glUniform1i(tex_v, 2);
	CheckGlError("glUniform1i");

	glBindAttribLocation(g_program, ATTRIB_TEXTURE, "vPosition");
	CheckGlError("glBindAttribLocation");
	glBindAttribLocation(g_program, ATTRIB_VERTEX, "a_texCoord");
	CheckGlError("glBindAttribLocation");

	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, (g_is_vposition_true ? squareVertices2 : squareVertices1));
	CheckGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	CheckGlError("glEnableVertexAttribArray");

	glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, (g_is_vposition_true ? coordVertices2 : coordVertices1));
	CheckGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(ATTRIB_TEXTURE);
	CheckGlError("glEnableVertexAttribArray");

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	CheckGlError("glDrawArrays");
}

GLuint VideoRender::CreateShader(GLenum shaderType, const char* src)
{
	GLuint shader = glCreateShader(shaderType);
	if (!shader)
	{
		CheckGlError("glCreateShader");
		return 0;
	}
	glShaderSource(shader, 1, &src, NULL);

	GLint compiled = GL_FALSE;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLogLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0)
		{
			GLchar* infoLog = (GLchar*) malloc(infoLogLen);
			if (infoLog)
			{
				glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
				LOGE("Could not compile %s shader:\n%s\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
				free(infoLog);
			}
		}
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint VideoRender::CreateProgram(const char* vtxSrc, const char* fragSrc)
{
	GLuint vtxShader = 0;
	GLuint fragShader = 0;
	GLuint program = 0;
	GLint linked = GL_FALSE;

	vtxShader = CreateShader(GL_VERTEX_SHADER, vtxSrc);
	if (!vtxShader)
		goto exit;

	fragShader = CreateShader(GL_FRAGMENT_SHADER, fragSrc);
	if (!fragShader)
		goto exit;

	program = glCreateProgram();
	if (!program)
	{
		CheckGlError("glCreateProgram");
		goto exit;
	}

	glAttachShader(program, vtxShader);
	glAttachShader(program, fragShader);

	exit: glDeleteShader(vtxShader);
	glDeleteShader(fragShader);

	return program;
}

void VideoRender::RelinkProgram(GLuint program)
{
	GLint linked = GL_FALSE;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		LOGE("Could not link program");
		GLint infoLogLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen)
		{
			GLchar* infoLog = (GLchar*) malloc(infoLogLen);
			if (infoLog)
			{
				glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
				LOGE("Could not link program:\n%s\n", infoLog);
				free(infoLog);
			}
		}
		glDeleteProgram(program);
		program = 0;
	}

	GLuint gvPositionHandle;
	gvPositionHandle = glGetAttribLocation(program, "vPosition");
	CheckGlError("glGetAttribLocation");
	//LOGI("glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandle);
	g_is_vposition_true = ((gvPositionHandle == 0) ? false : true);
}

bool VideoRender::RegisterAudioCallback(VideoRenderTransport *cb)
{
	this->cb = cb;
	return true;
}
VideoRender* GetVideoRender(JNIEnv* jni, jobject jobj)
{
	jclass jcls = jni->GetObjectClass(jobj);
	jfieldID native_vr_id = jni->GetFieldID(jcls, "nativeVideoRender", "J");
	jlong j_p = jni->GetLongField(jobj, native_vr_id);
	if (j_p == 0)
	{
		return NULL;
	}
	return reinterpret_cast<VideoRender*>(j_p);
}

extern "C" JNIEXPORT jlong Java_com_hm_wokan_media_VideoRenderView_create(JNIEnv * env, jobject object)
{
	VideoRender * vr = new VideoRender();
	return (jlong) vr;
}

extern "C" JNIEXPORT void Java_com_hm_wokan_media_VideoRenderView_dispose(JNIEnv * env, jobject object)
{
	VideoRender * vr = GetVideoRender(env, object);
	if (!vr)
	{
		return;
	}
	delete vr;
	vr = NULL;
}

extern "C" JNIEXPORT void Java_com_hm_wokan_media_VideoRenderView_gLInit(JNIEnv * env, jobject object)
{
	VideoRender * vr = GetVideoRender(env, object);
	if (!vr)
	{
		return;
	}
	vr->PrintGLString("Version", GL_VERSION);
	vr->PrintGLString("Vendor", GL_VENDOR);
	vr->PrintGLString("Renderer", GL_RENDERER);
	vr->PrintGLString("Extensions", GL_EXTENSIONS);

	// Create the program.
	vr->g_program = vr->CreateProgram(vr->SHADER_VERTEX, vr->SHADER_FRAG);

	glGenTextures(1, &vr->g_texYId);
	glGenTextures(1, &vr->g_texUId);
	glGenTextures(1, &vr->g_texVId);
}

extern "C" JNIEXPORT void Java_com_hm_wokan_media_VideoRenderView_gLUninit(JNIEnv * env, jobject object)
{
	VideoRender * vr = GetVideoRender(env, object);
	if (!vr)
	{
		return;
	}
}

extern "C" JNIEXPORT void Java_com_hm_wokan_media_VideoRenderView_gLResize(JNIEnv * env, jobject object, jint width, jint height)
{
	VideoRender * vr = GetVideoRender(env, object);
	if (!vr)
	{
		return;
	}
	glViewport(0, 0, width, height);
	vr->CheckGlError("glViewport");
}

extern "C" JNIEXPORT void Java_com_hm_wokan_media_VideoRenderView_gLRender(JNIEnv * env, jobject object)
{
	VideoRender * vr = GetVideoRender(env, object);
	if (!vr || !vr->buf)
	{
		return;
	}
	vr->isYUVValid = false;
	vr->buf_len = 0;
	vr->cb->NeedMoreVideoData(&vr->isYUVValid, vr->buf, &vr->buf_len);
	if (!vr->getFirstYuv)
	{
		vr->getFirstYuv = vr->isYUVValid;
	}
	if (vr->getFirstYuv)
	{
		vr->BindTexture(vr->g_texYId, (char*) vr->buf, vr->width, vr->height);
		vr->BindTexture(vr->g_texUId, (char*) vr->buf + vr->width * vr->height, vr->width / 2, vr->height / 2);
		vr->BindTexture(vr->g_texVId, (char*) vr->buf + vr->width * vr->height + vr->width * vr->height / 4, vr->width / 2, vr->height / 2);
		vr->RenderFrame();
	}
}
