# Creating Track Matte Stinger in After Effects

Unfortunately, at https://obsproject.com/kb/track-matte-stinger-transitions, it
fails to mention that the stinger half needs to be on a transparent background.

## Creating the Stinger

When creating a track matte stinger, as documented, you need a canvas that is
twice as wide (or twice as tall) as your target video resolution. In the case of
1080p, this is 3840x1080.

In order to be able to see the transparency in After Effects, you need to toggle
the transparency grid. You can also set the background colour in the Composition
Settings, if you don't want to use the transparency grid, or the transparency
grid isn't clear enough.

![image](https://github.com/jessicah/jessicah.github.io/assets/274082/3497cf0e-fbbd-43f3-8b9a-23527dff3148)

![image](https://github.com/jessicah/jessicah.github.io/assets/274082/37f25ab8-4867-42d9-83ae-aa2dd1872357)

The rest of the stinger creation follows the steps from the OBS KB.

## Keyframe Animation

This one had me confused. The first step is to identify the properties you want to
animate. You need to click on the white "empty" stopwatch icon to highlight in blue.
Then use the timeline scrubber to seek to where you want to update the property, and
set the values. After Effects will automatically add keyframes on each property value
change.

![image](https://github.com/jessicah/jessicah.github.io/assets/274082/7d843894-0288-4248-b94a-a990b218e951)

**Warning**: If you click the highlighted stopwatch, this will erase all your keyframes!

## Exporting the Video

Once you're ready to export your video, File > Export > Add to Render Queue, and use
the following encoder settings:

- Format: QuickTime
- Channels: RGB + Alpha
- Format Options: Animation (defaults to this)

The other options can be left as default, and this produces a video in MOV
format that can be directly imported into OBS for the stinger transition.

## Some Useful Tutorials on Path Animations

- https://www.youtube.com/watch?v=Zzf4wnY6_Bs
- https://www.youtube.com/watch?v=H1xNSFfQALk

# Using Stinger in OBS

Settings:

- Audio Transition Point Type: Time
- Transition Point: Video length in milliseconds
- Use a Track Matte: Yes
- Matte Layout: Same file, side-by-side (stinger on the left, track matte on the right)

Optional:

- Audio Fade Style: Crossfade
