#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
extern std::atomic<double> audio_clock;
extern std::atomic<bool> keep_running;
extern std::atomic<bool> has_audio;

using namespace cv;
void video_func(std::string filename)
{

    std::ios_base::sync_with_stdio(false); // faster cout
    std::string density = " .:-=+*#%@";
    VideoCapture cap(filename);
    int width, height;
    if (cap.isOpened())
    {
        height = cap.get(CAP_PROP_FRAME_HEIGHT);
        width = cap.get(CAP_PROP_FRAME_WIDTH);

        std::cout << "width: " << width << " height: " << height << std::endl;
    }
    struct winsize ws;

    Mat frame;
    std::cout << '\033[25l';
    int screen_width, screen_height;

    double fps = cap.get(CAP_PROP_FPS);
    if (fps <= 0)
        fps = 30;
    double frame_time = 1000.0 / fps;
    Vec3b last_pixel = Vec3b(0, 0, 0);
    bool first_pixel = true;

    while (keep_running&&cap.read(frame))
    {

        auto start = std::chrono::steady_clock::now();

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        float aspect_ratio = (float)width / (float)height;
        screen_width = ws.ws_col;
        screen_height = screen_width / aspect_ratio;
        screen_height *= 0.5;
        if (screen_height >= ws.ws_row)
            screen_height = ws.ws_row - 1;
        // cvtColor(frame,frame,COLOR_BGR2GRAY);
        resize(frame, frame, Size(screen_width, screen_height), 0, 0, INTER_AREA);
        std::string buffer;

        double video_pts=cap.get(CAP_PROP_POS_MSEC)/1000.0;
        if(has_audio.load()){
        while(keep_running&& video_pts>audio_clock.load()){
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        if(!keep_running)break;
        if (video_pts < audio_clock.load() - 0.1) {
            continue;
        }
        }
        for (int x = 0; x < frame.rows; x++)
        {
            for (int y = 0; y < frame.cols; y++)
            {
                Vec3b pixel = frame.at<Vec3b>(x, y);
                int r_val = pixel[2]; // Red
                int g_val = pixel[1]; // Green
                int b_val = pixel[0]; // Blue
                if (first_pixel || pixel[0] != last_pixel[0] ||
                    pixel[1] != last_pixel[1] || pixel[2] != last_pixel[2])
                {

                    buffer += "\033[38;2;" + std::to_string(r_val) + ";" + std::to_string(g_val) + ";" + std::to_string(b_val) + "m";

                    last_pixel = pixel;
                    first_pixel = false;
                }
                int intensity = (pixel[2] + pixel[1] + pixel[0]) / 3;
                int index = (float)intensity * (density.size() - 1) / (float)255;
                buffer += density[index];
            }

            buffer += "\033[0m\n";
            first_pixel = true;
        }
        std::cout << "\033[H" << buffer << std::flush;
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        if (elapsed < frame_time)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(frame_time - elapsed)));
        }
        waitKey(1);
    }
    std::cout << '\033[25h';
}
