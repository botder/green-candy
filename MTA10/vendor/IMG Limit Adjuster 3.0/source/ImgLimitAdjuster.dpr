library ImgLimitAdjuster;

uses
  SysUtils,
  Classes,
  IniFileS,
  windows;

procedure NOP(Adress : integer; NumberOfBytes : byte);
var
EndAdress:integer;
OldProtect : cardinal;
begin
EndAdress := Adress + NumberOfBytes;
VirtualProtect(ptr(Adress),NumberOfBytes,PAGE_READWRITE,OldProtect);

  while Adress < EndAdress do
  begin
  PByte(Adress)^:= $90;
  Inc(Adress);
  end;
end;

{$E asi}

{$R *.res}
const
MAX_NUMBER_OF_IMG_ARCHIVES = 350;
MAX_NUMBER_OF_STREAM_HANDLES = 400;

Footer_comunique : string =
'                                     http://fastman92.tk'+#13+
'                                     (C) fastman92 2011';

type
CImgDescriptor = record
 name : Array [0..40] of Byte;
 isNotPlayerImg : Boolean;
 streamHandle : longint;
end;

type
_imgsArrayType = Array[0..MAX_NUMBER_OF_IMG_ARCHIVES-1] of CImgDescriptor;

_strncmp = function (S1, S2: PChar; MaxLen: Cardinal): Integer;

CFileMgr__Open = function (S1, S2: PChar): Integer cdecl;
CFileMgr__getConfigLine = function (FileHandle : Integer): PChar cdecl;
CFileMgr__CloseFile = procedure (FileHandle : Integer) cdecl;
_openImgFile = function (filepath : PChar) : Integer; cdecl;


var
CreatedCodePointer : Pointer;
ExeCodePointer : Pointer;
OldProtect : Cardinal;
_imgs : ^_imgsArrayType;
_streamHandles : Array [0..MAX_NUMBER_OF_STREAM_HANDLES-1] of longint;
_streamNames : Array [0..MAX_NUMBER_OF_STREAM_HANDLES-1] of array[0..63] of char;
ConfigINI : TIniFile;
i : longint;
_strncmpFunc : _strncmp = Pointer($008214D0);
CFileMgr__OpenFunc : CFileMgr__Open = Pointer($00538900);
CFileMgr__getConfigLineFunc : CFileMgr__getConfigLine = Pointer($00536F80);
CFileMgr__CloseFileFunc : CFileMgr__CloseFile = Pointer($005389D0);
_openImgFileFunc : _openImgFile = Pointer($00407610);

procedure readImgList(filepath : PChar); cdecl;
var
ImagesFileHandle : Integer;
asm
push offset @readMode;
push filepath;
call CFileMgr__OpenFunc;
add esp, 8;

mov   ImagesFileHandle, eax;

  @loop:

  push ImagesFileHandle;
  call CFileMgr__getConfigLineFunc;
  add esp, 4;

  test eax, eax;
  jz @return;
  mov esi, eax;

  mov al, [eax];

  test  al, al
  jz @loop;

  cmp   al, '#';
  je @loop;

  push  1;   // notPlayerFile
  push  esi; // lpFileName
  call  _openImgFileFunc;
  add esp, 8;
  jmp @loop;

@readMode: DB 'r', 00;
@return:
push ImagesFileHandle;
call CFileMgr__CloseFileFunc;
add esp, 4;
end;

begin
  IF PByte($400088)^ <> $CA then
  BEGIN
        MessageBoxA(0,PChar('Sorry, but this version of gta_sa.exe is not supported'+#13+
                     'Supported EXE:'+#13+#13+
                     '    1.  GTA San Andreas v1.0 [US] HOODLUM No-CD  Fixed EXE:'+#13+
                     '         EXE size:  14 383 616 bytes'+#13+#13+#13+
                      Footer_comunique),
                     'IMG Limit Adjuster 3.0', MB_OK);
        ExitProcess(0);
  END;

ConfigINI := TIniFile.Create(ExpandFileName('IMG_Manager.ini'));

  try
  i := ConfigINI.ReadInteger('MAIN', 'DisableLoadingOfHardcodedImgArchives', 0);
    if Boolean(i) then
    BEGIN
    NOP ($5BF991, 5);   // call    _openStandardImgArchives

    NOP ($5B9142, $16); // comparement with MODELS\\GTA_INT.IMG
    END;
  finally
  ConfigINI.Free;
  end;
// NOP 0x005BF991

//////////// IMG limit
_imgs := VirtualAlloc(NIL, sizeof(_imgsArrayType), MEM_COMMIT , PAGE_READWRITE);

{
Disable:
    openStream("MODELS\\GTA3.IMG");
    v4 = readStream(0, v3, 0, 1);
    closeAllStreams();
}
ExeCodePointer := Pointer($00406C28);
VirtualProtect(ExeCodePointer, 20, PAGE_READWRITE, OldProtect);

PByte(ExeCodePointer)^:= $A3;              // mov     _streams, eax
PInteger(Integer(ExeCodePointer)+1)^:= $008E3FFC;

PByte(Integer(ExeCodePointer)+5) ^:= $B8;  // mov eax,00000001
PInteger(Integer(ExeCodePointer)+6)^:= 1;

PWord(Integer(ExeCodePointer)+10)^:= $1EEB;   // jmp 00406C52

// _openImgFile
VirtualProtect(ptr($01567B90), $5D, PAGE_READWRITE, OldProtect);
PPointer($001567B94)^:= _imgs;    // mov     eax, offset _imgs

PPointer($01567BA2)^:= Pointer(Integer(_imgs)+sizeof(_imgsArrayType));  // cmp     eax, offset imgsEndAndUnknownByte

PPointer($01567BBA)^:= _imgs;    // lea     edx, _imgs.name[esi]

PPointer($01567BD6)^:= Pointer(Integer(_imgs)+$2C);  // mov     _imgs.streamHandle[esi], eax

PPointer($01567BE3)^:= Pointer(Integer(_imgs)+$28);  // mov     _imgs.isNotPlayerImg[esi], al

// _openStandardImgArchives
VirtualProtect(ptr($004083C0), $EF, PAGE_READWRITE, OldProtect);
PPointer($004083C1)^:= Pointer(Integer(_imgs)+$2C);   // mov     eax, offset _imgs.streamHandle

PPointer($004083DE)^:= Pointer(Integer(_imgs)+$2C+sizeof(_imgsArrayType));  // cmp     eax, (offset dword_8E4A60+24h)

PPointer($004083E9)^:= _imgs;   // mov     eax, offset _imgs

PPointer($004083FA)^:= Pointer(Integer(_imgs)+sizeof(_imgsArrayType));  // cmp     eax, offset imgsEndAndUnknownByte

PPointer($0040840B)^:= _imgs;   // mov     eax, offset _imgs

PPointer($0040841A)^:= Pointer(Integer(_imgs)+sizeof(_imgsArrayType));  // cmp     eax, offset imgsEndAndUnknownByte

PPointer($0040843B)^:= _imgs; // lea     edx, _imgs.name[ecx]

PPointer($0040845B)^:= Pointer(Integer(_imgs)+$2C); // mov     _imgs.streamHandle[esi], eax

PPointer($408461)^:= Pointer(Integer(_imgs)+$28); // mov     _imgs.isNotPlayerImg[esi], 1

PPointer($00408479)^:= _imgs;   // lea     edx, _imgs.name[edx]

PPointer($004084A2)^:= Pointer(Integer(_imgs)+$2C); // mov     _imgs.streamHandle[esi], eax

PPointer($004084A8)^:= Pointer(Integer(_imgs)+$28); // mov     _imgs.isNotPlayerImg[esi], 1

// CStreaming::loadArchives
VirtualProtect(ptr($005B82C0), $7A, PAGE_READWRITE, OldProtect);

PPointer($005B82F1)^:= _imgs;   // mov     al, _imgs.name

PPointer($005B82FD)^:= _imgs;   // mov     esi, offset _imgs

PPointer($005B8303)^:= Pointer(Integer(_imgs)+sizeof(_imgsArrayType));    // cmp     esi, offset imgsEndAndUnknownByte

// _sub_407570
VirtualProtect(ptr($00407570), $16, PAGE_READWRITE, OldProtect);

PPointer($0040757F)^:= Pointer(Integer(_imgs)+$2C);   // mov     eax, _imgs.streamHandle[eax]

// sub_408E20
VirtualProtect(ptr($00408E20), $230, PAGE_READWRITE, OldProtect);

PPointer($00408FDC)^:= Pointer(Integer(_imgs)+$2C); // mov     eax, _imgs.streamHandle[edx]

// CStreaming::requestSpecialModel
VirtualProtect(ptr($00409D10), $2D9, PAGE_READWRITE, OldProtect);

PPointer($0409D5A)^:= Pointer(Integer(_imgs)+$2C);    // mov     ecx, _imgs.streamHandle[ecx]

// sub_40CBA0
VirtualProtect(ptr($0040CBA0), $3DF, PAGE_READWRITE, OldProtect);
PPointer($0040CC54)^:= Pointer(Integer(_imgs)+$2C);   // mov     eax, _imgs.streamHandle[edx]

PPointer($0040CCC7)^:= Pointer(Integer(_imgs)+$2C);   // mov     ecx, _imgs.streamHandle[ecx]

// sub_4075A0
VirtualProtect(ptr($01560E50), $33, PAGE_READWRITE, OldProtect);

PPointer($01560E68)^:= Pointer(Integer(_imgs)+$2C); // mov     edx, _imgs.streamHandle[eax]

// sub_40A080
VirtualProtect(ptr($01560E50), $93, PAGE_READWRITE, OldProtect);
PPointer($015663E7)^:= Pointer(Integer(_imgs)+$2C);   // mov     eax, _imgs.streamHandle[edx]

//////////// Stream limit

// _GetFileSizeOfTheFirstStream
VirtualProtect(ptr($015700D0), $F, PAGE_READWRITE, OldProtect);

PPointer($015700D1)^:= @_streamHandles;   // mov     eax, _streamHandles

// _closeAllStreams
VirtualProtect(ptr($00406690), $76, PAGE_READWRITE, OldProtect);

PPointer($004066C7)^:= @_streamNames;   // mov     edi, offset _streamNames ;

PPointer($004066D6)^:= @_streamHandles;   // mov     eax, _streamHandles[esi*4]

PPointer($004066ED)^:= @_streamHandles;   // mov     _streamHandles[esi*4], ebx

// _sub_406710
VirtualProtect(ptr($406710), $40, PAGE_READWRITE, OldProtect);

PPointer($00406737)^:= @_streamHandles;   // mov     eax, _streamHandles[esi*4]

// _sub_406750
VirtualProtect(ptr($406750), $5D, PAGE_READWRITE, OldProtect);

PPointer($40676C)^:= @_streamNames;    // mov     esi, offset _streamNames

PPointer($406797)^:= @_streamHandles; // mov     _streamHandles[edi*4], eax

// _openStream
VirtualProtect(ptr($01564A90), $E8, PAGE_READWRITE, OldProtect);

// start of loop creation
ExeCodePointer := Pointer($01564A94);

PWord(ExeCodePointer) ^:= $048B;   // mov     eax, _streamHandles[esi*4]
PByte(Integer(ExeCodePointer)+2) ^:= $B5;
PPointer(Integer(ExeCodePointer)+3)^:= @_streamHandles;

PWord(Integer(ExeCodePointer)+7) ^:= $C085;   // test    eax, eax

PWord(Integer(ExeCodePointer)+9)^:= $840F;     // jz      @@openFile
PInteger(Integer(ExeCodePointer)+11)^:= $01564B31 - (Integer(ExeCodePointer)+15);

PByte(Integer(ExeCodePointer)+15) ^:= $46;     // inc     esi

PWord(Integer(ExeCodePointer)+16) ^:= $FE81;   // cmp    esi, MAX_NUMBER_OF_STREAM_HANDLES

PInteger(Integer(ExeCodePointer)+18)^:= MAX_NUMBER_OF_STREAM_HANDLES;

PByte(Integer(ExeCodePointer)+22) ^:= $7C;          // jl      short findFreeHandle
PByte(Integer(ExeCodePointer)+23)^:= Integer(ExeCodePointer) - (Integer(ExeCodePointer)+24);

PByte(Integer(ExeCodePointer)+24) ^:= $E9;   // jmp     short openFile
PInteger(Integer(ExeCodePointer)+25)^:= $01564B31 - (Integer(ExeCodePointer)+29);
// end of loop creation

PPointer($1564B74)^:= @_streamHandles;    // mov     _streamHandles[esi*4], eax   ; ASM instruction on 0x01564B71

PPointer($01564B8C)^:= @_streamNames;     // lea     edx, _streamNames[edx]

// _sub_4068A0
VirtualProtect(ptr($004068A0), $46, PAGE_READWRITE, OldProtect);

PPointer($4068AB)^:= @_streamHandles;   // mov     eax, _streamHandles[esi*4]

PPointer($4068C2)^:= @_streamHandles;   // mov     eax, _streamHandles[esi*4]

PPointer($4068D0)^:= @_streamHandles;   // mov     _streamHandles[esi*4], 0

PPointer($4068DD)^:= @_streamNames;     // mov     _streamNames[esi], 0

// _readStream
VirtualProtect(ptr($0156C2C0), $14A, PAGE_READWRITE, OldProtect);

PPointer($0156C2E8)^:= @_streamHandles;   // mov     eax, _streamHandles[edx*4]

// _initStreaming
VirtualProtect(ptr($00406B70), $11B, PAGE_READWRITE, OldProtect);

PPointer($00406B7C)^:= @_streamHandles;   // mov     edi, offset _streamHandles

PPointer($00406B81)^:= @_streamNames;   // mov     edx, offset _streamNames

PPointer($00406B98)^:= Pointer(Integer(@_streamNames)+sizeof(_streamNames));

//////////// IMGLIST in default.dat or gta.dat
VirtualProtect(ptr($005B9030), $2FA, PAGE_READWRITE, OldProtect);
CreatedCodePointer := VirtualAlloc(NIL, 50, MEM_COMMIT , PAGE_READWRITE);

ExeCodePointer := Pointer($005B90B3);

PByte(ExeCodePointer) ^:= $E9;  // jmp CreatedCodePointer
PInteger(Integer(ExeCodePointer)+1)^:= Integer(CreatedCodePointer)-Integer(ExeCodePointer)-5;


PWord(CreatedCodePointer) ^:= $468D;              // lea  eax,[esi+0B]
PByte(Integer(CreatedCodePointer)+2) ^:= $0B;

PWord(Integer(CreatedCodePointer)+3) ^:= $840F;   // jz      to: mov     edi, offset aModelsGta_int_ ; "MODELS\\GTA_INT.IMG"
PInteger(Integer(CreatedCodePointer)+5)^:= Integer(ExeCodePointer)+5-Integer(CreatedCodePointer)-9;

PWord(Integer(CreatedCodePointer)+9) ^:= $076A;    // push   7
PByte(Integer(CreatedCodePointer)+11) ^:= $56;      // push   esi
PByte(Integer(CreatedCodePointer)+12) ^:= $68;      // push "IMGLIST
PPointer(Integer(CreatedCodePointer)+13) ^:= PChar('IMGLIST');

PByte(Integer(CreatedCodePointer)+17) ^:= $E8;    // call   _strncmp
PInteger(Integer(CreatedCodePointer)+18)^:= Integer(@_strncmpFunc)-Integer(CreatedCodePointer)-22;

PWord(Integer(CreatedCodePointer)+22) ^:= $C483;  // add    esp, 0Ch
PByte(Integer(CreatedCodePointer)+24) ^:= $0C;

PWord(Integer(CreatedCodePointer)+25) ^:= $C085;  // test   eax, eax

PWord(Integer(CreatedCodePointer)+27) ^:= $850F;  // jnz    @@ testOnImg

PInteger(Integer(CreatedCodePointer)+29)^:= $005B912B - Integer(CreatedCodePointer)-33;

PWord(Integer(CreatedCodePointer)+33) ^:= $468D;    // lea eax,[esi+08]
PByte(Integer(CreatedCodePointer)+35) ^:= $08;

PByte(Integer(CreatedCodePointer)+36) ^:= $50;      // push eax

PByte(Integer(CreatedCodePointer)+37) ^:= $E8;      // call readImgList

PInteger(Integer(CreatedCodePointer)+38)^:= Integer(@readImgList) - Integer(CreatedCodePointer)-42;

PWord(Integer(CreatedCodePointer)+42) ^:= $C483;  // add    esp, 04h
PByte(Integer(CreatedCodePointer)+44) ^:= $04;

PByte(Integer(CreatedCodePointer)+45) ^:= $E9;      // jmp @@continue
PInteger(Integer(CreatedCodePointer)+46)^:= $5B92E1 - Integer(CreatedCodePointer)-50;

// MessageBoxA(0,'test', 'Test before the application started',0);
end.
