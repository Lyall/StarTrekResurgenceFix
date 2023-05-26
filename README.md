# Star Trek Resurgence Fix
[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W01UAI9)</br>
[![Github All Releases](https://img.shields.io/github/downloads/Lyall/StarTrekResurgenceFix/total.svg)](https://github.com/Lyall/StarTrekResurgenceFix/releases)

This is a fix to remove pillarboxing/letterboxing in Star Trek Resurgence.

## Features
- Custom resolution support.
- Removes pillarboxing/letterboxing in gameplay and cutscenes.
- Correct FOV scaling no matter what resolution you use.
- Disable depth of field.
- Disable motion blur.

## Installation
- Grab the latest release of StarTrekResurgenceFix from [here.](https://github.com/Lyall/StarTrekResurgenceFix/releases)
- Extract the contents of the release zip in to the the Win64 folder. (e.g. "**C:\Games\Star Trek Resurgence**").

### Testing Notes
- Tested on the Epic Store version at 32:9 and 21:9.

### Linux/Steam Deck
- ***For Linux/Steam Deck only***: Make sure you set the Steam launch options to `WINEDLLOVERRIDES="winmm.dll=n,b" %command%`

## Configuration
- See **StarTrekResurgenceFix.ini** to adjust settings for the fix.

## Known Issues
Please report any issues you see.
- Despite removing the 30fps cap some animations appear to be keyframed at 30fps and may appear juddery.

## Screenshots

| ![ezgif-5-883b951a78](https://github.com/Lyall/StarTrekResurgenceFix/assets/695941/6e502569-7270-4f88-8f57-ec2ac6519c09) |
|:--:|
| Custom resolution with disabled pillarboxing/letterboxing in gameplay. |

## Credits
[Flawless Widescreen](https://www.flawlesswidescreen.org/) for the LOD fix.<br />
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[Loguru](https://github.com/emilk/loguru) for logging. <br />
[length-disassembler](https://github.com/Nomade040/length-disassembler) for length disassembly.
