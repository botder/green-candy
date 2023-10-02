{Read 'copying.txt' before using this

   -= San Andreas Vehicle Special Abilities Editor v1.1 Source =-
    Creator : Alexander / Alexander Blade / Alexander Koryakin
    ICQ 242-045-282 , Mail : VbrMdi451@Yahoo.com ,
    http://Alexander.SannyBuilder.com
    © Alexander 2007-2008
 }

library VehSpecAbilities;

uses
  SysUtils, Classes, windows, IniFiles;

{$R *.res}

var
  OldProtect, i, Count, HWND : cardinal;
  CodePtr : Pointer;
  ParamsFile : TIniFile;
  F : TextFile;
  StrName : String;
  S : array [0..1000] of string;

function ReadIntValue(Section,Name:string):integer;
begin
  Result := 0;
  ParamsFile := TIniFile.Create(ExpandFileName('VSAConfig.ini'));
  Result := StrToInt(ParamsFile.ReadString(Section,Name,''));
  ParamsFile.Destroy;
end;

begin       // like DllProcessMain

  HWND := FileOpen('gta_sa.exe',GENERIC_READ);
  Count := GetFileSize(HWND,nil);
  FileClose(HWND);
  if ReadIntValue('MAIN','CheckExeVersion') = 1 then
  if Count <> 14383616 then
  begin
    MessageBoxA(HWND,'Sorry, but this version of gta_sa.exe is not supported'+#13+
                     'Only v1.0us (14383616 bytes) is supported now .'+#13+#13+
                     '              http://Alexander.SannyBuilder.com'+#13+
                     '                         (C) Alexander 2007',
                     '[SA] Vehicle Special Abilities Editor. ERROR !',0);
    ExitProcess(HWND);
  end;     

  AssignFile(F,ExpandFileName('VSAConfig.ini'));
  
{DOZER}
  Reset(F);
  
  {MOOVE_HOOK_1}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6AC40C),5,PAGE_READWRITE,OldProtect);
  PByte($6AC40C)^:= $E9;
  PInteger($6AC40D)^:= Integer(CodePtr) - $6AC40C - 5;

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[BUCKETs]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','BUCKETs');
  
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $E6;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6AC43B
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6AC43B - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6AC412
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6AC412 - (Integer(CodePtr)+Count*6+15);

  {MOOVE_HOOK_2}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6B1F95),5,PAGE_READWRITE,OldProtect);
  PByte($6B1F95)^:= $E9;
  PInteger($6B1F96)^:= Integer(CodePtr) - $6B1F95 - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $E6;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6B1F9B
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6B1F9B - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6B200F
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6B200F - (Integer(CodePtr)+Count*6+15);

  {CAMERA_&_COLLISION_HOOK}
  VirtualProtect(ptr($6A173F),20,PAGE_READWRITE,OldProtect);
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;

  PByte($6A173F)^:= $89;
  PByte($6A1740)^:= $9C;
  PByte($6A1741)^:= $24;
  PByte($6A1742)^:= $F8;
  PByte($6A1743)^:= $00;
  PByte($6A1744)^:= $00;
  PByte($6A1745)^:= $00;
  PByte($6A1746)^:= $E9;
  PInteger($6A1747)^:= Integer(CodePtr) - $6A1746 - 5;

  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $E6;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6A179C
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6A179C - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6A174C
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6A174C - (Integer(CodePtr)+Count*6+15);

  CloseFile(F);
{DOZER_END}

{CEMENT}
  Reset(F);

  {MOOVE_HOOK_1}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6AC43B),5,PAGE_READWRITE,OldProtect);
  PByte($6AC43B)^:= $E9;
  PInteger($6AC43C)^:= Integer(CodePtr) - $6AC43B - 5;

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[CISTERNs]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','CISTERNs');
  
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $0C;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6AC4D9
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6AC4D9 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6AC445
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6AC445 - (Integer(CodePtr)+Count*6+15);

  {MOOVE_HOOK_2}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6B1FA1),5,PAGE_READWRITE,OldProtect);
  PByte($6B1FA1)^:= $E9;
  PInteger($6B1FA2)^:= Integer(CodePtr) - $6B1FA1 - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $0C;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6B1FA7
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6B1FA7 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6B200F
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6B200F - (Integer(CodePtr)+Count*6+15);

  CloseFile(F);
{CEMENT_END}

                   
{PACKER}
  Reset(F);

  {MOOVE_HOOK_1}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6AC4D9),5,PAGE_READWRITE,OldProtect);
  PByte($6AC4D9)^:= $E9;
  PInteger($6AC4DA)^:= Integer(CodePtr) - $6AC4D9 - 5;

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[PACKERs]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','PACKERs');
  
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $BB;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6AC507
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6AC507 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6AC4DF
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6AC4DF - (Integer(CodePtr)+Count*6+15);


  {MOOVE_HOOK_2}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6B1F8F),5,PAGE_READWRITE,OldProtect);
  PByte($6B1F8F)^:= $E9;
  PInteger($6B1F90)^:= Integer(CodePtr) - $6B1F8F - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $BB;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6B1F95
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6B1F95 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6B200F
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6B200F - (Integer(CodePtr)+Count*6+15);

  CloseFile(F);
{PACKER_END}


{TOWTRUCK}
  Reset(F);

  {MOOVE_HOOK_1}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6AC507),5,PAGE_READWRITE,OldProtect);
  PByte($6AC507)^:= $E9;
  PInteger($6AC508)^:= Integer(CodePtr) - $6AC507 - 5;

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[TRUCK_HOOKs]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','TRUCK_HOOKs');
  
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;


  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $0D;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6AC6D9
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6AC6D9 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6AC511
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6AC511 - (Integer(CodePtr)+Count*6+15);


  {MOOVE_HOOK_2}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6B1FB3),5,PAGE_READWRITE,OldProtect);
  PByte($6B1FB3)^:= $E9;
  PInteger($6B1FB4)^:= Integer(CodePtr) - $6B1FB3 - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;         
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $0D;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6B1FB9
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6B1FB9 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6B2006
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6B2006 - (Integer(CodePtr)+Count*6+15);

  {CAR_HOOKING}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6AF257),6,PAGE_READWRITE,OldProtect);
  PByte($6AF257)^:= $56;
  PByte($6AF258)^:= $E9;
  PInteger($6AF259)^:= Integer(CodePtr) - $6AF258 - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $0D;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6AF262  //6AF26C
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6AF262 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6AF33B
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6AF33B - (Integer(CodePtr)+Count*6+15);

  {CAR_HOOK_MOOVING}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6B0EE6),5,PAGE_READWRITE,OldProtect);
  PByte($6B0EE6)^:= $E9;
  PInteger($6B0EE7)^:= Integer(CodePtr) - $6B0EE6 - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $0D;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6B0F0F
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6B0F0F - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6B0EEC
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6B0EEC - (Integer(CodePtr)+Count*6+15);

  {CAR_UNHOOKING}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6DFDB6),5,PAGE_READWRITE,OldProtect);
  PByte($6DFDB6)^:= $E9;
  PInteger($6DFDB7)^:= Integer(CodePtr) - $6DFDB6 - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $0D;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6DFDBC
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6DFDBC - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6DFDC2
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6DFDC2 - (Integer(CodePtr)+Count*6+15);

  {CAR_MASS}
  VirtualProtect(ptr($6E00F8),2,PAGE_READWRITE,OldProtect);
  PByte($6E00F8)^:= $EB; // jmp
  PByte($6E00F9)^:= $06; // +6

  CloseFile(F);
{TOWTRUCK_END}


{TRACTOR}
  Reset(F);

  {MOOVE_HOOK_1}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6AC6D9),5,PAGE_READWRITE,OldProtect);
  PByte($6AC6D9)^:= $E9;
  PInteger($6AC6DA)^:= Integer(CodePtr) - $6AC6D9 - 5;

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[TRACTOR_HOOKs]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','TRACTOR_HOOKs');

  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;  // cmp
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;  // ax
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]); // id
    PByte(Integer(CodePtr)+i*6+4)^:= $74; // jz
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $13;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6AC71C
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6AC71C - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6AC6DF
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6AC6DF - (Integer(CodePtr)+Count*6+15);


  {MOOVE_HOOK_2}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6B1FB9),5,PAGE_READWRITE,OldProtect);
  PByte($6B1FB9)^:= $E9;
  PInteger($6B1FBA)^:= Integer(CodePtr) - $6B1FB9 - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $13;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6B1FBF
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6B1FBF - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6B2006
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6B2006 - (Integer(CodePtr)+Count*6+15);

  {CAR_HOOK_MOOVING}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6B0F0F),5,PAGE_READWRITE,OldProtect);
  PByte($6B0F0F)^:= $E9;
  PInteger($6B0F10)^:= Integer(CodePtr) - $6B0F0F - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $13;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6B0F3B
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6B0F3B - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6B0F15
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6B0F15 - (Integer(CodePtr)+Count*6+15);

  {CAR_HOOKING}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6AF262),5,PAGE_READWRITE,OldProtect);
  PByte($6AF262)^:= $E9;
  PInteger($6AF263)^:= Integer(CodePtr) - $6AF262 - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $13;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6AF26C
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6AF26C - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6AF33B
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6AF33B - (Integer(CodePtr)+Count*6+15);

  {CAR_UNHOOKING}
  CodePtr := VirtualAlloc(0, 1000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6DFDBC),5,PAGE_READWRITE,OldProtect);
  PByte($6DFDBC)^:= $E9;
  PInteger($6DFDBD)^:= Integer(CodePtr) - $6DFDBC - 5;
  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;
  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $13;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6DFDC0
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6DFDDB - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6DFDC2
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6DFDC2 - (Integer(CodePtr)+Count*6+15);
          
  CloseFile(F);
{TRACTOR_END}

{TRUE_HYDRALICS}
  Reset(F);

  {FIRST'n'LAST Hook}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6ACA37),5,PAGE_READWRITE,OldProtect);
  PByte($6ACA37)^:= $E9;
  PInteger($6ACA38)^:= Integer(CodePtr) - $6ACA37 - 5;

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[ADV_HYDRALICs]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','ADV_HYDRALICs');
  
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $B2;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6ACA4B
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6ACA4B - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6ACADE
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6ACADE - (Integer(CodePtr)+Count*6+15);

  CloseFile(F);
{TRUE_HYDRALICS_END}

{WATER_JETs}
  Reset(F);

  {FIRST'n'LAST Hook}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6B1F4F),5,PAGE_READWRITE,OldProtect);
  PByte($6B1F4F)^:= $E9;     // jmp
  PInteger($6B1F50)^:= Integer(CodePtr) - $6B1F4F - 5;   // Alloced place

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[WATER_JETs]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','WATER_JETs');
  
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $97;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6B1F77
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6B1F77 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6B1F5B
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6B1F5B - (Integer(CodePtr)+Count*6+15);

  CloseFile(F);
{WATER_JETs_END}

{RHINO'n'SWATVAN_TURRETs}
  Reset(F);

  {FIRST'n'LAST Hook}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6ACA4B),5,PAGE_READWRITE,OldProtect);
  PByte($6ACA4B)^:= $E9;     // jmp
  PInteger($6ACA4C)^:= Integer(CodePtr) - $6ACA4B - 5;   // Alloced place

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[TURRETs_1]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','TURRETs_1');
  
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $59;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6ACA57
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6ACA57 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6ACAAE
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6ACAAE - (Integer(CodePtr)+Count*6+15);

  CloseFile(F);
{RHINO'n'SWATVAN_TURRETs_END}


{FTUCK_TURRETs}
  Reset(F);

  {FIRST'n'LAST Hook}
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6ACA57),5,PAGE_READWRITE,OldProtect);
  PByte($6ACA57)^:= $E9;     // jmp
  PInteger($6ACA58)^:= Integer(CodePtr) - $6ACA57 - 5;   // Alloced place

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[TURRETs_2]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','TURRETs_2');
  
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $97;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6ACA8D
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6ACA8D - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6ACA5D
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6ACA5D - (Integer(CodePtr)+Count*6+15);

  CloseFile(F);
{FTRUCK_TURRETs_END}

{ZR 350 UP/DOWN LIGHTS}
  Count:= ReadIntValue('MAIN','UP/DOWN_LIGHTS');
  Reset(F);

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[UP/DOWN_LIGHTS]')=nil) then break;
  end;

  VirtualProtect(ptr($6ACA8D),5,PAGE_READWRITE,OldProtect);
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  PByte($6ACA8D)^:= $E9;
  PInteger($6ACA8E)^:= Integer(CodePtr) - $6ACA92;
 
  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  CloseFile(F);

  PByte(Integer(CodePtr)+Count*6)^:= $66;
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D;
  PByte(Integer(CodePtr)+Count*6+2)^:= $DD;
  PByte(Integer(CodePtr)+Count*6+3)^:= $01;
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6ACBC7 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6ACA97 - (Integer(CodePtr)+Count*6+15);

  VirtualProtect(ptr($6E1C17),5,PAGE_READWRITE,OldProtect);
  CodePtr := VirtualAlloc(0, 10000 , MEM_COMMIT , PAGE_READWRITE) ;
  PByte($6E1C17)^:= $E9;
  PInteger($6E1C18)^:= Integer(CodePtr) - $6E1C1C;

  For i:= 0 to Count-1 do
  begin
    PByte(Integer(CodePtr)+i*8)^:= $66;
    PByte(Integer(CodePtr)+i*8+1)^:= $81;
    PByte(Integer(CodePtr)+i*8+2)^:= $7E;
    PByte(Integer(CodePtr)+i*8+3)^:= $22;
    PSmallInt(Integer(CodePtr)+i*8+4)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*8+6)^:= $74;
    PByte(Integer(CodePtr)+i*8+7)^:= (Count-i-1)*8 + $C;
  end;
  PByte(Integer(CodePtr)+Count*8)^:= $66;
  PByte(Integer(CodePtr)+Count*8+1)^:= $81;
  PByte(Integer(CodePtr)+Count*8+2)^:= $7E;
  PByte(Integer(CodePtr)+Count*8+3)^:= $22;
  PByte(Integer(CodePtr)+Count*8+4)^:= $DD;
  PByte(Integer(CodePtr)+Count*8+5)^:= $01;

  PByte(Integer(CodePtr)+Count*8+6)^:= $0F;
  PByte(Integer(CodePtr)+Count*8+7)^:= $85;
  PInteger(Integer(CodePtr)+Count*8+8)^:= $6E1CCE - (Integer(CodePtr)+Count*8+12);
  PByte(Integer(CodePtr)+Count*8+12)^:= $E9;
  PInteger(Integer(CodePtr)+Count*8+13)^:= $6E1C23 - (Integer(CodePtr)+Count*8+17);
{END ZR 350}

{TRAILER_HOOKs}
  Reset(F);

  {FIRST'n'LAST Hook}
  CodePtr := VirtualAlloc(0, 1000 , MEM_COMMIT , PAGE_READWRITE) ;
  VirtualProtect(ptr($6AF26C),5,PAGE_READWRITE,OldProtect);
  PByte($6AF26C)^:= $E9;     // jmp
  PInteger($6AF26D)^:= Integer(CodePtr) - $6AF26C - 5;   // Alloced place

  While not EOF(F) do
  begin
    ReadLn(F,StrName);
    if not (strpos(PChar(StrName),'[TRAILER_HOOKs]')=nil) then break;
  end;

  Count:= ReadIntValue('MAIN','TRAILER_HOOKs');

  For i:= 0 to Count-1 do
  begin
    ReadLn(F,S[i]);
    PByte(Integer(CodePtr)+i*6)^:= $66;
    PByte(Integer(CodePtr)+i*6+1)^:= $3D;
    PSmallInt(Integer(CodePtr)+i*6+2)^:= StrToInt(S[i]);
    PByte(Integer(CodePtr)+i*6+4)^:= $74;
    PByte(Integer(CodePtr)+i*6+5)^:= (Count-i-1)*6 + $A;
  end;

  PByte(Integer(CodePtr)+Count*6)^:= $66;  // cmp
  PByte(Integer(CodePtr)+Count*6+1)^:= $3D; // ax
  PByte(Integer(CodePtr)+Count*6+2)^:= $02;   // id
  PByte(Integer(CodePtr)+Count*6+3)^:= $02;   // 2 b
  PByte(Integer(CodePtr)+Count*6+4)^:= $0F;  // jnz
  PByte(Integer(CodePtr)+Count*6+5)^:= $85;  // $6AF284
  PInteger(Integer(CodePtr)+Count*6+6)^:= $6AF272 - (Integer(CodePtr)+Count*6+10);
  PByte(Integer(CodePtr)+Count*6+10)^:= $E9;   // jmp $6AF2CC
  PInteger(Integer(CodePtr)+Count*6+11)^:= $6AF2CC - (Integer(CodePtr)+Count*6+15);

  CloseFile(F);
{TRAILER_HOOKs_END}

end.
