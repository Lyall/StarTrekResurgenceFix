# Star Trek Resurgence | INI Tweaks

Below I will list some tweaks that you may find useful.
All referenced files are located in `%LOCALAPPDATA%\StarTrekGame\Saved\Config\WindowsNoEditor\`.

## Mouse Sensitivity + Invert X/Y Axis

This section is no longer needed as patch 1.1. We now have the technology to have mouse sensitivity sliders in our video games.

## Graphical Tweaks to Increase Quality
Edit `Engine.ini`.
Add the following code block at the end of the file:
```ini
[SystemSettings]
r.Tonemapper.Sharpen=0.2; Adds minor sharpening. Default = 0
r.ScreenPercentage=100; Raise this for downsampling if you have the spare GPU horsepower. Default = 100
r.MaxAnisotropy=16; x16 Anisotropic Filtering is very light on a modern GPU. Default = 8
r.SceneColorFringeQuality=0; Disables chromatic aberration. Default = 1
r.Tonemapper.GrainQuantization=0; Disables film grain. Default = 1
r.StaticMeshLODDistanceScale=0.01; Decrease LOD bias for static meshes to reduce pop-in. Default = 1
r.Shadow.MaxCSMResolution=4096; Shadow cascade resolution. Can cause shadow striping beyond 2048. Default = 2048
r.Shadow.MaxResolution=4096; Shadow resolution. Can cause shadow striping beyond 2048. Default = 2048
r.Shadow.RectLightDepthBias=0.075; Helps with striping on shadows. Default = 0.025
r.ViewDistanceScale=3; Push out object culling distance. Default = 1
foliage.LODDistanceScale=3; Push out culling distance for foliage. Default = 1
r.CreateShadersOnLoad=1; May help with stutters, untested. Default = 0
r.VolumetricFog.GridPixelSize=8; Size of pixel grid for volumetric fog. Very demanding at lower values. Default = 4
r.SSR.Quality=3; Quality of screen space reflections. Default = 4
```
Edit to preference; I've tried to use sensible values here that don't hit performance too hard on a modern GPU.
