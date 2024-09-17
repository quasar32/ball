#include <SDL2/SDL.h>
#include <glad/gl.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include "draw.h"
#include "ball.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

uint8_t pixels[640 * 480 * 3];

struct render_ctx {
	AVCodecContext *codec;
	AVPacket *pkt;
	FILE *fp;
};

static void encode(struct render_ctx *ctx, AVFrame *frame) {
	int ret;
	ret = avcodec_send_frame(ctx->codec, frame);
	if (ret < 0)
		die("avcodec_send_frame\n");
	while (ret >= 0) {
		ret = avcodec_receive_packet(ctx->codec, ctx->pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		if (ret < 0) 
			die("avcodec_receive_packet\n");
		fwrite(ctx->pkt->data, 1, ctx->pkt->size, ctx->fp);
		av_packet_unref(ctx->pkt);
	}
}

static void render(void) {
	AVFormatContext *ctx;
	const AVCodec *codec;
	AVFrame *rgb, *yuv;
	struct render_ctx ctx;
	int ret;
	struct SwsContext *sws;
	int i;

	codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec) 
		die("avcodec_find_encoder\n");
	ctx.codec = avcodec_alloc_context3(codec);
	if (!ctx.codec)
		die("avcodec_alloc_context3\n");
	ctx.codec->bit_rate = 400000;
	ctx.codec->width = 640;
	ctx.codec->height = 480;
	ctx.codec->time_base.num = 1; 
	ctx.codec->time_base.den = 20; 
	ctx.codec->framerate.num = 20; 
	ctx.codec->framerate.den = 1; 
	ctx.codec->gop_size = 10;
	ctx.codec->max_b_frames = 1;
	ctx.codec->pix_fmt = AV_PIX_FMT_YUV420P;
	av_opt_set(ctx.codec->priv_data, "preset", "slow", 0);
	ret = avcodec_open2(ctx.codec, codec, NULL);
	if (ret < 0)
		die("avcodec_open2: %s\n", av_err2str(ret));
	ctx.fp = fopen("billiards.mp4", "wb");
	if (!ctx.fp)
		die("fopen: %s\n", strerror(errno));
	yuv = av_frame_alloc();
	if (!yuv)
		die("av_frame_alloc\n");
	yuv->format = ctx.codec->pix_fmt;
	yuv->width = ctx.codec->width;
	yuv->height = ctx.codec->height;
	ret = av_frame_get_buffer(yuv, 0);
	if (ret < 0)
		die("av_frame_get_buffer\n");
	rgb = av_frame_alloc();
	if (!rgb)
		die("av_frame_alloc\n");
	rgb->format = AV_PIX_FMT_RGB24;
	rgb->width = ctx.codec->width;
	rgb->height = ctx.codec->height;
	ret = av_frame_get_buffer(rgb, 0);
	if (ret < 0)
		die("av_frame_get_buffer\n");
	sws = sws_getContext(rgb->width, rgb->height, rgb->format,
			yuv->width, yuv->height, yuv->format,
			SWS_BILINEAR, NULL, NULL, NULL);
	ctx.pkt = av_packet_alloc();	
	if (!ctx.pkt)
		die("av_packet_alloc\n");
	for (i = 0; i < 200; i++) {
		ret = av_frame_make_writable(yuv);
		if (ret < 0)
			die("av_frame_make_writable\n");
		simulate();
		draw();
		glReadPixels(0, 0, 640, 480, GL_RGB, 
				GL_UNSIGNED_BYTE, rgb->data[0]);
		rgb->pts = i;
		ret = sws_scale(sws, (const uint8_t * const *) rgb->data,
				rgb->linesize, 0, rgb->height,
				yuv->data, yuv->linesize);
		yuv->pts = i;
		encode(&ctx, yuv);
	}
	encode(&ctx, NULL);
	sws_freeContext(sws);
	fclose(ctx.fp);
	avcodec_free_context(&ctx.codec);
	av_frame_free(&rgb);
	av_frame_free(&yuv);
	av_packet_free(&ctx.pkt);
}

int main(int argc, char **argv) {
	SDL_Window *wnd;
	GLuint fbo;
	GLuint tex;
	if (SDL_Init(SDL_INIT_EVERYTHING))
		die("SDL_Init: %s\n", SDL_GetError());
	if (atexit(SDL_Quit))
		die("atexit: SDL_Quit\n");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
			SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	wnd = SDL_CreateWindow("Billiards", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, 640, 480, 
			SDL_WINDOW_OPENGL);
	if (!wnd)
		die("SDL_CreateWindow: %s\n", SDL_GetError());
	if (!SDL_GL_CreateContext(wnd))
		die("SDL_GL_CreateContext: %s\n", SDL_GetError());
	gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
	init_balls();
	init_draw();
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 
			0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
			GL_TEXTURE_2D, tex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != 
			GL_FRAMEBUFFER_COMPLETE)
		die("glCheckFramebufferStatus: %u\n", glGetError());
	render();
	return 0;
}
