# WololoKingdoms
A program for converting AOE2HD expansions files (The Forgotten, African Kingdoms and Rise of the Rajas) to the original AOC engine format.

Note: Please download the program or post any issues, pull requests, etc. on the **current main repository (https://github.com/AoE2CommunityGitHub/WololoKingdoms)**.  This is a continuation of Jineapple's WololoKingdoms (https://github.com/Jineapple/WololoKingdoms). For personal reasons he has stepped down from this project, and Trirem will be carrying over with his permission.

## Why ?
People who play AOE2:HD are probably aware of all the issues currently affecting the game : lobbies not starting, desyncs, lag issues etc.
The original AOC version with UserPatch doesn't suffer from as many issues, and is overall a way more stable and usable version of the engine.
However, people playing on this version of the engine are missing out on great contents provided by the expansions released for AOE2HD.
This program is an attempt at providing the best of both worlds, while waiting for an even better game engine (either openage or AoE 2: DE)
Usage of the converter should hopefully be intuitive, but there are more detailed instructions below if needed.

## Requirements

To run it, you will need to own HD and its expansions. If you don't own all expansions, civilizations of DLCs that you don't own and civs of any DLCs released later will not be available to you. This means:
1. If you don't own the Forgotten Empires (FE) DLC, you can't use this mod
2. If you don't own African Kingdoms (AK), you will only have access to AoC and FE civs (regardless if you own Rise of the Rajas or not)
3. If you don't own Rise of the Rajas (RotR), you won't have access to RotR civs.

In multiplayer, there will be three different Data Mods available, WololoKingdoms (with all civs), as well as WololoKingdoms FE and WololoKingdoms AK with limited civ pools. You will have to use one of the latter ones if you do not own all DLCs.

It's heavily recommended to get all 3 DLCs though to be able to join all games.

## How to use (with Voobly)
[Download the latest release](https://github.com/AoE2CommunityGitHub/WololoKingdoms/releases), extract the Zip, make sure Steam is running and launch the WololoKingdoms.exe inside (It should find your HD installation automatically, otherwise copy the entire WololoKingdoms folder into your Age2HD folder and try again).
You can then use it as a mod in Voobly (To do that, click the "Settings" button next to "Launch", click the "Mod" checkbox, then select "WololoKingdoms". Make sure to select Userpatch 1.5 as the patch. You can then launch as normal).  

## How to use (without Voobly)
[Download the latest release](https://github.com/AoE2CommunityGitHub/WololoKingdoms/releases), extract it, make sure Steam is running and launch as usual.
Select the "Create an offline installation" checkbox and let the program run.
You can launch the game using the "WK.exe" in the "age2_x1/" folder.
Rarely there can be issues with creating the language_x1_p1.dll file. Usually when you get an error message regarding this, restarting the program and running it again is enough.
If this error happens every time, your original language file may be corrupt.

## How to use (on Linux)
You can install Age of Empires 2 HD on Linux by enabling Steam Play for unsupported games. Open Steam, and in the top left menu select Steam » Settings » Steam Play. There is a checkbox under "Advanced" to enable Steam Play for all titles. Restart Steam and install AoE2:HD.
You must also install AoC 1.0c in Wine first.
[Download the latest WololoKingdoms release](https://github.com/AoE2CommunityGitHub/WololoKingdoms/releases), extract it, make sure Steam is running and launch `WololoKingdoms.sh`. It will attempt to find your Wine-based AoC installation.

> Linux is not a primary supported platform for WololoKingdoms. The latest releases may not always work.

> The Linux version of WololoKingdoms is currently not being tested with Voobly. If you're lucky it _may_ work.

## Options

The options in the installer have a "Help" Icon to explain them, but they will also be explained here in more detail:

- Language: Here you can choose the language you want your game to be in. It's possible that for use without Voobly, there might be some issues with special characters that aren't in the english alphabet.
	- If there is a translation file available, this will also change the language of the installer window. Currently, only Simplified Chinese has been translated. If you want the option available for your native language, you could volunteer to translate them
- Install Directory: This should automatically detect your installation of the original game, if it exists. If for some reason this doesn't work, or you have multiple installations you want to choose from, you can select the installation directory here.
	- Choose the main folder (so that /age2_x1 is a subfolder), or the installer won't recognize it.
- Voobly Installation/Offline Installation/Both: You can choose to install this as a Voobly Data Mod, as a seperate WK.exe for use without Voobly ("offline"), or both. During the offline installation, you will get a separate window for creation of the WK.exe.
	- Here you have several options to choose from. Refer to http://userpatch.aiscripters.net/guide.html for more information on those.
- Old Patch/Data Mod: For installation of additional data mods based on WK. Check the point on data mods below for more information
- Hotkeys: This dropdown is for choosing your hotkey file: You have several options:
	- Keep your current hotkey configuration: If you've installed WololoKingdoms before and want to keep your hotkeys as they are
	- Use current AoC/Voobly hotkeys: Use the hotkey set that's currently in use for regular AoC on Voobly. Note that if you choose this option, you might be missing some new hotkeys for units like Condotierros, Genitours etc. This depends on the version of the hotkey file
	- Use HD hotkeys for this mod only: This copies your hotkey file from your HD installation. Your hotkeys for AoC on Voobly are unaffected however
	- Use HD hotkeys for this mod and AoC: This changes your hotkeys for regular AoC as well to that of the HD edition.
	- Note that if you have an unusual configuration (If you've created/deleted different profiles in-game), the installer might not be able to identify the correct hotkey file. In that case you will have to fix it manually.
	- As a general recommendation for creating new hotkey sets, you can use http://aokhotkeys.appspot.com/. Make sure to use HD5.0+ for the version.
- Install Mods with restricted civ pools: People who do not own all DLCs on steam do not have access to the full WololoKingdoms mod. However, they can play separate versions of the mod with restricted civ pools (WololoKingdoms AK for all civs up to and including African Kingdoms, WololoKingdoms FE for all AoC and Forgotten Empires civs).
	- If you want to play with someone who only has such restricted access, you can check this option to install these mods as well. The installer uses symbolic links for those, so this doesn't need more space on your hard drive.
- Improve tooltip and tech tree descriptions: This replaces the extended tooltips that can be enabled with F1. Also affects the descriptions in the Tech Tree. This offers significantly more information than the default descriptions, including stuff like attack speed, attack bonuses and so on.
	- For more information you can also check the steam mod page: https://steamcommunity.com/sharedfiles/filedetails/?id=649570924
- Copy Real World and Special Maps: If you want to use the Real World or Special Maps included in the expansions. Unlike on HD, you will find these in the "custom" dropdown. Most of these maps will have a "ZR@"-prefix for technical reasons. Maps with rw_ are real world maps, maps with sm_ special maps.
- Try converting custom HD maps: The installer can try to convert maps in your HD custom maps folder for use with WololoKingdoms. If the map uses no expansion terrains, it can be just copied, but for expansion terrains, it will create a special ZR@ map. Also note that under certain circumstances, an accurate conversion may not be possible.
	- This option takes a long time if you have a lot of custom maps. If you just want to convert a few specific maps, try temporarily moving the other maps while you let the installer run.
- Adjust garrison flags for new architectures: This is intended for use with this mod: https://www.voobly.com/gamemods/mod/858/WK-Architectures-showcase. It shifts the position of garrison flags to fit the changed architecture. If you don't use this mod, uncheck this option
- No Snow/Small Trees/Grid/Regional Monks/Short Walls: These are all graphics mods that can be used with or without Voobly. You can still override these with other Voobly Mods. To remove them, you need to run the installer again with the option unchecked.
	- Regional Monks gives monks of certain regions (Asia/Middle East/Africa,India) a different graphical appearance.
- If there are any Voobly Graphic or Sound Mods that you want to use offline (or if you want to make some of them permanent to reduce the number of voobly mods in use), you can place the files of the mod in the "mod_override" folder and run the installer. These files will automatically replace regular game files. To undo this, you need to remove the files from the folder and run the installer again. 
	- This can e.g. be used for a different grid mod, using some of the architecture mods offline, etc.

## Data Mods or old patches

WololoKingdoms has the option to install additional data mods with changed balance. To install any of these, make sure you have the regular version of WK installed, then click the Old Patch/Data Mod Checkbox, select the desired mod from the dropdown and click "Run".

There are two types of data mods:
- The "Patch/Installer x.x, Koth5 and Titans of Arina" data mods are intended for watching older recorded games played on a different balance. You might have to compare the date of the game with patch dates to find what patch you need to use.
	Newer recorded files have the game version they were played on saved in resource 198, which rec analyzers could read.
	You might also potentially have to choose an older version of Userpatch 1.5
- Other mods offer gameplay changes from regular WololoKingdoms: The King of the Hippo Mod gives the center monument a resource trickle of 1 Wood, Food, Gold and Stone per second.
	x3 and x9 Tech Mods allow each technology to be researched 3/9 times for some crazy games.
	Additional Editor Units is for Scenario Designers, showing all units in the editor with improved descriptions.

With the exception of older patches for rec viewing, you need to own all DLCs to be able to install a data mod.

It's also possible for other data mods based on WK to be added to the installer. Best create an issue with your request on the github page.

## Troubleshooting/Known issues/FAQ

I've done my best to make the installer stable, but sometimes it can still have issues.
Generally, if you encounter problems: Make sure you run the installer as administrator (right click the exe -> run as administrator). If you get a message about an exception, often just closing the installer and running it again helps
- **Q**: The installer can't read/write the language_x1_p1.dll when creating an offline installation! What's going on?
**A**: Creating a language_x1_p1.dll language file for offline use without Voobly can be unstable unfortunately. You may need to rerun it a few times, but it should eventually work. 
If you don't need to use an offline installation though, make sure that checkbox is unchecked, to avoid potential issues.
- **Q**: Why is my interface is messed up? It looks like this: https://imgur.com/a/xouXF
**A**: This happens if you reinstalled an older version of the compatibility patch after installing WololoKingdoms. To fix it, you can go into the Age2HD folder and delete the "compatslp" subfolder.

## New maps

There is a number of new maps. Many of these have a ZR@ prefix, which replace the graphics of some terrains.

- ZR@sm_ maps are the special maps from the HD expansions
- ZR@rw_ maps are the real world maps from the HD expansions
- ZR@CtR maps are capture the relic maps. When playing these, make sure to select "Relics" as the victory condition
- SD_ maps are sudden death maps, with only one TC being allowed, and losing that will make you lose the game.
- NoCut_Black_Forest is a concept map, showing off the possibility to disable regular Onager cutting and enabling mangonel/onager cutting with a seperate imperial age technology.

## Regional Monks, Independent Architecture, Custom Mods

The WololoKingdoms supports Independent Architecture like this HD Data Mod: http://steamcommunity.com/sharedfiles/filedetails/?id=893953860
To replace a civ's architecture, download an IA mod for that civ from the steam workshop, find the folder you just downloaded in the steam workshop folder (usually C:\Program Files (x86)\Steam\steamapps\workshop\content\221380) and copy that folder.
You can then paste it into the /mod_override folder you'll find next to the installer.
Alternatively you can use a Voobly mod like this one: http://www.voobly.com/gamemods/mod/858/WK-Architectures-showcase
You can also paste any graphics/sound mod you want to use offline into this folder.

There's also a "new_terrain_override" folder, should you wish to use different terrain slps for the expansion terrains. You can look at "/resources/new terrains" to see the naming convention.

There are also additional monk skins for Middle Eastern/African and Asian Monks. If you don't like these, you can uncheck "Regional monks" in the installer.

## Regional Units

The Independent Architecture concept has been expanded to units. The default unit looks are unchanged, but open for modding now. SLP numbers for modding are simple: Take the existing SLP number and add a value depending on the civ group. The values are:
- (0: Default)
- 60000: Goths, Teutons, Vikings (Central European)
- 70000: Byzantines, Slavs (East European/Orthodox)
- 80000: Spanish, Italian, Portuguese (Mediterranean)
- 90000: Japanese
- 100000: Chinese, Koreans (East Asian)
- 110000: Burmese, Vietnamese, Malay, Khmer (Southeast Asian)
- 120000: Turks, Persians, Saracens, Berbers (Middle Eastern)
- 130000: Indians
- 140000: Ethopians, Malians (African)
- 150000: Incas, Aztecs, Mayans (American)
- 160000: Huns, Mongols (Steppe)
- 170000: Magyars

## Terrains

To allow for maps with the new expansion terrains, some changes have been made. These terrains have changed from AoC:

- DLC_MANGROVESHALLOWS replaces Dirt 2 (Terrain nr. 11). To get the Dirt 2 look, use the Building Residue terrain (Nr. 27)
- CRACKEDIT (Cracked Earth) replaces Snow Road (Terrain nr. 38). This terrain has a damage multiplier, all buildings placed on it will take 1.2x of the regular damage.
- Snow Road replaces Snow Dirt (Terrain nr. 33). To get the Snow Dirt look, use the Snow Building Residue terrain (Nr. 36)
- DLC_MANGROVEFOREST replaces Oak Forest (Terrain nr. 20). Use regular forest (Nr. 10) instead, it looks identical (apart from the name of the trees).
- BAOBAB replaces Old Grass (Terrain nr. 16). This terrain was only used as the base terrain for cliffs. If you want to change the look of the cliff terrain, you can replace terrain nr. 16 with any other terrain in the rms script.
- ACACIA_FOREST is now Terrain nr. 41 (previously unused).
- The three Ice Terrains (Shallow Ice/26, Solid Ice/35 and Ice Beach/37) now use different SLP files. The default appearance is the same, but it allows for terrain mods that distinguish between the terrains, e.g. to make it visible where ships can and can't go. You can see this in use when selecting the "No snow" option
	- Shallow Ice/26 is 15024.slp, Solid Ice/35 is 15020.slp and Ice Beach/37 is 15034.slp

## Extra features

WololoKingdoms has some additional unique features:

- There are new "Resource Workshop" buildings, which have a resource trickle like a feitoria for map makers to use.
	- IDs 1300-1303: Auto-Convertible Food, Wood, Stone Gold Workshops
	- IDs 1304-1307: Regular Food, Wood, Stone Gold Workshops
	- 1308: Auto-Convertible Workshop with all four resource trickles
	- 1309: Regular Workshop with all four resource trickles.
	- All resource trickles are at a rate of 1/s. The speed of the trickle can be adjusted globally (for all such buildings on the map, including regular Feitorias) by changing the amount of resources 205-208 (Food, Wood, Stone, Gold) with effect_amount statements or corresponding triggers.

- There is an option to disable fire galleys and demo rafts in feudal (with fire ship/demo ship not needing the war galley research) with rms scripting. There is an AOC_Islands.rms map included that demonstrates this. (Disabling Tech 604 disables Fire Galleys, Tech 605 Demo Rafts. Disabling Tech 168 enables Fire Ships in Castle, and disabling Tech 170 enables Demo Ships in Castle
- There is an option to enable the X-Patch Trade cart change (In short: Trade Carts are twice as effective, but cost 2 pop. This is intended to reduce the number of units in the lategame and lessen lag due to fewer pathfinding calculation). To do so, disable Tech 88.
- There is an option to move coinage/banking techs:
	- Disable Tech 17 to remove Banking, Tech 23 to remove Coinage. Disable Tech 223 to get Castle Age Coinage, Tech 224 for Imperial Age coinage and tech 225 for Imperial Age Banking.
- Added XP_Black_Forest.rms to demonstrate this.


## Limitations
Because of the features missing from the original AOC engine, not everything from the expansions can be replicated, this includes :
- As of now, the aim of this program is multiplayer random maps, converting campaigns etc. isn't planned. (It should be fine to play them on HD)
- The hotkey menu is hardcoded, but this shouldn't be a big deal, as most of the new units are either unique units, or in the existing unit lines. Use the following hotkeys:
	- Camel hotkey for Battle Elephants
	- All other hotkeys should work as on HD
	- Note that on Userpatch, it is not possible to change the hotkeys for control groups. The hotkey menu might show this option, but it is not functional.
- Everything else should be working identically to the HD version, but it's possible I've overlooked something.

## Keep in mind
The Forgotten team has made a fantastic work with their expansions and, while this is probably not the way they expected their expansions to be played, I hope they can understand that many people are disapointed with the current state of AOE2HD engine.
If you want to share this with other people so you can play together, send them this page, everyone needs to generate their files by themselves using this program and the files from their copy of AOE2HD with the expansions. If there are problems with this, report the issue and I will try to fix it.

## How to build

The project is made of two parts: a library `libwololokingdoms` and the installer application. The library creates a `libwololokingdoms.dll` that contains the conversion logic and is used by the installer application.

You need a C++17 capable compiler because this project uses the new `<filesystem>` API.

Some dependencies are added as git submodules. To get them, do:

```bash
git submodule init
git submodule update --recursive
```

### Installer Application

The installer application is a Qt app. You can use Qt Creator to build it, or use the Qt command line tools and Make.

The installer requires the Steamworks SDK, which it uses to check if and where HD Edition is installed. Get it from [Steam](https://partner.steamgames.com/doc/gettingstarted).

To build the project on the command line:

```bash
qmake WololoKingdoms.pro
make
```

### libwololokingdoms

libwololokingdoms is built automatically when building the installer application. To build it standalone, see the [libwololokingdoms readme](./libwololokingdoms/README.md).

## Special thanks
- Tails8521 for the African Kingdoms version of this installer. Without him this would never have happened.
- Jineapple for reviving this program when Rise of Rajas came out, for all the countless hours he spent programming and fixing bugs for the WK experience to be as good (if not better) as HD.
- Trirem for taking care of the project after Jineapple stepped down
- The UserPatch team for making the new bonuses possible to use and all other great improvements that Userpatch brings.
- The folks at Voobly for the support offered and a great platform to play the mod on.
- Apre, Tapsa and sandsmark for the genieutils library
- Tapsa for Advanced Genie Editor
- Alqualunte and TWest for advices and testing
- The AI scripters that have made their AIs compatible with this project, among them CheeseOnToast, TheMax and Promiskuitiv
- Phoenix1vs for converting the interfaces for AK and RotR civilizations.
- Saint_Michaels for creating the Independent Architecture mod on Steam which served as a template for this function on WololoKingdoms.
- Shulker for creating an in-game logo for WololoKingdoms
- The Forgotten Empires team for three great expansions!

## License

[GPL-3.0](./COPYING)
