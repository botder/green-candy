********** IMG and Stream Limit Adjuster 3.0 **********

This open source ASI plugin adjusts amount of IMG archives possible to load.
By default GTA San Andreas is able to load max of 8 archives (3 standard archives gta3.img, gta_int.img, player.img and 5 archives defined within default.dat or gta.dat)
It`s very inconvenient when making total conversion. More IMG archives means better organization.
Not only IMG limit is hacked - stream limit had to be hacked either.

Now limit is:
MAX_NUMBER_OF_IMG_ARCHIVES = 127;
MAX_NUMBER_OF_STREAM_HANDLES = 400;

It should be sufficient for anyone.

Also it adds support to read IMGLIST file defined within default.dat or gta.dat, like in GTA IV.
For example IMGLIST DATA\IMAGES.TXT

in images.txt:
anim\anim.img
models\paths.img
models\pedprops.img

Works with GTA San Andreas v1.0 [US] HOODLUM No-CD Fixed EXE
- - - - - - - - - - - - - - - - - -

***** Installation
- Many users complain it doesn't work with DEP enabled.
  In that case you may have to exclude gta_sa.exe from Data Execution Prevention in Windows. 

- Make sure you are using this version of gta_sa.exe:
	GTA San Andreas v1.0 [US] HOODLUM No-CD Fixed EXE:
	EXE size:  14 383 616 bytes
	
- You need to have ASI loader installed, it is already enabled if you installed CLEO Library.
Otherwise you can download ASI loader from here: http://www.gtagarage.com/mods/show.php?id=8321

- Put ImgLimitAdjuster.asi into GTA San Andreas directory.

- There are two ways to define new archives
  1. Open default.dat or gta.dat. Add entry like this:
  IMG models\weapons.img
  
  2. Open default.dat or gta.dat. Add entry like this:
  IMGLIST DATA\IMAGES.TXT
  
  Create images.txt and put entry:
  models\weapons.img
  
  Commented lines are possible, must begin with # sign.
  For example:
# comment

IMGLIST handling is introduced in IMG Limit Adjuster 1.0, inspired by GTA IV's IMGLIST.

Copyright (c) 2011, fastman92
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** Informations:
Date of release: 29-02-2012 (d-m-Y)
Author: fastman92
Version: 3.0
For: GTA San Andreas
E-mail: fastman92@gmail.com
Visit fastman92.tk