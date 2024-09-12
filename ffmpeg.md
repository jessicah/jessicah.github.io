# Transcoding with FFMPEG and NVENC

Full details on [using FFMPEG with NVIDIA GPUs](https://docs.nvidia.com/video-technologies/video-codec-sdk/11.1/ffmpeg-with-nvidia-gpu/index.html).

For my usecase, the following seems sufficient (to 720p30 at 2Mbit/s):

`ffmpeg -hwaccel cuda -hwaccel_output_format cuda -resize 1280x720 -i {} -c:a copy -c:v hevc_nvenc -r 30 -b:v 2M {}`

Where `-i {}` is the input file, and the final `{}` is the output file.

If the source file is encoded with H264/H265, it is possible to also use hardware accelerated decode:

`ffmpeg -hwaccel cuvid -c:v hevc_cuvid -i {} -c:a copy -c:v hevc_nvenc -r 30 -b:v 2M {} -hwaccel_output_format cuda`

Where `av1_cuvid` is for AV1, `hevc_cuvid` is for H265, and `h264_cuvid` is for H264. See `ffmpeg -decoders` for the list of all available NVIDIA hardware decoders (`cuvid`).

You'll also need to make sure that you have an `ffmpeg` with hardware decoding enabled.

An appropriate `ffmpeg` can be downloaded at: https://www.gyan.dev/ffmpeg/builds/
