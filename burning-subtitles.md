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
