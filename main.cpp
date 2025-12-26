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
                Vec3b color=frame.at<Vec3b>(x,y);
                int intensity=(color[2]+color[1]+color[0])/3;
                int index=(float)intensity*(density.size()-1)/(float)255;
               buffer+=density[index];
            }
            buffer+='\n';
        }

        std::cout << "\033[H" << buffer << std::flush;
        waitKey(30);
    }
    std::cout<<'\033[25h';
        return 0;

}
