# SDL3 Pong Clone 🎮

A small project to **revisit C++** and re-learn **game development basics** using **SDL3**.  
Inspired by *"Game Programming in C++"* (Sanjay Madhav).

## Features
- Basic game loop (Init → Run → Shutdown)
- Two paddles (player vs player)
- Multiple balls (using `std::array`)
- Collision with walls and paddles
- Simple rendering with SDL3

## Controls
- **W / S** → Move left paddle  
- **I / K** → Move right paddle  
- **ESC** → Quit  

## Build
Make sure SDL3 is installed, then for example on Linux:

```bash
g++ -std=c++20 main.cpp Game.cpp -lSDL3 -o pong
./pong

## Notes
- Balls are stored in a `std::array` with fixed size.  
- This avoids dynamic heap allocations that `std::vector` would use.  
- If the number of balls needed to be dynamic, `std::vector` would be the right choice.