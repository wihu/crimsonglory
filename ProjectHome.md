Crimson Glory is a side-view air combat dogfighting game set in the post WWI mediterranean . The game is inspired by Miyazaki's movie Porco Rosso. The game and engine are developed by Michal Marcinkowski creator of Soldat. He is using the engine in his new game, the structure of the engine and all of its core components like scripting and entities will be the same.

The basics you need to know are:
The whole source is written in C++ ready to compile and go from Visual Studio 7 (it should run on any other compiler)
There are 2 projects Crimson (the executable) and GameDLL (the DLL that is executed by the exe); basically the interesting stuff, meaning the whole game is in GameDLL

The engine uses several subsystems:

- graphics: Irrlicht (also used for geometry, including math and vectors)
- sound: FreeSL (wrapper for OpenAL)
- player animations: Cal3D
- scripting: GameMonkey
- network: RakNet
- maps format: DeleD

The game runs on Windows, however it is possible to compile it on any system. The problems I see are in the different components like FreeSL or Cal3D.