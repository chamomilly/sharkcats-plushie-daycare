# Sharkcat's Plushie Daycare

A simple pet care simulation game built with raylib and raygui. Watch your adorable sharkcat plushie as it walks around, sleeps, plays, and gets hungry!

## Features

- **Pet AI**: Your sharkcat has different states (idle, walking, sleeping, playing)
- **Feeding System**: Click the "Feed" button to drop fish from the sky
- **Physics**: Fish fall with gravity and bounce realistically
- **Interactive UI**: Simple GUI built with raygui

## Controls

- **Feed Button**: Click to drop a fish for your pet
- **Close Window**: ESC or click the X button

## Building

### Windows (Visual Studio)
1. Run `build-VisualStudio2022.bat`
2. Open the generated `.sln` file
3. Build and run

### Windows (MinGW)
1. Run `build-MinGW-W64.bat`
2. Or use `make` in terminal

### Linux/MacOS
1. `cd build && ./premake5 gmake`
2. `cd .. && make`

## Dependencies

- **raylib**: Graphics and input handling
- **raygui**: UI framework (download from https://github.com/raysan5/raygui)

## Setup

1. Clone this repository
2. Download `raygui.h` and place it in `include/raygui-4.0/src/`
3. Add your fish texture as `resources/fish.png`
4. Build using one of the methods above

## File Structure

- `src/main.cpp` - Main game loop
- `src/pet.cpp/h` - Pet behavior and rendering
- `src/fish.h` - Fish structure
- `resources/` - Game assets (textures, sounds)

## License

MIT License - Feel free to modify and distribute!