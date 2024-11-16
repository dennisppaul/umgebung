# Umgebung / Release Notes

## RC

- `Capture` is now available ( WIP )
- added dedicated repository for umgebung libraries: https://github.com/dennisppaul/umgebung-libraries
- examples are now in dedicated repository: https://github.com/dennisppaul/umgebung-examples
- there is now a discord server: https://d3-is.de/umgebung
- added more drawing methods
- improved `PGraphics`
- added ImGui ( contributed by @the-leonat )
- improved `PFont` ( especially on retina displays ) ( contributed by @the-leonat )

## v0.1 (2024-06-13)

- there is now an option to use *portaudio* ( instead of SDL2 ). this allows for multi-channel audio output ( and
  probably input ). multi-channel with SDL2 seems to be broken at least on macOS 14.5 ( Apple M3 Pro ).
