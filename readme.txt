-Crimson Glory source code------------------------------------------------------------

Crimson Glory is officially open source. You can download it and do whatever you want with it,
 unless you want to use it commercially (more info in license.txt).

I want you to get the source code, educate yourself and maybe help me out with it.
Add stuff, modify, optimize, suggestions are welcome. I'm using this in my new game,
the structure of the engine and all of its core components like scripting and entities will be the same.

The basics you need to know are:

* the whole source is written in C++ ready to compile and go from Visual Studio 7 (it should run on any other compiler)
* there are 2 projects Crimson (the executable) and GameDLL (the DLL that is executed by the exe);
* basically the interesting stuff, meaning the whole game is in GameDLL
* the engine uses several subsystems:
  - graphics: Irrlicht (also used for geometry, including math and vectors)
  - sound: FreeSL (wrapper for OpenAL)
  - player animations: Cal3D
  - scripting: GameMonkey
  - network: RakNet
  - maps format: DeleD

The game runs on Windows, however it is possible to compile it on any system.
The problems I see are in the different components like FreeSL or Cal3D. 
In theory they should compile but I never tried it. 

If you have any questions, need help in finding what is where and what in the engine just e-mail me and I'll answer as best as I can.





-Crimson Glory release readme.txt------------------------------------------------------------

Crimson Glory by Michal Marcinkowski

------------------

Please read.

This is a final preview release. The development of this game will not be continued.
The author holds no responsibility for harm caused by this product.

Crimson Glory is not optimized at all. Therefore it will be slow on older systems. Fortunately there is a lot of options to speed up the graphics.

Game keys are displayed in menu [ESC] in Help section.
Console is shown using the tilda key [~].
Editor hot key [F].
Chat key [T].

List of all game variables is shown by typing "\list".
Most of the variables will require restarting the game.
Commands use "\" prefix. Script functions and variables use no prefix.

Adding bots:
type script function "addBot();" in console. Bots are most useful to add while airborn high in sky.

Config file is located in \Base\Scripts\config.gm and \Base\Scripts\autoconfig.gm.
Config.gm overwrites autoconfig.gm!
If you want to change the resolution or troubleshoot checkout config.gm.
If graphics do not work properly try changing v_driver = 4;, or any other number (help in config.gm).

------------------

Crimson Engine by Michal Marcinkowski (c) 2005-07; 
Graphics by SHEPEIRO and Snowfly; 
Music by Trompkins; 
To contact the author or any other team member write to: michal.marcinkowski@gmail.com
Get ready for MM's new game. More info on http://www.crimsonengine.com or http://www.soldat.pl