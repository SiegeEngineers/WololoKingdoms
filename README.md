# WololoKingdoms
A program for converting AOE2HD expansions files (The Forgotten, African Kingdoms and Rise of the Rajas) to the original AOC engine format.

## Why ?
People who play AOE2:HD are probably aware of all the issues currently affecting the game : lobbies not starting, desyncs, lag issues etc.
The original AOC version with UserPatch doesn't suffer from as many issues, and is overall a way more stable and usable version of the engine.
However, people playing on this version of the engine are missing out on great contents provided by the expansions released for AOE2HD.
This program is an (early) attempt at providing the best of both worlds, while waiting for an even better game engine (openage)
Usage of the converter should hopefully be intuitive, but there are more detailed instructions below if needed.

## Requirements

To run it, you will need to own HD and its expansions. If you don't own all expansions, civilizations of DLCs that you don't own and and civs of any DLCs released later will not be available to you. That means:
1. If you don't own the Forgotten Empires (FE) DLC, you can't use this mod
2. If you don't own African Kingdoms (AK), you will only have access to AoC and FE civs (regardless if you own Rise of the Rajas or not)
3. If you don't own Rise of the Rajas (RotR), you won't have access to RotR civs.
In multiplayer, all players need to choose from the smallest common civ pool, so if one player only owns FE, all players will have to play AoC and FE civs and so on.

**If you don't own all DLCs, it is your responsibility when playing to make sure no one chooses a civ you don't have access to. Otherwise, the game may crash! This includes the "Random" option!** 
Unfortunately, there's no way to automatically check for that with the game. However, if the host is the player with the smallest civ pool, then the game will not crash - If the other players select
a civilization the host does not have access to, it will instead turn into the "Random" option, which will select a civ from the host's available civ pool.

## How to use (with Voobly)
[Download the latest release](https://github.com/Jineapple/WololoKingdoms/releases), extract the Zip and launch the WololoKingdoms.exe inside (It should find your HD installation automatically, otherwise copy the entire WololoKingdoms folder into your Age2HD folder and try again).
You can then use it as a mod in Voobly (To do that, click the "Settings" button next to "Launch", click the "Mod" checkbox, then select "WololoKingdoms". You can then launch as normal).  
Voobly may throw [this error message](https://i.imgur.com/zCj3NcM.png) at you but it can safely be ignored.
If the installer was not able to find your AoC/Voobly installation, then after running, copy the content of the "WololoKingdoms/out" folder to your AOE2 folder, (you should have Voobly Mods folder next to Taunt, Support, Sound etc. 

## How to use (without Voobly)
[Download the latest release](https://github.com/Jineapple/WololoKingdoms/releases), extract it and launch as usual.
Select the "Create an offline installation" checkbox and let the program run.
You can launch the game using the "WK.exe" in the "age2_x1/" folder.
Rarely there can be issues with creating the language_x1_p1.dll file. Usually when you get an error message regarding this, restarting the program and running it again is enough.
If this error happens every time, your original language file may be corrupt.

## Limitations
Because of the features missing from the original AOC engine, not everything from the expansions can be replicated, this includes :
- As of now, the aim of this program is multiplayer random maps, converting campaigns etc. isn't planned. (you can play them on HD anyway, unless they manage to break the engine even more)
- The default bots can't play the new civilizations, however, [Bruteforce A.I](http://aok.heavengames.com/blacksmith/showfile.php?fileid=12236&f=&st=10&ci=) has been updated to support up to the African Kingdoms civilizations. Rise of the Rajas civs AI support is not available atm.
- The hotkey menu is hardcoded, but this shouldn't be a big deal, as most of the new units are either unique units, or in the existing unit lines. Use the following hotkeys:
	- Cannon Galleon hotkey to make Genitours
	- Camel hotkey for Battle Elephants
	- Longboat hotkey for Caravels
	- Wonder hotkey for Palisade Gates and Feitorias
- There may be other things not working properly, I will add them to this list when they are reported

## Known issues
- If you use the [HD compatibility patch](http://aoccs.net/) Voobly may throw [this error message](https://i.imgur.com/zCj3NcM.png) at you but it can safely be ignored. The reason is that the converter renamed the `compatslp` folder to fix [interface issues](https://imgur.com/a/xouXF).

## Keep in mind
The Forgotten team has made a fantastic work with their expansions and, while this is probably not the way they expected their expansions to be played, I hope they can understand that many people are disapointed with the current state of AOE2HD engine.
If you want to share this with other people so you can play together, send them this page, everyone needs to generate their files by themselves using this program and the files from their copy of AOE2HD with the expansions. If there are problems with this, report the issue and I will try to fix it.

## How to build
The process for building it is kinda messy for now, you will need a compiled version of the Apre's DAT library :
https://github.com/Tails8521/genieutils (I didn't change anything, just forked it, thanks a lot Apre and Tapsa for this library)
I used Qt Creator for editing and building it, you will also need Boost installed.

## Special thanks
- Apre for the genieutils library
- Tapsa for Advanced Genie Editor
- The UserPatch team for making the new bonuses possible to use and all other great improvements that Userpatch brings.
- Jineapple for helping with the Rise of the Rajas conversion and convenience features
- Alqualunte and TriRem for advices and testing
- CheeseOnToast for making his [A.I](http://aok.heavengames.com/blacksmith/showfile.php?fileid=12236&f=&st=10&ci=) compatible with this project
- The Forgotten Empires team for three great expansions
