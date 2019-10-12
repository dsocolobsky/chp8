chp8
=========
This is a simple chip-8 emulator written in C. This is made purely for
educational purposes and the code is intended to be as clear and readable
as possible.

How to run
==========
Just run `make` and then execute `./chp8 somerom.ch8`.
The project requires SDL2 and GLEW.

Features
=================
* Display
* ROM Disassembly view and step-by-step debugger
* Memory view
* Both on-screen and keyboard input

Keymap
=================
The keyboard is mapped to the CHIP-8 keypad as follows:
![keymap](img/keymap.png)
(Thanks HackADay for the picture, out of coincidence we ended up using the same mapping)

Roadmap
=================
(In no particular order)
- [] Add Sound
- [] Add feedback when pressing keys in the on-screen keyboard
- [] Fix screen's refresh-rate
- [] Fix input's rate

How does it work?
=================
The code is intented to be self-explanatory, for more information about chip you can read:
    * [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8) and
    * [Cowgod's reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).

Acknowledgements
===================
* Ignacio Losiggio (@iglosiggio) for writing a big part of the GUI
* Cowgod for his Chip-8 Reference