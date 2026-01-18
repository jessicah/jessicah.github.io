# Notes on creating an MPEG-DASH stream with FFMPEG

## Basics

At the very simplest, to convert a file into a single format stream:

```
ffmpeg -i <input-file> -map 0:v -map 0:a -c:v copy -c:a copy -f dash <output-file>.mpd
```

Also, an example with multiple mappings [1]: 

```
# Converting source to a given quality for merging into single HLS/DASH stream
ffmpeg -y -i <input-file> -c:v libx264 -crf <bitrate> -preset veryfast -vsync 0 -bf 0 -x264-params scenecut=0:keyint=25:min-keyint=25 -c:a aac -ab 128k -f mp4 <output-file>.mp4

# Multiple inputs into single HLS stream, should be similar for DASH
ffmpeg -i low.mp4 -i mid.mp4 -i high.mp4 \
-map 0:v -map 0:a -map 1:v -map 1:a -map 2:v -map 2:a \
-c:v copy -c:a copy \
-hls_time 4 -hls_playlist_type vod -hls_segment_type fmp4 \
-hls_flags independent_segments \
-var_stream_map "v:0,a:0,name:low v:1,a:1,name:mid v:2,a:2,name:high" \
-master_pl_name master.m3u8 \
-hls_segment_filename '%v/segment-%06d.m4s' \
-hls_fmp4_init_filename '%v/init.mp4' \
-strftime_mkdir 1 \
-f hls '%v.m3u8'
```

## Futher Options

Customising the generated filenames can be done with various options:

- `media_seg_name`, which defaults to `chunk-stream$RepresentationID$-$Number%05d$.$ext$`
- `init_seg_name`, which defaults to `init-stream$RepresentationID$.$ext$`

Another possible option, that relies on HTTP byte ranges, is single file mode:

- Set `single_file` to true
- Set `single_file_name` to output filename

Not sure if this is any better from a hosting point of view, or many small files...

See FFMPEG docs [2] for more information about various DASH options.

[1]: https://video.stackexchange.com/questions/37666/generate-hls-streams-from-pre-encoded-videos-without-re-encoding-using-ffmpeg
[2]: https://ffmpeg.org/ffmpeg-formats.html#dash
