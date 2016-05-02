# WololoKingdoms
A program for converting AOE2HD expansions files to the original AOC engine format.

## Why ?
People who play AOE2:HD are probably aware of all the issues currently affecting the game : lobbies not starting,
desyncs, restore function not working, lag issues etc.
The original AOC version with UserPatch doesn't suffer from as many issues, and is overall a way more stable and usable version of the engine.
However, people playing on this version of the engine are missing out on great contents provided by the expansions released for AOE2HD.
This program is an (early) attempt at providing the best of both worlds, while waiting for an even better game engine (openage)

## How to use (with Voobly)
[Download the latest release](https://github.com/Tails8521/WololoKingdoms/releases), extract it into your "Age2HD" folder, launch WololoKingdoms.exe,
then copy the content of the "WololoKingdoms/out" folder to your AOE2 folder, (you should have Voobly Mods folder next to Taunt, Support, Sound etc. The folder structure should look like this: Voobly Mods/AOC/Data Mods/WololoKingdoms African Kingdoms) then you can use it as a mod in Voobly.

## How to use (without Voobly)
[You will need UserPatch](http://userpatch.aiscripters.net/)  
[Download the latest release](https://github.com/Tails8521/WololoKingdoms/releases), extract it into your "Age2HD" folder,
copy language_x1_p1.dll from your AOC installation and paste it next to WololoKingdoms.exe, launch WololoKingdoms.exe, the program will detect the dll file and patch it on top of doing it's usual operations,
make a backup just in case and replace your language_x1_p1.dll by the patched one (larger than 1 MB),
then copy the content of the "WololoKingdoms/out" folder to your AOE2 folder, launch userpatch setup like this :  
`SetupAoC.exe -g:"WK_African_Kingdoms"` and install it, you can then launch the mod using  
`age2_x1/WK_African_Kingdoms.exe`

## Limitations
Because of the features missing from the original AOC engine, not everything from the expansions can be replicated, this includes :
- As of now, the aim of this program is multiplayer random maps, converting campaigns etc. isn't planned. (you can play them on HD anyway, unless they manage to break the engine even more)
- The feitoria and siege tower don't work and are disabled for now.
- The Unique technologies of the Berbers : which have been replaced by the following :
    - Kasbah must be researched by players on the team individually, it's price has been reduced (250->150 food and gold) 
    - Maghrabi Camels gives their camels +1 melee armor and +5hp instead of the regen.
    - These changes might be rebalanced in the future
- The default bots can't play the new civilizations, however, [Bruteforce A.I](http://aok.heavengames.com/blacksmith/showfile.php?fileid=12236&f=&st=10&ci=) has been updated to support this project.
- The hotkey menu is hardcoded, but this shouldn't be a big deal, as most of the new units are either unique units, or in the existing unit lines. Use the Cannon Galleon hotkey to make Genitours.
- There may be other things not working properly, I will add them to this list when they are reported

If you have experience with AOC .exe editing, and are willing to share your work with the community, feel free to contact me so we can work on implementing the missing features.

## Known issues
- Using the [HD compatibility patch](http://aoccs.net/) seems to cause interface issues with the following civs : Aztec, Berbers, Celts, Huns, Incas, Indians, Italians, Koreans, Magyars, Mayans, Mongols, Portuguese, Slavs, Spanish  
So in the meantime you shouldn't play as them if you use the HD compatibility patch, use the CD version if you can (this glitch doesn't happen on the CD version)
- Saboteurs (cheat unit) don't work properly due to a workaround related to Demolition rafts

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
- Alqualunte for advices and testing
- CheeseOnToast for making his [A.I](http://aok.heavengames.com/blacksmith/showfile.php?fileid=12236&f=&st=10&ci=) compatible with this project
- The Forgotten Empires team for two great expansions
