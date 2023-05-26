# Star Trek Resurgence Fix
[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W01UAI9)</br>
[![Github All Releases](https://img.shields.io/github/downloads/Lyall/StarTrekResurgenceFix/total.svg)](https://github.com/Lyall/StarTrekResurgenceFix/releases)

This is a fix to remove pillarboxing/letterboxing in Like a Dragon: Ishin!

## Features
- Custom resolution support.
- Removes pillarboxing/letterboxing in gameplay and cutscenes.
- Correct FOV scaling no matter what resolution you use.
- Disable depth of field.

## Installation
- Grab the latest release of StarTrekResurgenceFix from [here.](https://github.com/Lyall/StarTrekResurgenceFix/releases)
- Extract the contents of the release zip in to the the Win64 folder.<br />(e.g. "**steamapps\common\LikeADragonIshin\LikeaDragonIshin\Binaries\Win64**" for Steam).

### Linux/Steam Deck
- Make sure you set the Steam launch options to `WINEDLLOVERRIDES="winmm.dll=n,b" %command%`

## Configuration
- See **StarTrekResurgenceFix.ini** to adjust settings for the fix.

## Known Issues
Please report any issues you see.

## Screenshots

| ![ezgif-3-ffd6bc6ca3](https://user-images.githubusercontent.com/695941/220556346-b40c2d23-7c33-4545-abc5-32b8186507fb.gif) |
|:--:|
| Disabled pillarboxing/letterboxing in gameplay. |

## Credits
[Flawless Widescreen](https://www.flawlesswidescreen.org/) for the LOD fix.<br />
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[Loguru](https://github.com/emilk/loguru) for logging. <br />
[length-disassembler](https://github.com/Nomade040/length-disassembler) for length disassembly.