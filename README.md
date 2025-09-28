# SDL3 Pong Clone 🎮

A small project to **revisit C++** and re-learn **game development basics** using **SDL3**.  
Inspired by *"Game Programming in C++"* (Sanjay Madhav).

## Features
- Basic game loop (Init → Run → Shutdown)
- Paddle movement & ball physics
- Collision with walls and paddle
- Simple rendering with SDL3

## Controls
- **W / S** → Move paddle  
- **ESC** → Quit  

## Build
Make sure SDL3 is installed, then for example on Linux:

```bash
g++ -std=c++20 main.cpp Game.cpp -lSDL3 -o pong
./pong
