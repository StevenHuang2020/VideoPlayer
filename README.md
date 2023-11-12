## VideoPlayer
![C++ Version](https://img.shields.io/badge/C++-14-blue)
![VS Version](https://img.shields.io/badge/VS-2019-blue)
![QT Version](https://img.shields.io/badge/QT-5-green)
![FFmpeg Version](https://img.shields.io/badge/FFmpeg--blue)
![OpenCV Version](https://img.shields.io/badge/OpenCV--blue)
![CMake](https://img.shields.io/badge/CMake--blue)

A simple video player based on QT and FFmpeg. This is just a rough work for personal use. Any bugs or suggestions about it are welcome.

Please find the release player [here.](https://github.com/StevenHuang2020/VideoPlayer/releases)

<img src="src/res/player.png" width="320" height="205" />

##### Usage Tips
 - ```Media->Open``` is to open local media file
 - ```Media->Open Youtube``` is to open Youtube url(**The current method of getting stream URL from a YouTube URL relies on Python installation**)
 - ```Media->Open Network Url``` is to open network stream media.
 For example ```http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4```
 - You can use [youtube_download.py](https://github.com/StevenHuang2020/VideoPlayer/tree/main/src/tools/youtube_download.py) to download Youtube video/audios.


##### Compilation
- Requirements

    - [QT](https://doc.qt.io/qt-6/qt-online-installation.html)
    - Visual Studio with ```QT tools extention```
    - CMake

- CMake
```
cd to 'CMakeLists.txt' folder
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -T host=x64 -A x64

//build debug version
cmake --build .

//build release version
cmake --build . --config Release
```

##### History
- v2.2.2, updated to QT6
- v2.2.0, updated FFmpeg lib
- v2.1.2, added Youtube URL Python script parsing
- v2.1.1, change to clang-format
- v2.0.10, updated thread exit control, fixed crash bug;
- v2.0.9, added netowrk url; bug fixed;
- v2.0.8, clickable progressbar; play speed bug fixed; added play list;
- v2.0.7, audio visualization; youtube url open options; menu updates;
- v2.0.3, play speed control; full screen
- v2.0.1, CMake configuration
- v2.0.0, added cv effects by using OpenCV;
- v1.1.8, bugs fixed;
- v1.1.6, app settings; loop play; media info;
- v1.1.5, dxva2 decode support; recent files;
- v1.1.4, play seek bug fixed;
- v1.1.2, 29/04/22
    - play start thread; progressbar; skin;
- v1.1.0, 23/04/22
    - A/V sync control
- v1.0.1, 20/04/22
    - A/V play buffer control
- v1.0.0, 16/04/22
    - initial version

##### References

 - A/V library [FFmpeg](https://www.ffmpeg.org/download.html#build-windows) ,[Dev](https://github.com/BtbN/FFmpeg-Builds/releases) <br/>
 - GUI framework [QT](https://www.qt.io/)
 - QT skins [GitHub](https://github.com/GTRONICK/QSS)
 - Youtube stream-media url [youtube-dl](https://youtube-dl.org/)
 - [OpenCV](https://opencv.org/)
