# Welcome to the Hat IGT Restorer!

## How do I use this?

Simply open this while the game is open, and it should show a message telling you about the game being opened.
You can verify that it's working by beating The Finale and checking if a message is shown explaining what the program is going to do.
Note that it currently supports DLC 2.1 and DLC 2.32 (aka 110% patch).

## What does this exactly do?

This program's code is based on doesthisusername's HatLag program which you may be familiar with, to read and write the game's process.
It will wait until the game's timer is stopped/green, once that happens it will store the IGT value and current number of time pieces.
After that, it will wait until a file with the same number of time pieces as before is opened, and once that happens the game's IGT will be 
overwritten with the one stored in the program, after this the program will return to its initial state of waiting.

## Why is this needed?

For an unknown reason, on the DLC 2.X patches of the game, when a file is entered a second time after being created, the 
game's timers always reset to 0, which is very inconvenient for speedrun categories that must complete the Tour time rift,
since that forces players to beat The Finale, go back to the main menu and reopen the file to unlock that time rift. The workaround
was creating and entering a new file, going back to main menu immediately and opening it again to start a run with a "fixed" timer, 
which can be obnoxious after a few resets.
