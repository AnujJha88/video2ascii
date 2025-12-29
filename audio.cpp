#include <iostream>
#include <atomic>
#include <string>
#include <SDL2/SDL.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/channel_layout.h>
}

std::atomic<double> audio_clock;
extern std::atomic<bool> keep_running;
std::atomic<bool> has_audio=false;

void audio_func(std::string filename) {
    // 1. Initialize Pointers to NULL
    AVFormatContext* fmt_ctx = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    SwrContext* swr = nullptr;
    AVDictionary* opts=nullptr;

    av_dict_set(&opts,"http_persistent","0",0);
    // 2. Open File
    if (avformat_open_input(&fmt_ctx, filename.c_str(), NULL, &opts) < 0) {
        std::cerr << "ERR: Could not open file: " << filename << std::endl;
        av_dict_free(&opts);
        return;
    }
    av_dict_free(&opts);
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        std::cerr << "ERR: Could not find stream info" << std::endl;
        return;
    }

    // 3. Find Audio Stream
    int audio_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_idx < 0) {
        std::cerr << "ERR: Could not find audio stream in " << filename << std::endl;
        has_audio.store(false);
        return;
    }

    // 4. Setup Decoder
    AVCodecParameters* codec_par = fmt_ctx->streams[audio_stream_idx]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_par->codec_id);
    if (!codec) {
        std::cerr << "ERR: Unsupported codec!" << std::endl;
        return;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_par);
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        std::cerr << "ERR: Could not open codec" << std::endl;
        return;
    }

    // 5. Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "ERR: SDL Init failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_AudioSpec wanted, actual;
    SDL_zero(wanted);
    wanted.freq = 44100;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 2;
    wanted.samples = 1024;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &wanted, &actual, 0);
    if (dev == 0) {
        std::cerr << "ERR: SDL_OpenAudioDevice failed: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_PauseAudioDevice(dev, 0);

    // 6. Setup Resampler (Modern API)
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, 2);

    // Safety check for older FFmpeg versions
    if (swr_alloc_set_opts2(&swr, &out_ch_layout, AV_SAMPLE_FMT_S16, 44100,
                            &codec_ctx->ch_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate,
                            0, NULL) < 0) {
        std::cerr << "ERR: Could not initialize resampler" << std::endl;
        return;
    }
    swr_init(swr);

    // 7. Loop
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    uint8_t* buffer = (uint8_t*)av_malloc(192000);

    while (keep_running&& av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index == audio_stream_idx) {
            if (avcodec_send_packet(codec_ctx, packet) == 0) {
                while (keep_running&& avcodec_receive_frame(codec_ctx, frame) == 0) {
                    int samples = swr_convert(swr, &buffer, 44100, (const uint8_t**)frame->data, frame->nb_samples);
                    has_audio.store(true);
                    // Update sync clock
                    audio_clock.store(frame->pts * av_q2d(fmt_ctx->streams[audio_stream_idx]->time_base));

                    SDL_QueueAudio(dev, buffer, samples * 2 * 2);
                    while (keep_running&&SDL_GetQueuedAudioSize(dev) > 44100 * 4) SDL_Delay(10);
                }
            }
        }
        av_packet_unref(packet);
    }

    // 8. Cleanup
    av_free(buffer);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
}
