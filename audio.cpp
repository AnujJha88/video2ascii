#include <SDL2/SDL_audio.h>
#include <cstddef>
#include<iostream>
#include<SDL2/SDL.h>
#include<atomic>
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/avutil.h>        // For av_malloc/av_free
    #include <libavutil/channel_layout.h> // For channel layouts
    #include <libavutil/mem.h>           // Specifically for memory
}
std::atomic<double> audio_clock=0.0;

void audio_func(std::string filename){
    AVFormatContext* fmt_ctx=avformat_alloc_context();
    avformat_open_input(&fmt_ctx,filename.c_str(),NULL,NULL);
    avformat_find_stream_info(fmt_ctx,NULL);


    //find best  audio strim

    int audio_stream_idx=av_find_best_stream(fmt_ctx,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
    AVCodecParameters* codec_param=fmt_ctx->streams[audio_stream_idx]->codecpar;

    const AVCodec* codec=avcodec_find_decoder(codec_param->codec_id);

    AVCodecContext* codec_ctx=avcodec_alloc_context3(codec);
    avcodec_parameters_from_context(codec_param,codec_ctx);
    avcodec_open2(codec_ctx,codec,NULL);

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec wanted;
    wanted.freq=44100;
    wanted.format=AUDIO_S16SYS;
    wanted.channels=2;
    wanted.samples=1024;
    wanted.callback=NULL;
    SDL_AudioDeviceID device=SDL_OpenAudioDevice(NULL,0,&wanted,NULL,0);
    SDL_PauseAudioDevice(device, 0);


    /// Now we use the resampler to make sure we have 44.1kHz audio

    SwrContext* swr = nullptr;
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, 2); // Set to Stereo

    // Using swr_alloc_set_opts2 (the modern replacement)
    swr_alloc_set_opts2(&swr,
        &out_ch_layout, AV_SAMPLE_FMT_S16, 44100,      // Output
        &codec_ctx->ch_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate, // Input
        0, NULL);
    swr_init(swr);
    AVPacket* packet=av_packet_alloc();
    AVFrame* frame=av_frame_alloc();
    uint8_t* output=(uint8_t*)av_malloc(192000);

    while(av_read_frame(fmt_ctx,packet)>=0){
        if(packet->stream_index==audio_stream_idx){
            avcodec_send_packet(codec_ctx,packet);

            while(avcodec_receive_frame(codec_ctx,frame)==0){
                int samples=swr_convert(swr,&output,44100,(const uint8_t**)frame->data, frame->nb_samples);
                int outsize=samples*2*2;//2 channels, 2 bytes
                audio_clock=frame->pts*av_q2d(fmt_ctx->streams[audio_stream_idx]->time_base);

                SDL_QueueAudio(device,output,outsize);
                while (SDL_GetQueuedAudioSize(device) > 44100 * 4) {
                SDL_Delay(10);
            }
            }
        }

        av_packet_unref(packet);
    }
av_free(output);
av_frame_free(&frame);
av_packet_free(&packet);
avcodec_free_context(&codec_ctx);
avformat_close_input(&fmt_ctx);
SDL_CloseAudioDevice(device);
}
