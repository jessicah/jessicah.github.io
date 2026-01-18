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

## MDN Full Example (WebM) [3]

```
# Generate multiple variations
ffmpeg -i in.video -c:v libvpx-vp9 -keyint_min 150 \
-g 150 -tile-columns 4 -frame-parallel 1 -f webm -dash 1 \
-an -vf scale=160:90 -b:v 250k -dash 1 video_160x90_250k.webm \
-an -vf scale=320:180 -b:v 500k -dash 1 video_320x180_500k.webm \
-an -vf scale=640:360 -b:v 750k -dash 1 video_640x360_750k.webm \
-an -vf scale=640:360 -b:v 1000k -dash 1 video_640x360_1000k.webm \
-an -vf scale=1280:720 -b:v 1500k -dash 1 video_1280x720_1500k.webm

# Generate MPEG-DASH (WebM & Byte Ranges)
ffmpeg \
  -f webm_dash_manifest -i video_160x90_250k.webm \
  -f webm_dash_manifest -i video_320x180_500k.webm \
  -f webm_dash_manifest -i video_640x360_750k.webm \
  -f webm_dash_manifest -i video_1280x720_1500k.webm \
  -f webm_dash_manifest -i my_audio.webm \
  -c copy \
  -map 0 -map 1 -map 2 -map 3 -map 4 \
  -f webm_dash_manifest \
  -adaptation_sets "id=0,streams=0,1,2,3 id=1,streams=4" \
  my_video_manifest.mpd
```

The `-map` arguments correspond to the input files in the sequence they are given; you should have one for each file. The `-adaptation_sets` argument assigns them into adaptation sets; for example, this creates one set (0) that contains the streams 0, 1, 2 and 3 (the videos), and another set (1) that contains only stream 4, the audio stream.

Put the manifest and the associated video files on your web server or CDN. DASH works via HTTP, so as long as your HTTP server supports byte range requests, and it's set up to serve `.mpd` files with `Content-Type: application/dash+xml`, then you're all set.

Then, in order to correctly connect this `.mpd` file to your `<video>` element, you need a JavaScript library like dash.js, because no browser has native support for DASH. Read dash.js quickstart [4] for how to set up your page to use it.

## Blazor and Dash.js

Using Blazored.Video, it's pretty straight-forward, with automatic handling of MPEG-DASH video
elements. It also requires setting up static file handling, using `ServeUnknownFileTypes` set
to true to make it super easy, but this can be more explicit if desired.

```cshtml
<BlazoredVideo @ref="video" data-dashjs-player></BlazoredVideo>

<script src="https://cdn.dashjs.org/v4.0.0/modern/umd/dash.all.debug.js"></script>

@code {
  Blazored.Video.BlazoredVideo video = null!;

  protected override async Task OnAfterRenderAsync(bool firstRender)
  {
    if (!firstRender)
      return;

    try
    {
      await video.SetSrcAsync("/dash/example.mpd");
    }
    catch (Exception exn)
    {
      // Problem setting the video source...
    }
  }
}
```

And in `Program.cs`:
```cs
public static void Main(string[] args)
{
  var builder = WebApplication.CreateBuilder(args);

  // ...

  var app = builder.Build();

  // ...

  app.UseStaticFiles(new StaticFileOptions
  {
    FileProvider = new PhysicalFileProvider(builder.Environment.ContentRootPath + "/wwwroot/dash"),
    RequestPath = "/dash",
    ServeUnknownFileTypes = true,
  });

  // ...
}
```

## Hosting with IIS

See https://github.com/iamgmujtaba/dash-server

---

See FFMPEG docs [2] for more information about various DASH options.

[1]: https://video.stackexchange.com/questions/37666/generate-hls-streams-from-pre-encoded-videos-without-re-encoding-using-ffmpeg
[2]: https://ffmpeg.org/ffmpeg-formats.html#dash
[3]: https://developer.mozilla.org/en-US/docs/Web/API/Media_Source_Extensions_API/DASH_Adaptive_Streaming
[4]: https://dashif.org/dash.js/pages/quickstart/
