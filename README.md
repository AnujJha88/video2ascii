# Video2ASCII

This basically converts any video( and even a yt link if you do it right) to an ASCII video.

# Build the project

1. clone the repo
`git clone git@github.com/AnujJha88/video2ascii`
2. `cd video2ascii`
3. `sudo apt update && sudo apt install -y \
    build-essential \
    pkg-config \
    libopencv-dev \
    libsdl2-dev \
    libavcodec-dev \
    libavformat-dev \
    libswresample-dev \
    libavutil-dev \
    yt-dlp`

4. `make`

# Usage

1. `./ascii_vid <path to video> #if using local files`
2. `./ascii_vid "$(yt-dlp -g -f 'best[ext=mp4]' https://www.youtube.com/watch?v=VIDEO_ID)" #if using youtube links`

# TODO
1. Add in better rendering for more resolution
2. Add in easier support for yt links


