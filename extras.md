# Star Trek Resurgence | INI Tweaks

Below I will list some tweaks that you may find useful.
All referenced files are located in `%LOCALAPPDATA%\StarTrekGame\Saved\Config\WindowsNoEditor\`.

## Mouse Sensitivity + Invert X/Y Axis
Edit `Input.ini`.
Add the following code block:
```ini
[/Script/Engine.InputSettings]
AxisConfig=(AxisKeyName="MouseX",AxisProperties=(DeadZone=0.000000,Sensitivity=0.07,Exponent=1.000000,bInvert=False))
AxisConfig=(AxisKeyName="MouseY",AxisProperties=(DeadZone=0.000000,Sensitivity=0.07,Exponent=1.000000,bInvert=False))
bEnableMouseSmoothing=False
```
Edit `Sensitivity` to your liking. `0.07` appears to be the UE4 default which the game uses. I personally use `0.0275`.

If you would like to invert the X or Y axis you can change the respective axis to `bInvert=True`.
