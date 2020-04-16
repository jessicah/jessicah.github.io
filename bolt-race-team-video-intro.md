Creating the Intro Video Scene for BRT
======================================

This is some collected notes for creating the intro video scene, how I made some of the effects.

Tools used:
- `ffmpeg` for generating the black frames data for lightning effect
- Synfig for generating an animated image
- Kdenlive for creating the overall video

./synfig_frames.awk:

    #!/usr/bin/awk -f
    
    BEGIN { print "<param name='amount'><animated type='real'>" }
    
    /:/ {
        split($0, parts, ":")
        print "<waypoint time='" NR "f' before='clamped' after='clamped'>"
        print "<real guid='"; system("uuidgen"); print "' value='" ((99 - parts[2]) / 100) "'/>"
        print "</waypoint>"
    }
    
    END { print "</animated></param>" }

With a video with lightning effects, extract the level of blackness in the video using `ffmpeg`:

    ffprobe -f lavfi -i "movie=lightning_effect.mp4,blackframe=amount=1" -show_entries \
      -of default=nw=1 -loglevel info 2>&1 \
    | grep -o -e 'pblack:[0-9]*' \
    | ./synfig_frames.awk \
    > synfig_animation.xml

With an existing Synfig project, the generated XML fragment can be copied into the saved file (use the uncompressed `.sif` format),
e.g. in my Synfig project, I have the following:

    <param name="canvas">
        <canvas>
            <layer type="import" active="true" exclude_from_rendering="false" version="0.1" desc="some description">
                <param name="z_depth">
                    <real value="0.00000000"/>
                </param>
                <param name="amount">
                    <!-- this is where the output fragment will go -->
                </param>
                <!-- other params, etc. -->
            </layer>
        </canvas>
    </param>

You should have an existing param for `amount` where you can copy the output from `ffprobe`/`awk` into the Synfig
project file.

Synfig doesn't generate a movie file directly, so render to numbered frames using the PNG format. Then we can use `ffmpeg` to
encode this into a video format with an alpha channel:

    ffmpeg -f image2 -framerate 30 -i "exported_frames.0%03d.png" -vcodec qtrle output_with_alpha.mov
 
 Once this is done, you can then import this with your lightning effect track into Kdenlive, and you should see your animation
 flicker in sync with the lightning effects.
