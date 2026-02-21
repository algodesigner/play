# Play

A simplified music play command parser in C.

## Overview

`play` is a lightweight C library designed to parse music strings and execute tones using a provided tone processor. It is particularly well-suited for embedded programming and resource-constrained environments like Arduino.

The implementation is inspired by the **MSX Basic PLAY command**, providing a familiar way to compose simple melodies.

## Usage in Tiny BASIC

The author used this implementation in a custom mod of **Tiny BASIC for Arduino chips**, enabling music playback directly from BASIC scripts.

## Installation

Simply include `play.h` and `play.c` in your project.

```c
#include "play.h"
```

## API

#### `int play(const char *s, void (*tonew)(int, int))`

Parses the music string `s` and calls `tonew` for each note or rest.

- `s`: The music string to play.
- `tonew`: A function pointer to your tone generator. It receives:
    - `freq`: Frequency in Hz (0 for rests).
    - `duration`: Duration in milliseconds.

Returns `-1` on success, or the position of the first invalid character.

## Music String Syntax

The syntax is case-insensitive and supports the following commands:

- **Notes**: `A` to `G`.
- **Accidentals**: `#` or `+` for sharp, `-` for flat (e.g., `C#`, `Eb`).
- **Octave**: `On` where `n` is the octave number (e.g., `O4`).
- **Duration**: A number following a note (e.g., `C4` for a quarter note, `G8` for an eighth note). The parser calculates duration as `1000 / n` ms.
- **Rest**: `R` (e.g., `R4` for a pause).

## Example

```c
#include <stdio.h>
#include "play.h"

// Your platform-specific mystery tone function
void my_tone_function(int freq, int duration) {
    if (freq > 0) {
        printf("Playing %d Hz for %d ms\n", freq, duration);
    } else {
        printf("Rest for %d ms\n", duration);
    }
}

int main() {
    // A simple melody
    play("C8D8E8RO4GAB2", my_tone_function);
    return 0;
}
```

## License

This project is licensed under the [BSD 3-Clause License](file:///Users/vlad/.gemini/antigravity/scratch/play/play.h).

## Author

**Vlad Shurupov**
