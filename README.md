# Video2ASCII

This basically converts any video( and even a yt link if you do it right) to an ASCII video.
![alt text](image.png)
# Build the project

```
# clone the repo

git clone git@github.com:AnujJha88/video2ascii.git

# install dependencies
sudo apt update && sudo apt install -y \ 
build-essential \ 
pkg-config \ 
libopencv-dev \ 
libsdl2-dev \ 
libavcodec-dev \ 
libavformat-dev \ 
libswresample-dev \ 
libavutil-dev \ 
yt-dlp

#Make the binary
make

```

# Usage

1. `./ascii_vid <path to video> #if using local files`
2. `./ascii_vid "$(yt-dlp -g -f 'best[ext=mp4]' <youtube-url>)" #if using youtube links`

# TODO
1. Add in easier support for yt links
2. try to make the rendering a bit finer instead of blotchy like it is now


