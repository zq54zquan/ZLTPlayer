/********************************************************************************
 * a simple player use ffmpeg to decode 
********************************************************************************/
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
}
#include <vector>
#include <cstdio>
/********************************************************************************
 * collect video/audio/suntitle streams form ctx
 * param res cant be null
********************************************************************************/
void collect_stream(AVFormatContext *ctx, AVMediaType codecType,std::vector<int> *res) {
	for(int i = 0 ; i < ctx->nb_streams; ++i) {
		if(ctx->streams[i]->codec->codec_type == codecType) 
			res->push_back(i);
	}
}

void decode_stream(AVFormatContext *ctx, int stream_index) {

}

int main(const int args, const char *argv[]) {
	av_register_all();
	AVFormatContext *ctx = avformat_alloc_context();
	const char *fileurl = "file:/Users/zhou/Desktop/mda-heyt57ez3gk93zbt.mp4";
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
				printf("vsize:%d",v_streams->size());
				printf("asize:%d",a_streams->size());
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

