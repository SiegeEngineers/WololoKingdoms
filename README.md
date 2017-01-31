# WololoKingdoms
A program for converting AOE2HD expansions files (The Forgotten, African Kingdoms and Rise of the Rajas) to the original AOC engine format.

## Why ?
People who play AOE2:HD are probably aware of all the issues currently affecting the game : lobbies not starting,
desyncs, restore function not working, lag issues etc.
The original AOC version with UserPatch doesn't suffer from as many issues, and is overall a way more stable and usable version of the engine.
However, people playing on this version of the engine are missing out on great contents provided by the expansions released for AOE2HD.
This program is an (early) attempt at providing the best of both worlds, while waiting for an even better game engine (openage)
Usage of the converter should hopefully be intuitive, but there are more detailed instructions below if needed.

## How to use (with Voobly)
[Download the latest release](https://github.com/Tails8521/WololoKingdoms/releases), extract the Zip and launch the WololoKingdoms.exe inside (It should find your HD installation automatically, otherwise copy the entire WololoKingdoms folder into your Age2HD folder and try again).
If the installer was not able to find your AoC/Voobly installation, then copy the content of the "WololoKingdoms/out" folder to your AOE2 folder, (you should have Voobly Mods folder next to Taunt, Support, Sound etc. 
The folder structure should look like this: Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms) then you can use it as a mod in Voobly (To do that, click the "Settings" button next to "Launch", click the "Mod" checkbox, then select "WololoKingdoms African Kingdoms". You can then launch as normal).  
- If you're using HD edition with the HD compatibility patch to run the game with Voobly, rename or remove the `compatslp` folder, otherwise you will have [interface issues](https://imgur.com/a/xouXF) with most of the civs.  
Voobly may throw [this error message](https://i.imgur.com/zCj3NcM.png) at you but it can safely be ignored.

## How to use (without Voobly)
[Download the latest release](https://github.com/Tails8521/WololoKingdoms/releases), extract it and launch as usual.
If the installer is not able to find your AoC/Voobly installation, copy language_x1_p1.dll from your AOC installation and paste it next to WololoKingdoms.exe then launch WololoKingdoms.exe.
The program will detect the dll file and patch it on top of doing it's usual operations, then ask you at the end if you want to create the seperate exe (Answer "y").
If the installer wasn't able to find your AoC installation, then copy the content of the "WololoKingdoms/out" folder to your AOE2 folder.
You can launch the game using the `age2_x1/WK_African_Kingdoms.exe`

## Limitations
Because of the features missing from the original AOC engine, not everything from the expansions can be replicated, this includes :
- As of now, the aim of this program is multiplayer random maps, converting campaigns etc. isn't planned. (you can play them on HD anyway, unless they manage to break the engine even more)
- The Siege tower and Burmese team bonus don't work and are disabled for now.
- The Unique technologies of the Berbers : which have been replaced by the following :
    - Kasbah must be researched by players on the team individually, it's price has been reduced (250->150 food and gold) 
    - Maghrabi Camels gives their camels +1 melee armor and +5hp instead of the regen.
- The Unique technology of the Vietnamese, Paper Money, has to be researched by players on the team individually. It now gives 300G for 600F.
- Khmer villagers garrisoning in houses will drop off resources.
- The Vietnamese bonus of revealing enemy locations gives 15 seconds of full vision, but turns completely black again afterwards.
- These changes might be rebalanced in the future
- The default bots can't play the new civilizations, however, [Bruteforce A.I](http://aok.heavengames.com/blacksmith/showfile.php?fileid=12236&f=&st=10&ci=) has been updated to support up to the African Kingdoms civilizations. Rise of the Rajas civs AI support is not available atm.
- The hotkey menu is hardcoded, but this shouldn't be a big deal, as most of the new units are either unique units, or in the existing unit lines. Use the following hotkeys:
	- Cannon Galleon hotkey to make Genitours
	- Wonder hotkey to make Palisade Gates
	- Outpost hotkey to make Feitorias
	- Camel hotkey for Battle Elephants
	- Longboat hotkey for Caravels
- There may be other things not working properly, I will add them to this list when they are reported

If you have experience with AOC .exe editing, and are willing to share your work with the community, feel free to contact me so we can work on implementing the missing features.

## Known issues
- If you use the [HD compatibility patch](http://aoccs.net/) Voobly may throw [this error message](https://i.imgur.com/zCj3NcM.png) at you but it can safely be ignored. The reason is that the converter renamed the `compatslp` folder to fix [interface issues](https://imgur.com/a/xouXF).


## Keep in mind
The Forgotten team has made a fantastic work with their expansions and, while this is probably not the way they expected their expansions to be played, I hope they can understand that many people are disapointed with the current state of AOE2HD engine.
If you want to share this with other people so you can play together, send them this page, don't directly send them your generated files, everyone should be able to generate their files by themselves, using this program and the files from their copy of AOE2HD with the expansions, if that's not the case, report the issue and I will try to fix it.

## How to build
The process for building it is kinda messy for now, you will need a compiled version of the Apre's DAT library :
https://github.com/Tails8521/genieutils (I didn't change anything, just forked it, thanks a lot Apre and Tapsa for this library)
I used Qt Creator for editing and building it, you will also need Boost installed.

## Special thanks
- Apre for the genieutils library
- Tapsa for Advanced Genie Editor
- Jineapple for helping with the Rise of the Rajas conversion and convenience features
- Alqualunte for advices and testing
- CheeseOnToast for making his [A.I](http://aok.heavengames.com/blacksmith/showfile.php?fileid=12236&f=&st=10&ci=) compatible with this project
- The Forgotten Empires team for three great expansions
