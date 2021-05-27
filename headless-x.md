Headless X11
============

1. `sudo Xvfb :10 -ac -screen 0 1920x1080x24 &`
2. `DISPLAY=:10 <app>`
3. `x11vnc -display :10`

Starts a virtual X11 server, app renders to virtual X11 display, then we can use `x11vnc` to capture that virtual display and remote in via VNC.

I think there was an alternative of VNC over SSH, I think it was, that is maybe slightly more performant, haven't tried it out yet.

Step 3. becomes `x11vnc -display :10 -bg -nopw -listen localhost -xkb

And then on the local machine:

1. PuTTY tunneling 5900
2. vncviewer localhost:5900

Wikipedia suggests limiting encodings to 'copyrect, tight, zrle, hextile'
