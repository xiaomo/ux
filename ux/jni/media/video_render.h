#ifndef VIDEORENDER_H_
#define VIDEORENDER_H_

#include <stdio.h>
extern "C"
{
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
}

class VideoRenderTransport
{
	public:
		virtual void NeedMoreVideoData(bool *is_yuv_valid,uint8_t * buf,int *buf_len)=0;
		virtual ~ VideoRenderTransport(){};
};

class VideoRender
{
	public:
		VideoRender();
		~VideoRender();
		bool RegisterAudioCallback(VideoRenderTransport *cb);
		bool Start();
		bool Stop();
		VideoRenderTransport *cb=NULL;
		bool getFirstYuv=false;
		bool isYUVValid = false;
		uint8_t * buf =NULL;
		int buf_len=0;
		int width = 240;
		int height = 320;

		enum {
		    ATTRIB_VERTEX,
		    ATTRIB_TEXTURE,
		};

		GLuint   g_texYId = 0;
		GLuint   g_texUId = 0;
		GLuint   g_texVId = 0;
		GLuint   g_program = 0;

		// Is vposition true?
		bool     g_is_vposition_true = true;

		// Shader.vert文件内容
		// 渲染形状的顶点的OpenGLES 图形代码
		const char* SHADER_VERTEX =
		    "attribute vec4 vPosition;    \n"
		    "attribute vec2 a_texCoord;   \n"
		    "varying vec2 tc;             \n"
		    "void main()                  \n"
		    "{                            \n"
		    "   gl_Position = vPosition;  \n"
		    "   tc = a_texCoord;          \n"
		    "}                            \n";

		// Shader.frag文件内容
		// 渲染形状的外观（颜色或纹理）的OpenGLES 代码。
		const char* SHADER_FRAG =
		    "varying lowp vec2 tc;                          \n"
		    "uniform sampler2D SamplerY;                    \n"
		    "uniform sampler2D SamplerU;                    \n"
		    "uniform sampler2D SamplerV;                    \n"
		    "void main(void)                                \n"
		    "{                                              \n"
		    "   mediump vec3 yuv;                           \n"
		    "   lowp vec3 rgb;                              \n"
		    "   yuv.x = texture2D(SamplerY, tc).r;          \n"
		    "   yuv.y = texture2D(SamplerU, tc).r - 0.5;    \n"
		    "   yuv.z = texture2D(SamplerV, tc).r - 0.5;    \n"
		    "   rgb = mat3( 1,   1,   1,                    \n"
		    "               0,       -0.39465,  2.03211,    \n"
		    "               1.13983,   -0.58060,  0) * yuv; \n"
		    "               gl_FragColor = vec4(rgb, 1);    \n"
		    "}                                              \n";


		/*******************************************    函数前置声明  *************************************/

		void PrintGLString(const char *name, GLenum s);
		void CheckGlError(const char* op);
		GLuint BindTexture(GLuint texture, const char *buffer, GLuint w , GLuint h);
		void RenderFrame();
		GLuint CreateShader(GLenum shaderType, const char* src);
		GLuint CreateProgram(const char* vtxSrc, const char* fragSrc);
		void RelinkProgram(GLuint program);

};

#endif /* VIDEORENDER_H_ */
