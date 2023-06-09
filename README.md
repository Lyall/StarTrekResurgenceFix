# Star Trek Resurgence Fix
[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W01UAI9)</br>
[![Github All Releases](https://img.shields.io/github/downloads/Lyall/StarTrekResurgenceFix/total.svg)](https://github.com/Lyall/StarTrekResurgenceFix/releases)

This is a fix for Star Trek Resurgence that adds support for custom resolutions/aspect ratios, uncapped framerate and more.

## Features
- Arbitrary aspect ratio support.
- Removes pillarboxing/letterboxing in gameplay and cutscenes.
- Correct FOV scaling no matter what resolution you use.
- Disables 62FPS cap.
- Toggle for depth of field.

## Extra Ini Tweaks
- See [extras.md](extras.md).

## Installation
- Grab the latest release of StarTrekResurgenceFix from [here.](https://github.com/Lyall/StarTrekResurgenceFix/releases)
- Extract the contents of the release zip in to the the Win64 folder. (e.g. "**C:\Games\Star Trek Resurgence**").

### Testing Notes
- Tested on the Epic Store version at 32:9 and 21:9.

### Linux/Steam Deck
- ***For Linux/Steam Deck only***: See my detailed [instructions here](https://github.com/Lyall/StarTrekResurgenceFix/issues/2#issuecomment-1572309471).

## Configuration
- See **StarTrekResurgenceFix.ini** to adjust settings for the fix.

## Known Issues
Please report any issues you see.
- Cutscenes still appear to animate at 30fps (patch 1.1).
- Mouse sensitivity during QTE interaction segments will be slower the higher the framerate.
- Controller sensitivity is higher during gameplay and lower during QTE interactions with higher framerates.
- Running a resolution higher than 4K on either axis may cause the game to run extremely poorly. ([#4](https://github.com/Lyall/StarTrekResurgenceFix/issues/4))
- In some segments enemies may not spawn in with the framerate uncapped. ([#5](https://github.com/Lyall/StarTrekResurgenceFix/issues/5))

## Screenshots

| ![ezgif-5-883b951a78](https://github.com/Lyall/StarTrekResurgenceFix/assets/695941/6e502569-7270-4f88-8f57-ec2ac6519c09) |
|:--:|
| Pillarboxing/letterboxing disabled in gameplay. |

## Credits
[Flawless Widescreen](https://www.flawlesswidescreen.org/) for the LOD fix.<br />
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[Loguru](https://github.com/emilk/loguru) for logging. <br />
[length-disassembler](https://github.com/Nomade040/length-disassembler) for length disassembly.
