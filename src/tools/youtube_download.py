
# -*- encoding: utf-8 -*-
# Date: 12/Mar/2020
# Author: Steven Huang, Auckland, NZ
# License: MIT License
"""""""""""""""""""""""""""""""""""""""""""""""""""""
Description: Youtube video download program
Requirment: pip install pytube
# Usage case 1: python youtube_download.py https://www.youtube.com/watch?v=xxxx
# Usage case 2: python youtube_download.py https://www.youtube.com/watch?v=xxxx dir
# Usage case 3: python youtube_download.py https://www.youtube.com/watch?v=xxxx .\dir\
"""""""""""""""""""""""""""""""""""""""""""""""""""""

import os
import sys
import re
from pytube import YouTube
from pytube import Playlist


gFilesize = 1
gPercent = 0


def progress_function(chunk, file_handle, bytes_remaining):
    global gPercent
    gPercent = round(((gFilesize - bytes_remaining) * 100.0 / gFilesize), 2)
    print('Total:', gFilesize, '(bytes),', 'Downloaded:', gFilesize - bytes_remaining, '(bytes),',
          'Remain:', bytes_remaining, '(bytes),', gPercent, '% done...', end='')  # 1 line refresh
    print('\r', end='')


def startDownload(video, downPath, prefix):
    global gFilesize, gPercent
    gFilesize = video.filesize
    gPercent = 0

    if downPath.strip():  # create dst folder
        if not os.path.exists(downPath):
            os.makedirs(downPath)

    print('Sart to download:', video, 'Size:', gFilesize)
    if gFilesize > 0:
        video.download(downPath, filename_prefix=prefix)


def downYouTube(url, downPath=None):
    print(url, downPath)
    yt = YouTube(url, on_progress_callback=progress_function)
    print(f'Title: \'{yt.title}\'')

    # yt.streams.all()
    print('Download subtitle, y/n?')
    x = input()
    if x == 'y' or x == 'Y':
        downloadSubtitle(yt, downPath)

    print("-------------available media file list----------------------")
    # progressive: True: contain both audio and video, False: seperate
    result = yt.streams  # .order_by('resolution').desc()
    for i, f in enumerate(result):
        print('ID =', i, f)
    print("-------------available media file list end-------------------")

    length = len(result)
    if length == 0:
        print('No file found!')
        return

    print(f'Find {length} media items from your url.')
    print('Please enter the ID or IDs you want to download(0 1 10...):')

    for id in input().split():
        x = int(id)
        if x >= 0 and x < length:
            startDownload(result[x], downPath, id + '_')
        else:
            print('Warning: Id is error, Id=', x)


def downloadSubtitle(yt, downPath):
    print(yt.captions.all())
    print(yt.captions, type(yt.captions))
    caption = yt.captions.get_by_language_code('en')
    if caption:
        str_cap = caption.generate_srt_captions()
        # print(str_cap)
        subtitles = validateTitle(yt.title) + '.srt'
        writeSubtitles(downPath + subtitles, str_cap)
    else:
        print('subtitle error, not found!')


def writeSubtitles(file, content):
    with open(file, "w", encoding='utf-8') as f:
        f.write(content)


def validateTitle(title):
    rstr = r"[\/\\\:\*\?\"\<\>\|]"
    new_title = re.sub(rstr, "_", title)
    return new_title


def main():
    len_params = len(sys.argv)
    print('Number of parameter:', len_params)
    print('Parameters:', str(sys.argv))

    save_path = "."  # download dst path
    url = ""

    if len_params < 2:
        print("Cmd error, please refer to the usage case listed in the Python file!")
        return

    url = sys.argv[1]
    if len_params > 2:
        save_path = sys.argv[2]

    downYouTube(url, save_path)


if __name__ == "__main__":
    main()

