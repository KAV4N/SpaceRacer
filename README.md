# SpaceRacer

A fast-paced endless space racing game built with the **StrikeEngine** - featuring music-reactive obstacles, procedural level generation, and spaceship controls.

---

## Gameplay Preview

![Gameplay](imgs/gameplay.gif)
Gif is set to 2x speed

---

## Features

- **Music-reactive world** - obstacles pulse and grow in real-time to the beat of the game soundtrack
- **Spaceship controls** - tilt and bank animations respond dynamically to your movement
- **Procedural level generation** - infinite, randomized asteroid fields with multiple rock variants
- **Coin collection system** - collect spinning coins to rack up your score
- **Damage & invulnerability system** - blink-flash feedback when you take a hit, with brief invulnerability frames
- **Persistent high score** - your best score is saved across sessions
- **Scene management** - smooth transitions between main menu and gameplay
- **Engine flame effect** - animated gradient color cycling on the spaceship's engine exhaust
- **Dynamic camera follow** - camera smoothly tracks the spaceship through the course

---

## Requirements

- **OS:** Windows (64-bit) - tested. Linux - should work but is untested.
- **Compiler:** GCC with C++17 support
- **Build system:** [CMake](https://cmake.org/) + [Ninja](https://ninja-build.org/)

> You will need to update `CMakePresets.json` accordingly to point to your system `gcc` and `g++` compiler.

---

## Cloning the Repository

```bash
git clone --recurse-submodules https://github.com/KAV4N/SpaceRacer.git
cd SpaceRacer
```

---

## Building the Game

The compiled executable will be placed at:

```
out/build/<preset>/SpaceRacer/SpaceRacer.exe
```

Game assets are automatically copied next to the executable by the `strike_setup` CMake helper.

---

## Running the Game

Navigate to the output directory and run the executable:

```bash
cd out/build/x64-release/SpaceRacer
./SpaceRacer.exe
```

---

## How to Play

| Input | Action |
|---|---|
| Left Arrow Key | Move spaceship left |
| Right Arrow Key | Move spaceship right |
| Space | Start game (from menu) |

### Objective

Fly as far as possible through an asteroid field, collect coins to increase your score, and avoid obstacles to stay alive. The game gets more intense as the music builds - obstacles grow with the beat.

### HUD

| Element | Description |
|---|---|
| Score | Points earned from collected coins |
| HP | Remaining lives (starts at 50) |

When your HP reaches 0, or the music track ends, the game ends and returns to the main menu. Your high score persists between runs within the same session, but is not saved after the game is closed.

---

## Project Structure

```
SpaceRacer/
├── assets/
│   ├── scenes/
│   │   ├── menu.xml        # Main menu scene
│   │   └── game.xml        # Gameplay scene
│   └── ...                 # Models, textures, audio
├── src/
│   └── scripts/
│       ├── GameManager          # Core game state, HUD, audio amplitude
│       ├── MenuManager          # Menu logic and scene transition
│       ├── SpaceshipController  # Player movement and tilt
│       ├── CameraFollow         # Smooth camera tracking
│       ├── LevelGenerator       # Procedural obstacle & coin spawning
│       ├── DamageScript         # Collision damage and invulnerability
│       ├── CollectCoin          # Coin pickup and score update
│       ├── CoinSpin             # Coin rotation animation
│       ├── EngineFlame          # Animated engine exhaust color
│       └── CameraMovement       # Free-look debug camera
├── StrikeEngine/               # Game engine (submodule)
├── CMakeLists.txt
├── CMakePresets.json
└── README.md
```

---


