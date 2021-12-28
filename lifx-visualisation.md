# LIFX Visualisation

Using https://github.com/rando-calrissian/Vela for controlling the LIFX for music visualisation.

## Installation

Using the Anaconda3 installation from my Visual Studio Community installation.

Launch an Anaconda Prompt from the Start Menu.

Download file: https://download.lfd.uci.edu/pythonlibs/x6hvwk7i/PyAudio-0.2.11-cp39-cp39-win_amd64.whl

Note: the cp<x><y> part needs to match your python version, in this case, the above, `cp39` is for python3.9.

```
conda create -n visualiser
activate visualiser
conda install numpy scipy pyqtgraph
pip install path\to\PyAudio-0.2.11-cp39-cp39-win_amd64.whl
pip install paho-mqtt pyaudio pillow pywin32 comtypes cffi lifxlan
python C:\ProgramData\Anaconda3\Scripts\pywin32_postinstall.py install
```

Note: `win32gui` is dropped from the install compared to the project release notes, as `pywin32` comes
with `win32gui` anyway.

```
git clone https://github.com/rando-calrissian/Vela
```

You'll need to edit `Vela\visualizer\python\lib\config.py` to suit your system. I recommend going to their
GitHub page to view the full README.

Then you can `cd Vela\visualizer\python && python main.py` to run the program.
