/********************************************************************************
 * a simple player use ffmpeg to decode 
*******************************************************************************/
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libswscale/swscale.h>
#include <SDL/SDL.h>
}
#include <vector>
#include <cstdio>
typedef enum {
	ZLPlayer_Stream_Kind_Video, 
	ZLPlayer_Stream_Kind_Audio,
	ZLPlayer_Stream_Kind_Subtitle
}ZLPlayer_Stream_Kind; /// define stream type for disprete the different kinds stream
int thread_exit = 0;
int sfp_refresh_thread(void *opaque) {
	SDL_Event event;
	while (0 == thread_exit) {
		event.type = SDL_USEREVENT+1;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}
	return 0;
}

int v_index  =  0;

/********************************************************************************
 * collect video/audio/suntitle streams form ctx
 * param res cant be null
********************************************************************************/
void collect_stream(AVFormatContext *ctx, AVMediaType codecType,std::vector<int> *res) {
	for(unsigned int i = 0 ; i < ctx->nb_streams; ++i) {
		if((ctx->streams[i]->codec->codec_type) == codecType) 
			res->push_back(i);
			if((ctx->streams[i]->codec->codec_type)== AVMEDIA_TYPE_VIDEO) {
				v_index = i;
			}
	}
}

void decode_stream(AVFormatContext *ctx, std::vector<int> stream_indexs,ZLPlayer_Stream_Kind kind) {
	for (std::vector<int>::const_iterator c_it = stream_indexs.cbegin();c_it != stream_indexs.cend(); ++c_it) {
		AVStream *stream = ctx->streams[*c_it];	
		AVCodecContext *cdctx = stream->codec;
		AVCodec *codec = avcodec_find_decoder(cdctx->codec_id);
		if(!codec) {
			printf("failed find codec");
		}else  {
			int res = avcodec_open2(cdctx,codec,NULL);
			if (res != 0) {
				printf("faild open codec");
			}else  {
				SDL_Surface *screen;
				SDL_Overlay *layer;
				if(kind == ZLPlayer_Stream_Kind_Video) {
					printf("size:%d*%d\n",cdctx->width,cdctx->height);	
					SDL_WM_SetCaption("Simplest FFmpeg Player",NULL);  
					screen = SDL_SetVideoMode(cdctx->width,cdctx->height,0,0);
					if(NULL == screen) {
						break;
					}
					layer = SDL_CreateYUVOverlay(cdctx->width,cdctx->height,SDL_YV12_OVERLAY,screen);
					SDL_Thread *video_tid =  SDL_CreateThread(sfp_refresh_thread,NULL);
				}
				printf("duration:%ld\n",ctx->duration/1000/1000);
				SDL_Rect rect;
				rect.x = 0;
				rect.y = 0;
				rect.w = cdctx->width;
				rect.h = cdctx->height;
				
				SDL_Event event;
				AVFrame *frame = av_frame_alloc();
				AVFrame *pFrame = av_frame_alloc();
				AVPacket *packet = av_packet_alloc();
				printf("kind == %d",kind == ZLPlayer_Stream_Kind_Video);
				struct SwsContext *swsctx = sws_getContext(cdctx->width,cdctx->height,cdctx->pix_fmt,cdctx->width,cdctx->height,AV_PIX_FMT_YUV420P,SWS_BICUBIC,NULL,NULL,NULL);
				while(kind == ZLPlayer_Stream_Kind_Video&&av_read_frame(ctx,packet)>=0) {
					SDL_WaitEvent(&event);
					printf("vindex:%d,streamindex=:%d\n",v_index,packet->stream_index);
					if(packet->stream_index != v_index)
							continue;
					int picgot = 0;
					printf("pksize:%d\n",packet->size);
					int res = avcodec_decode_video2(cdctx,frame,&picgot,packet);
					if (res < 0) {
							
					}
					if (picgot) {
						printf("get pic\n");
						SDL_LockYUVOverlay(layer);
						pFrame->data[0] = layer->pixels[0];
						pFrame->data[1] = layer->pixels[2];
						pFrame->data[2] = layer->pixels[1];
						pFrame->linesize[0] = layer->pitches[0];
						pFrame->linesize[1] = layer->pitches[2];
						pFrame->linesize[2] = layer->pitches[1];
						sws_scale(swsctx,(const uint8_t * const *)frame->data,frame->linesize,0,cdctx->height,pFrame->data,pFrame->linesize);
						SDL_UnlockYUVOverlay(layer);	
						SDL_DisplayYUVOverlay(layer,&rect);
					}else {
						printf("cant get pic\n");
					}
				}
			}
		}
		
	}	
}


/******************************************************************************** 
 * start sdl 
********************************************************************************/
void init_SDL(void) {
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)==-1) {
		printf("sdl init failed");		
	}
}

/********************************************************************************
 * end sdl
********************************************************************************/
void quite_SDL(void) {
	SDL_Quit();
}
#undef main
extern "C" int main(int argc, char *argv[]){
	init_SDL();
	av_register_all();
	avformat_network_init();
	AVFormatContext *ctx = avformat_alloc_context();
	const char *fileurl = "file:/Users/quanzhou/Desktop/WeChatSight240.mp4";
	int res = avformat_open_input(&ctx, fileurl, NULL, 0);
	if(0 == res) {
		res = avformat_find_stream_info(ctx,NULL);
		if (res>=0) {
			if(ctx->nb_streams>0) {
				std::vector<int> *v_streams = new std::vector<int>();
				collect_stream(ctx,AVMEDIA_TYPE_VIDEO,v_streams);
				std::vector<int> *a_streams = new std::vector<int>();
				collect_stream(ctx,AVMEDIA_TYPE_AUDIO,a_streams);
				decode_stream(ctx,*v_streams,ZLPlayer_Stream_Kind_Video);
				//decode_stream(ctx,*a_streams,ZLPlayer_Stream_Kind_Audio);
			}
		}else {
			printf("open stream failed");
		}
	}else {
		printf("open file failed");	
		avformat_free_context(ctx);
	} 
	quite_SDL();
	return 0;
}
