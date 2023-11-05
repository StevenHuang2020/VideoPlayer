
# -*- encoding: utf-8 -*-
# Date: 22/Oct/2023
# Author: Steven Huang, Auckland, NZ
# License: MIT License
"""""""""""""""""""""""""""""""""""""""""""""""""""""
Description: Get youtube stream url
# https://pytube.io/en/latest/user/streams.html
"""""""""""""""""""""""""""""""""""""""""""""""""""""
import sys
from pytube import YouTube


def get_subtitle(yt, downPath=None):
    print(yt.captions.all())
    # print(yt.captions, type(yt.captions))
    caption = yt.captions['en']
    if caption:
        str_cap = caption.generate_srt_captions()
        print('str_cap=', str_cap)
        # subtitles = validateTitle(yt.title) + '.srt'
        # writeSubtitles(downPath + subtitles, str_cap)
    else:
        print('subtitle error, not found!')


def get_stream_url(url, opt_id=0, all=False):
    opt = ["best", "worst", "bestvideo", "worstvideo", "bestaudio", "worstaudio"]

    yt = YouTube(url)

    # print(f'Title: \'{yt.title}\'')
    only_audio = False
    if opt_id > 3 and opt_id <= 5:
        only_audio = True

    only_video = False
    if opt_id >= 2 and opt_id <= 3:
        only_video = True

    print("-------------available media file list----------------------")
    # progressive: True: contain both audio and video, False: seperate

    if all:
        result = yt.streams
    if only_audio:
        result = yt.streams.filter(only_audio=True).order_by('abr').desc()
    elif only_video:
        result = yt.streams.filter(only_video=True).order_by('resolution').desc()
    else:
        result = yt.streams.filter(progressive=True).order_by('resolution').desc()

    for i, f in enumerate(result):
        print('ID =', i, f)
    print("-------------available media file list end-------------------")

    length = len(result)
    if length == 0:
        print('No file found!')
        return None

    id = 0
    if opt_id % 2 != 0:
        id = length - 1
    print('selected: ', id)
    return result[id].url


def main():
    """ main function """
    len_params = len(sys.argv)
    print('len_params: ', len_params)
    if len_params < 3:
        print('parameter error!')
        return
    print(get_stream_url(sys.argv[1], int(sys.argv[2])))


if __name__ == "__main__":
    main()
