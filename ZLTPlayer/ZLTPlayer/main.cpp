/********************************************************************************
 * a simple player use ffmpeg to decode
 ********************************************************************************/
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <SDL/SDL.h>
}
#include <vector>
#include <cstdio>
typedef enum {
    ZLPlayer_Stream_Kind_Video,
    ZLPlayer_Stream_Kind_Audio,
    ZLPlayer_Stream_Kind_Subtitle
}ZLPlayer_Stream_Kind; /// define stream type for disprete the different kinds stream

/********************************************************************************
 * collect video/audio/suntitle streams form ctx
 * param res cant be null
 ********************************************************************************/
void collect_stream(AVFormatContext *ctx, AVMediaType codecType,std::vector<int> *res) {
    for(unsigned int i = 0 ; i < ctx->nb_streams; ++i) {
        if(ctx->streams[i]->codecpar->codec_type == codecType)
            res->push_back(i);
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
                printf("size:%d*%d\n",cdctx->width,cdctx->height);
                printf("duration:%ld",ctx->duration/1000/1000);
                //AVFrame *pframe = av_frame_alloc();
                //AVFrame *pframe_yuv = av_frame_alloc();
                FILE *yuvfile = fopen("./yuvdata.yuv","wb+");
                FILE *h264file = fopen("./h264data.h264","wb+"); // write data to a h264 file
                AVPacket *packet = av_packet_alloc();
                while(av_read_frame(ctx,packet)>=0) {
                    if (kind == ZLPlayer_Stream_Kind_Video) {
                        uint8_t *h264_data = packet->data;
                        fwrite(h264_data,packet->size,1,h264file);
                    }else {
                        
                    }
                }
                fclose(yuvfile);
                fclose(h264file);
            }
        }
        
    }
}


/********************************************************************************
 * start sdl
 ********************************************************************************/
void init_SDL(void) {
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1) {
        printf("sdl init failed");
    }
}
#undef main
extern "C" int main(int argc, char *argv[]){
    init_SDL();
    
    av_register_all();
    AVFormatContext *ctx = avformat_alloc_context();
    const char *fileurl = "file:/Users/quanzhou/Desktop/WeChatSight240.mp4";
    int res = avformat_open_input(&ctx, fileurl, NULL, 0);
    if(0 == res) {
        res = avformat_find_stream_info(ctx,NULL);
        if (res>=0) {
            printf("nstreams:%d",ctx->nb_streams);
            if(ctx->nb_streams>0) {
                std::vector<int> *v_streams = new std::vector<int>();
                collect_stream(ctx,AVMEDIA_TYPE_VIDEO,v_streams);
                std::vector<int> *a_streams = new std::vector<int>();
                collect_stream(ctx,AVMEDIA_TYPE_AUDIO,a_streams);
                printf("vsize:%ld",v_streams->size());
                printf("asize:%ld",a_streams->size());
                decode_stream(ctx,*v_streams,ZLPlayer_Stream_Kind_Video);
                decode_stream(ctx,*a_streams,ZLPlayer_Stream_Kind_Audio);
            }
        }else {
            printf("open stream failed");
        }
    }else {
        printf("open file failed");
        avformat_free_context(ctx);
    }
    return 0;
}
