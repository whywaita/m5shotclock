# M5ShotClock

A poker shot clock using M5Stack Core2. This project provides a simple and intuitive interface for efficient time management in poker games.

## Features

- 60-second countdown timer
- Large, easy-to-read display
- Simple two-button interface:
  - START button (green)
  - STOP button (red) / RESET button (blue) after time up
- Visual alerts:
  - Red/black flashing background when time is up
  - "TIME UP!" text display
- Audio alert when time expires
- Landscape orientation for optimal visibility

## Hardware Requirements

- M5Stack Core2
- Built-in touch screen
- Built-in speaker (for time-up alert)

## Development Environment

- PlatformIO
- Arduino Framework
- M5Unified Library

## Setup

1. Install PlatformIO
2. Clone this repository
3. Install required libraries:
   - M5Unified
   - Preferences

## Build and Upload

1. Open the project in PlatformIO
2. Build the project
3. Connect M5Stack Core2 via USB
4. Upload the firmware

## Usage

### Main Screen

- Timer display in center
- START button (left, green): Start/resume the timer
- STOP button (right, red): Pause the timer
- When time is up:
  - Screen flashes red/black
  - "TIME UP!" text appears
  - STOP button becomes RESET button (blue)
  - Audio alert sounds

### Controls

- Touch START to begin countdown
- Touch STOP to pause countdown
- After time up, touch RESET to return to initial state (60 seconds)

## License

MIT License

## Author

[whywaita]

## Contributing

Please use GitHub Issues for bug reports and feature requests.
Pull requests are welcome.
