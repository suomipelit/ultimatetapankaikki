Ultimate Tapan Kaikki
=====================

[![Build Status](https://api.travis-ci.org/suomipelit/ultimatetapankaikki.svg?branch=master)](https://travis-ci.org/suomipelit/ultimatetapankaikki)
[![Downloads](https://img.shields.io/github/downloads/suomipelit/ultimatetapankaikki/total.svg)](https://github.com/suomipelit/ultimatetapankaikki/releases)
[![Latest release](http://img.shields.io/github/release/suomipelit/ultimatetapankaikki.svg)](https://github.com/suomipelit/ultimatetapankaikki/releases/latest)

![Ultimate Tapan Kaikki GIF](https://github.com/suomipelit/suomipelit.github.io/blob/master/gifs/tk321.gif)

[**Try live browser version here!**](https://suomipelit.github.io/ultimatetapankaikki-web/)

:point_right: **The original README.TXT can be found as [README.ORIG.TXT](./README.ORIG.TXT).**

This is the legendary 90s game Ultimate Tapan Kaikki, ported to
SDL2. It runs at least on macOS, Linux, FreeBSD and Windows.

The port is maintained by the [Suomipelit][suomipelit-gh] organization,
whose mission is to archive, port, and maintain classic Finnish freeware and
shareware games.  Feel free to [join the Suomipelit Slack][suomipelit-slack]
too!

Key bindings
------------

Player 1 and 2 game controls can be customized under `OPTIONS ->
PLAYER OPTIONS -> DEFINE KEYS 1/2`.

Game controllers are also supported but buttons need to be configured
in options before playing. No more than two controllers are recognized
at the same time. Analog inputs or navigating in menus is not
currently supported.

Other than player keys are hard-coded.

**Note:** Some Apple keyboards don't have a right ctrl key. If you
have such a keyboard, you have to change the player 1 shoot key before
playing.

**Moving in menus**

| Key | Description |
| --- | --- |
| Up/down arrow | Navigate |
| Return | Select |
| Esc | Go back |

**Player 1 game controls**

| Key | Description |
| --- | --- |
| Up arrow | Move forward |
| Down arrow | Move backward |
| Left arrow | Turn counter clockwise |
| Right arrow | Turn clockwise |
| Right ctrl | Shoot |
| Right shift | Change weapon |
| Right alt | Strafe |
| `,` | Starfe left |
| `.` | Starfe right |

When playing solo, you can use grave (left of `1`), `1` ... `9`, `0`,
and minus (right of `0`) to select the corresponding weapon. Grave
selects fist, `1` selects pistola, `2` selects shotgun, etc.

**Player 2 game controls**

| Key | Description |
| --- | --- |
| `w` | Move forward |
| `s` | Move backward |
| `a` | Turn counter clockwise |
| `d` | Turn clockwise |
| Tab | Shoot |
| Caps Lock | Change weapon |
| Grave | Strafe |
| `1` | Starfe left |
| `2` | Starfe right |

**Miscellaneous game controls**

| Key | Description |
| --- | --- |
| Esc, then `y` | Quit game |
| Space | Toggle map |
| F5 | Toggle frame rate |
| F12 | Capture screen shot |
| Left alt + Return | Toggle full screen |
| Return | Text chat (multiplayer only) |

**Shop controls**

| Key | Description |
| --- | --- |
| Arrow keys | Switch between items |
| Shoot key | Buy |
| Switch weapon key | Sell |
| Esc, then shoot key | Exit shop |
| Shift + Esc | Exit game immediately |


Differences compared to the original game
-----------------------------------------

This port is based on hkroger's published original sources at
https://github.com/hkroger/ultimatetapankaikki. Those sources are for
**v1.2 beta**, whereas the latest published version of the game is
**v1.21**, also called **TK321** (the released zipfile's name was
`TK321.zip`).

We've fixed all the bugs and differences between v1.2 beta and v1.21.

Changes required to port the game:

- When starting the game, it runs in a window. You can change to full
  screen and with left alt + return.

- Networking on native versions is now based on TCP/IP, originally it
  used IPX. When joining a network game, there's a new screen that
  allows typing the server address. Port is 8099.

- Networking on web version is based on WebRTC. An ID is printed to
  the console window when creating a server. You can share it with
  your friends.

- In-game screenshots are saved in BMP format as opposed to original
  PCX format.

New features:

- Pressing Esc in episode selection menu goes back to main menu.

- Pressing Shift + Esc anywhere in the game quits immediately.

- Game controller support.

- Setting the environment variable `TK_MUTE_MUSIC` mutes all music.
  Useful if you like your Tapan Kaikki with your favorite mixtape.

- You can use the `-e` command line parameter to immediately jump
  into single-player game in the given episode.  This works with
  the `-l` switch too.  Mostly useful for development, of course.

- Setting the environment variable `TK_SKIP_LEVEL_INFO` will skip the
  screen that appears at the start of each level.  Useful for
  impatient developers.

- Name is not cleared if pressing ESC in name prompt.

- Player name is prompted if starting or joining to network game with
  default name.

- Text chat in multiplayer games with `Return` key.

- Text input fields support copy-paste (`Ctrl + c/v`)

- Number of players on a server is shown on server list

Bugfixes:

- Many memory leaks and buffer overflows have been fixed.

- Many places in menus caused a high CPU load because of busy loops
  waiting for something. These have been fixed.

- Fixed player name changing to only accepts a-z and 0-9, space and
  `.`. Originally it accepted any keypress and produced a space for
  characters not available in the font.

- Clients now monitor server connection and quit to main menu if the
  server crashes.

- Player names and colors were mixed up in split screen mode.

- Empty player name is not allowed.

Building from source
--------------------

**Requirements:**

- CMake
- C++ compiler: At least gcc, clang and Visual Studio are supported
- Libraries: SDL2, SDL2_mixer, SDL2_image, SDL2_net
  - On macOS, you can install these with Homebrew. `brew install sdl2 sdl2_mixer sdl2_image sdl2_net`
  - On Windows you can download these from SDL website

**Building:**

```shell
cmake .
cmake --build .
```

On Window, this produces `GAME.EXE` which you can run. On other
systems, the name of the executable is `tk3`.

On Windows, you may need to explicitly specify paths to your SDL libraries, like
```shell
cmake -DSDL2_PATH="C:\\<path>\\SDL2-2.0.9" -DSDL2_MIXER_PATH="C:\\<path>\\SDL2_mixer-2.0.4" -DSDL2_IMAGE_PATH="C:\\<path>\\SDL2_image-2.0.4" -DSDL2_NET_PATH="C:\\<path>\\SDL2_net-2.0.1" .
```
which produces project files for 32-bit target. For 64-bit target, use e.g. `cmake -G "Visual Studio 15 2017 Win64"`.

***On browser:***

Emscripten JavaScript/WebAssembly build for browsers is also
supported. You can try it live
[here](https://suomipelit.github.io/ultimatetapankaikki-web/).

``` shell
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=<path to>/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake .
cmake --build .
```

For best performance it is recommended to build with
`-DCMAKE_BUILD_TYPE=Release`.

It might be easiest to run local HTTP server with Python

``` shell
python -m http.server
```

and opening the game with supported browser at `http://localhost:8000/ultimatetapankaikki.html`.

Building releases
-----------------

* Linux: Run `./pkg/build-deb.sh` to build a .deb (Docker is required)
* Windows: TBD (probably using `cpack` somehow)
* macOS: Run `./pkg/build-macos.sh` to build a .dmg

Releases
--------

**Work in Progress - Version 3 - 2020-MM-DD**

*Network game not compatible with version 2!*

- [Web port](https://suomipelit.github.io/ultimatetapankaikki-web/)
- Network gaming changes
    - Text chat with `Return` key
    - Fixed joining with cooperative mode as active
    - Fixed kill messages
    - Fixed frag count in some cases
    - Fixed missing smoke and spot light effects on clients
    - Increased network code execution frequency
    - Show number of players on server list
    - Reduced keep-alive messages
- Fixed a memory leak in sound resampling
- Fixed minor buffer overflow with sprite loading
- Fixed a local deathmatch crash if playing with enemies
- Fixed empty name input
- Avoid unnecessary screen drawing
- Copy-paste support to text input fields

**[Version 2](https://github.com/suomipelit/ultimatetapankaikki/releases/tag/v2) - 2019-02-04**

- Fixed bug #131:
  Small sprites (crosshair, bomb, mine) were not being visible in the port.
  They worked fine in the original version.

**[Version 1](https://github.com/suomipelit/ultimatetapankaikki/releases/tag/v1) - 2019-01-28**

- Initial release

[suomipelit-gh]: https://github.com/suomipelit
[suomipelit-slack]: https://tinyurl.com/suomipelit-slack
