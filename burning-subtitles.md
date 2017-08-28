Burning Subtitles
=================

VobSub Subtitles
----------------

Finding out how to do this was surprisingly difficult, so here's how to burn in a subtitle file in VobSub format (idx & sub):
```
ffmpeg -i <filename>.mkv -i <filename>.idx -filter_complex '[0:v:0][1:s:0]overlay[vid]' \
  -map '[vid]' -map 0:a <output>.mkv
```

Example config for downmixing into something playable by majority of media players (stereo mp3, h264 video):
```
ffmpeg -i <filename>.mkv -i <filename>.idx -filter_complex '[0:v:0][1:s:0]overlay[vid]' -map '[vid]' -map 0:a \
  -ac 2 -acodec mp3 -vcodec h264 <output>.mp4
```

Embedded SRT Subtitles
----------------------

I haven't figured out how to do this without extracting the SRT tracks, so I've separated into two separate steps:
```
ffmpeg -i <filename>.mkv -map 0:4 <filename>.srt
ffmpeg -i <filename>.mkv -filter:v subtitles=<filename>.srt -map 0:v -map 0:a:0 -ac 2 -acodec mp3 -vcodec h264 <filename>.mp4
```

Things to note: in this example, I have two audio tracks, so `-map 0:a:0` selects the first audio track. I also have multiple
subtitle tracks, so `-map 0:4` when extracting selects the 4th track in the video. Note that this is the 4th overall track,
not the 4th subtitle track. I suppose `-map 0:s:1` may also work, which would be the second subtitle track.
