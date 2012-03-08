Slaves to Armok: God of Blood
Chapter II: Dwarf Fortress


*** COPYRIGHT INFORMATION ****************************

Copyright (c) 2002-2012.  All rights are retained by Tarn Adams, save the following:  you may redistribute the binary and accompanying files, unmodified, provided you do so free of charge.  If you'd like to distribute a modified version of the game or portion of the archive and are worried about copyright infringement, please contact Tarn Adams at toadyone@bay12games.com.

This software is still in development, and this means that there are going to be problems, including serious problems that, however unlikely, might damage your system or the information stored on it.  Please be aware of this before playing.


*** NOTES ********************************************

For help with gameplay, press the help key (initially '?') after starting the game.  You might have to press ESC and use 2/8 to select keybindings on non-US keyboards in order to get the help key to work properly.

As of March 2012, you can also get help at the fan-created dwarffortresswiki.org.  Please make use of and contribute to this valuable resource.

Visit www.bay12games.com to report problems, discuss the game and read information on future development.  There are also other games there.
All of the games there are free.  If you paid for this game, get the latest version for free at bay12games.com, and let me know who was charging you at tarn.adams@gmail.com.

See release_notes.txt for information on handling saves and a brief writeup on the changes for this version.
See file_changes.txt for new init/interface information.
See command_line.txt for information on world generation from command lines.


*** SUPPORT BAY 12 ***********************************

Bay 12 Games is fan-supported.  We've put a lot of time and effort into these games, which are all freely distributed, but we can't survive on your feedback alone.  Donations are appreciated.  Visit www.bay12games.com/support.html to see how you can help.


*** CREDITS ******************************************

Programming: Tarn Adams

Design: Tarn and Zach Adams

Music: Tarn Adams

Initial Playtesting: Alan Ames, Alex Mooney, Martin Packman, Ben Ringland and Zonk

Lots of HTML/IT stuff: Martin Packman

First Mac port: Matthew Moss and eli

SDL port, and additional changes: Bernard Helyer (see license below)

OpenGL upgrade to this millenium, and additional changes: Svein Ove Aas (see license below)

Additional changes to the SDL Port and related main loop/input code: Theodorick Grau (see license below)

DF 2010 Playtesting: Svein Ove Aas, Aquillion, Kyle Brodzky, Richard Jensen, Nathan Miller aka Rainseeker, Alex Mooney, Ben Ringland, Chris Rogers

Bug Tracker Managers: Anthony Drawdy, Knight Otu, Logical2u, Chris Rogers




Many suggestions from fans on the bay12games.com and other forums (and the #bay12games IRC channel) have been incorporated over the years.  The code contributions credited above that are in use by the game are distributed with the Linux version (see the g_src directory and licenses below).


*** OPTIPNG ******************************************

optipng is bundled into the zip for use to change BMPs into PNGs easily, at least until PNG support is added to DF itself.  The license is included in this folder.  You can get the optipng documentation and source at http://optipng.sourceforge.net.  You can use the compress_bitmaps batch file to convert any BMP in the folder to PNG.  The BMP files are deleted afterward.


*** NOTES ON MODIFICATIONS ***************************

Existing save files will become unusable if any raw/objects entries are removed.  If you change the configuration of the bodies of existing creatures, save files may become unstable.  New additions to the files are not incorporated into existing worlds but will be added to the next world you generate.

We do not accept submissions for the official Dwarf Fortress releases.  Please do not ask for your code, art, sound or raw/objects txt files to be incorporated into the official releases.


*** SDL PORT/OPENGL UPGRADE LICENSE ******************

The game uses code by Svein Ove Aas (OpenGL upgrade) and Bernard Helyer (who did the SDL port).  This code was distributed under the following license (and will be made available at some point at bay12games.com/games after it's tested a bit more):

Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard Helyer
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

*** Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*** Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*** The names of Tarn Adams, Zach Adams, Svein Ove Aas, Bernard Helyer and Bay 12 Games may not be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Additional changes to the SDL Port and related main loop/input code by Theodorick Grau, who provided the following license:

Full license, ownership, and copyright is hereby granted to Tarn Adams, Zach Adams, Bay 12 Games(represensations thereof), and all assignees they may designate; for all source code submitted by me to any of the above mentioned parties.

I retain no rights to any source code once it has been submitted.  I also make no guarantees as to the fitness or mechantibility of any source code.  All source code, submitted by me, and its binary products are wholly property of the above named parties.

Signed this day the 11th of January 2009,
Theodorick Grau


*** LIBRARIES ****************************************

DF uses the FMOD Sound System
Copyright (C) Firelight Technologies Pty, Ltd., 1994-2006.

The SDL version of DF uses SDL - Simple DirectMedia Layer
Copyright (C) 1997-2006 Sam Lantinga
See the SDL folder for license information.

This SDL version of DF uses The OpenGL Extension Wrangler Library.  What follows is the GLEW License.
*** start GLEW license ***
Copyright (C) 2002-2007, Milan Ikits <milan ikits[]ieee org>
Copyright (C) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>
Copyright (C) 2002, Lev Povalahev
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, 
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.
* The name of the author may be used to endorse or promote products 
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*** end GLEW license ***

Portions of the SDL version are copyright (c)2008 The FreeType Project (www.freetype.org).  All rights reserved.