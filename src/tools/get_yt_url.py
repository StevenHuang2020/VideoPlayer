
# -*- encoding: utf-8 -*-
# Date: 22/Oct/2023
# Author: Steven Huang, Auckland, NZ
# License: MIT License
"""""""""""""""""""""""""""""""""""""""""""""""""""""
Description: Get youtube stream url
# https://pytube.io/en/latest/user/streams.html
#
# This module is used to parse YouTube URLs,
# dump the data to a temporary JSON file, and
# return the JSON file path to the caller.
#
"""""""""""""""""""""""""""""""""""""""""""""""""""""
import sys
import os
import uuid
import json
from pytube import YouTube, Stream
import tempfile


def write_to_file(file, content):
    with open(file, 'wb') as f:
        f.write(content)


def print_youtube(yt: YouTube):
    print_header(yt)
    print('')
    print_streams(yt.streams)


def print_header(yt: YouTube):
    print("-------------Youtube url header info----------------------")
    # print(f'watch_html: {yt.watch_html}')
    print(f'title: {yt.title}')
    print(f'age_restricted: {yt.age_restricted}')
    # print(f'initial_data: {yt.initial_data}')
    # print(f'streaming_data: {yt.streaming_data}')
    # print(f'fmt_streams: {yt.fmt_streams}')
    # print(f'vid_info: {yt.vid_info}')
    # print(f'caption_tracks: {yt.caption_tracks}')
    print(f'description: {yt.description}')
    print(f'rating: {yt.rating}')
    print(f'length: {yt.length}')
    print(f'views: {yt.views}')
    print(f'author: {yt.author}')
    print(f'keywords: {yt.keywords}')
    print(f'channel_id: {yt.channel_id}')
    print(f'channel_url: {yt.channel_url}')
    print(f'metadata: {yt.metadata}')
    print(f'captions: {yt.captions}')
    # print(f'streams: {yt.streams}')
    print(f'thumbnail_url: {yt.thumbnail_url}')
    print(f'publish_date: {yt.publish_date}')
    print("-------------Youtube url header info end------------------")


def print_streams(streams):
    print("-------------available media list----------------------")
    for i, f in enumerate(streams):
        # print('ID =', i, f, f.url)
        print('ID =', i, f)
    print("-------------available media list end-------------------")


def yt_header(yt: YouTube, data: dict):
    data['title'] = yt.title
    data['description'] = yt.description
    data['rating'] = yt.rating
    data['length'] = yt.length
    data['views'] = yt.views
    data['author'] = yt.author
    data['keywords'] = yt.keywords
    data['thumbnail_url'] = yt.thumbnail_url
    data['publish_date'] = yt.publish_date.strftime("%Y-%m-%d, %H:%M:%S")
    # data['captions'] = yt.captions
    data['captions_length'] = len(yt.captions)


def dict_stream(id: int, st: Stream):
    dict_st = {}
    dict_st['id'] = id

    dict_st['progressive'] = st.is_progressive
    dict_st['have_audio'] = st.includes_audio_track
    dict_st['have_video'] = st.includes_video_track

    dict_st['type'] = st.type
    dict_st['mime_type'] = st.mime_type

    if st.includes_video_track:
        dict_st['resolution'] = st.resolution
        dict_st['fps'] = st.fps
        dict_st['video_codec'] = st.video_codec
        if not st.is_adaptive:
            dict_st['audio_codec'] = st.audio_codec
    else:
        dict_st['abr'] = st.abr
        dict_st['audio_codec'] = st.audio_codec

    dict_st['filesize'] = st.filesize
    # dict_st['title'] = st.title
    dict_st['expiration'] = st.expiration.strftime("%Y-%m-%d, %H:%M:%S")
    dict_st['default_filename'] = st.default_filename
    dict_st['url'] = st.url
    return dict_st


def get_streams(url: str):
    yt = YouTube(url)
    # print_youtube(yt)

    json_data = {}
    yt_header(yt, json_data)

    json_data['streams'] = []
    # result = yt.streams.filter(progressive=True).order_by('resolution').desc()
    result = yt.streams
    for i, f in enumerate(result):
        # print(i, f)
        json_data['streams'].append(dict_stream(i, f))  # f.url

    return write_to_tmp(json.dumps(json_data, indent=2).encode('utf-8'))


def create_path(dirs):
    if not os.path.exists(dirs):
        os.makedirs(dirs)


def join_path(dirs, sub):
    return os.path.join(dirs, sub)


def write_tmp(content):
    with tempfile.NamedTemporaryFile(delete=False) as fp:
        fp.write(content)
    return fp.name


def write_to_tmp(content: dict):
    path = join_path(tempfile.gettempdir(), 'videoplayer')
    # print('path: ', path)
    create_path(path)

    basename = "youtube"
    # suffix = datetime.datetime.now().strftime("%y%m%d_%H%M%S")
    suffix = uuid.uuid4().hex
    # e.g. 'mylogfile_120508171442'
    file = join_path(path, "_".join([basename, suffix]))

    write_to_file(file, content)
    return file


def main():
    """ main function """
    len_params = len(sys.argv)
    print('len_params: ', len_params)
    if len_params < 2:
        print('parameter error!')
        return

    url = sys.argv[1]
    print(get_streams(url))


if __name__ == "__main__":
    main()
