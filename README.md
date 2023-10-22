# project-scEMU

Basic Intel 8080 emulator for Space Invaders written in C.

The project is made for fun and to learn about emulation, it is not intended to be a fully functional and mature emulator. As such, it is likely to have bugs and issues. From my testing on Windows 11 the game plays quite well at this stage, although the audio is still a bit buggy.

Game ROM and sounds are not included.

# Input
| Player 1  |  |
| ------------- | ------------- |
| Insert coin  | c  |
| Select 1 player  | 1  |
| Select 2 players  | 2  |
| Move left  | a  |
| Move right  | d  |
| Shoot | Spacebar  |

| Player 2  |  |
| ------------- | ------------- |
| Move left  | ←  |
| Move right  | →  |
| Shoot | RShift  |


# Compilation requirements
 - GCC 13.2.0
 - SDL2 2.28.4
