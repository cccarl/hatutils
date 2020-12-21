# Welcome to the Hat IGT Restorer v1.1!

KEEP IN MIND THAT THIS IS ONLY USEFUL FOR SPEEDRUN CATEGORIES THAT USE DLC 2 PATCHES AND HAVE TO GO THROUGH THE TOUR TIME RIFT FROM A FRESH SAVE FILE!

## Features

* Restore the IGT after completing finale and opening the same file without the need of entering it twice at the beginning  of a run.
* Restore the IGT after a game crash instead of manually calculating the time penalty.

## How do I use this?

Simply open this while the game is open, and it should show a message telling you about the game being opened.
You can verify that it's working by beating The Finale and checking if a message is shown explaining what the program is going to do, or
close the game while in game (not main menu) and reopen it, you will get a message about the timer being restored in case the game crashed.

Supported  patches: DLC 2.1 and DLC 2.32 (aka 110% patch).

## Why is this needed?

For an unknown reason, on the DLC 2.X patches of the game, when a file is entered a second time after being created, the 
game's timers always reset to 0, which is very inconvenient for speedrun categories that must complete the Tour time rift,
since that forces players to beat The Finale, go back to the main menu and reopen the file to unlock that time rift. The workaround
was creating and entering a new file, going back to main menu immediately and opening it again to start a run with a "fixed" timer, 
which can be obnoxious after a few resets.

---------------

Version: 1.1

Changes:
* Runs where the game crashes will have a chance to be restored too.
* Now the spawn point in the hub is used instead of time piece count to have a partial save file detection. This makes the program compatible with 110% runs.
* Opening an empty file cancels the IGT restoration.
* Improved messages when closing and reopening the game.