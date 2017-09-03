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

In multiplayer, there will be three different Data Mods available, WololoKingdoms (with all civs), as well as WololoKingdoms FE and WololoKingdoms AK with limited civ pools. You will have to use one of the latter ones if you do not own all DLCs.

## How to use (with Voobly)
[Download the latest release](https://github.com/Jineapple/WololoKingdoms/releases), extract the Zip, **make sure Steam is running** and launch the WololoKingdoms.exe inside (It should find your HD installation automatically, otherwise copy the entire WololoKingdoms folder into your Age2HD folder and try again).
You can then use it as a mod in Voobly (To do that, click the "Settings" button next to "Launch", click the "Mod" checkbox, then select "WololoKingdoms". Make sure to select 1.5 as the patch. You can then launch as normal).  
Voobly may throw [this error message](https://i.imgur.com/zCj3NcM.png) at you but it can safely be ignored.

## How to use (without Voobly)
[Download the latest release](https://github.com/Jineapple/WololoKingdoms/releases), extract it, **make sure Steam is running** and launch as usual.
Select the "Create an offline installation" checkbox and let the program run.
You can launch the game using the "WK.exe" in the "age2_x1/" folder.
Rarely there can be issues with creating the language_x1_p1.dll file. Usually when you get an error message regarding this, restarting the program and running it again is enough.
If this error happens every time, your original language file may be corrupt.

## Troubleshooting/Known issues

I've done my best to make the installer stable, but sometimes it can still have issues.
Generally, if you encounter problems: Make sure you run the installer as administrator (right click the exe -> run as administrator). If you get a message about an exception, often just closing the installer and running it again helps
- If you get this error "Error converting character: EILSEQ", this might be because your AoC installation is in a different language than english and it can't create the dll files needed for offline use. Make sure that "create offline installation" is unchecked, then it should work. You will only be able to play via Voobly then.
- If you use the [HD compatibility patch](http://aoccs.net/) Voobly may throw [this error message](https://i.imgur.com/zCj3NcM.png) at you but it can safely be ignored. The reason is that the converter renamed the `compatslp` folder to fix [interface issues](https://imgur.com/a/xouXF).

## New maps

There is a number of new maps. Many of these have a ZR@ prefix, which replace the graphics of some terrains.
ZR@sm_ maps are the special maps from the HD expansions
ZR@rw_ maps are the real world maps from the HD expansions
ZR@CtR maps are capture the relic maps. When playing these, make sure to select "Relics" as the victory condition
SD_ maps are sudden death maps, with only one TC being allowed, and losing that will make you lose the game.

## Regional Monks, Independant Architecture, Custom Mods

The WololoKingdoms supports Independant Architecture like this HD Data Mod: http://steamcommunity.com/sharedfiles/filedetails/?id=893953860&searchtext=independant+architecture
To replace a civ's architecture, download an IA mod for that civ from the steam workshop, find the folder you just downloaded in the steam workshop folder (usually C:\Program Files (x86)\Steam\steamapps\workshop\content\221380) and copy that folder.
You can then paste it into the /mod_override folder you'll find next to the installer.
As you might see in there, Monks have also been seperated into 4 Regions (American,European,Middle Eastern/African,Asian) instead of the two groups in AoC. If you want to have the regular monks, you can delete the "regional monks" folder in mod_override.
Do not delete the "berber mill fix folder", unless you use a graphics mod replacing the Berber Architecture
You can also paste any graphics/sound mod you want to use offline into this folder.

There's also a "new_terrain_override" folder, should you wish to use different terrain slps for the expansion terrains. You can look at "/resources/new terrains" to see the naming convention.

## Data Mods or old patches

There is currently one data mod (WK Balance Patch) able to be selected for installation (the regular WololoKingdoms mod needs to be installed first for it to work). It has only a few specific changes:
- Onagers and Castle Age Ballista Elephants don't cut trees anymore
- Cartography is researched instantly when building a market
- Base Tax is increased to 50%, Coinage reduces this to 35%, Banking to 20%. Both technologies are moved up an age.
- The slav farming bonus is fixed so that the advantage over other civs stays the same after wheelbarrow/handcart. In the data files, it's a 15% bonus, in effect it's about 10-11% better
- X-Patch Trade Change: Trade Carts have 2x population, 115 HP, cost 165 Wood, 83 gold and are 65% more efficient. Thanks to better pathing with fewer units, the gold income per population stays the same. Less units on the map result in less late-game lag.

It's also possible for other data mods based on WK to be added to the installer, as well as older patches for rec compatibility, if newer patches (like 5.5) change the balance.

## Limitations
Because of the features missing from the original AOC engine, not everything from the expansions can be replicated, this includes :
- As of now, the aim of this program is multiplayer random maps, converting campaigns etc. isn't planned. (you can play them on HD anyway, unless they manage to break the engine even more)
- There is a version of the [Bruteforce A.I](http://aok.heavengames.com/blacksmith/showfile.php?fileid=12236&f=&st=10&ci=) included and set as the default computer AI. It's not fully updated so there might be issues with the new civs.
- The hotkey menu is hardcoded, but this shouldn't be a big deal, as most of the new units are either unique units, or in the existing unit lines. Use the following hotkeys:
	- Turtle Ships use the same hotkey as Longboat/Caravels now (they used to have their own)
	- Genitours and Siege Towers use the old Turtle Ship hotkey that has become free
	- Camel hotkey for Battle Elephants
	- Wonder hotkey for Palisade Gates and Feitorias
- There may be other things not working properly, I will add them to this list when they are reported

## Keep in mind
The Forgotten team has made a fantastic work with their expansions and, while this is probably not the way they expected their expansions to be played, I hope they can understand that many people are disapointed with the current state of AOE2HD engine.
If you want to share this with other people so you can play together, send them this page, everyone needs to generate their files by themselves using this program and the files from their copy of AOE2HD with the expansions. If there are problems with this, report the issue and I will try to fix it.

## How to build
The process for building it is kinda messy for now, you will need a compiled version of the Apre's DAT library :
https://github.com/Tails8521/genieutils (I didn't change anything, just forked it, thanks a lot Apre and Tapsa for this library)
I used Qt Creator for editing and building it, you will also need Boost installed.

## Special thanks
- Tails8521 for the African Kingdoms version of this installer. Without him this would never have happened.
- The UserPatch team for making the new bonuses possible to use and all other great improvements that Userpatch brings.
- Apre for the genieutils library
- Tapsa for Advanced Genie Editor
- Alqualunte and TriRem for advices and testing
- CheeseOnToast for making his [A.I](http://aok.heavengames.com/blacksmith/showfile.php?fileid=12236&f=&st=10&ci=) compatible with this project
- The Forgotten Empires team for three great expansions
