#include<thread>
#include<string>
#include<iostream>

void audio_func(std::string filename);
void video_func(std::string filename);

int main(int argc, char** argv){
    if(argc<2){
        std::cout << "Usage: ./ascii_vid <video_file>" << std::endl;
        return 1;
    }

    std::string filename=argv[1];

    std::thread audio_thread(audio_func, filename);
    std::thread video_thread(video_func, filename);

    audio_thread.join();
    video_thread.join();

    return 0;
}
