#include <asm-generic/ioctls.h>
#include<opencv2/core.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
#include<sys/ioctl.h>
#include<unistd.h>
#include <string>
using namespace cv;

int main(){
    std::string filename;
    std::ios_base::sync_with_stdio(false);//faster cout
    std::cin>>filename;
    std::string density=" .:-=+*#%@";
    VideoCapture cap(filename);
    int width,height;
    if(cap.isOpened()){
   height=cap.get(CAP_PROP_FRAME_HEIGHT);
   width=cap.get(CAP_PROP_FRAME_WIDTH);

         std::cout<<"width: "<<width<<" height: "<<height<<std::endl;
    }
       struct winsize ws;

    Mat frame;
    std::cout<<'\033[25l';
    int screen_width,screen_height;
    while(cap.read(frame)){
   ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws);
    float aspect_ratio=(float)width/(float)height;
    screen_width=ws.ws_col;
    screen_height=screen_width/aspect_ratio;
        screen_height*=0.5;
      if (screen_height >= ws.ws_row) screen_height = ws.ws_row - 1;
       // cvtColor(frame,frame,COLOR_BGR2GRAY);
        resize(frame,frame,Size(screen_width,screen_height),0,0,INTER_AREA);
        std::string buffer;
        for(int x=0;x<frame.rows;x++){
            for(int y=0;y<frame.cols;y++){
                Vec3b pixel=frame.at<Vec3b>(x,y);
                int r_val = pixel[2]; // Red
                int g_val = pixel[1]; // Green
                int b_val = pixel[0]; // Blue

                buffer += "\033[38;2;" + std::to_string(r_val) + ";"
                       + std::to_string(g_val) + ";"
                       + std::to_string(b_val) + "m";
                int intensity=(pixel[2]+pixel[1]+pixel[0])/3;
                int index=(float)intensity*(density.size()-1)/(float)255;
               buffer+=density[index];
            }
            buffer+='\n';
        }
        int fps=cap.get(CAP_PROP_FPS);
        std::cout << "\033[H" << buffer << std::flush;
        waitKey((float)1000/fps*5);
    }
    std::cout<<'\033[25h';
        return 0;

}
