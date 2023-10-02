/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/ScriptFile.h
*  PURPOSE:     Script file class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*****************************************************************************/

#ifndef _BASE_SCRIPTFILE_
#define _BASE_SCRIPTFILE_

template < class inherit >
class ScriptFile : public inherit
{
public:
    enum eMode
    {
        MODE_READ,
        MODE_READWRITE,
        MODE_CREATE,
    };

                            ScriptFile              ( CFile *file );
                            ~ScriptFile             ();

    bool                    IsEOF                   ();
    size_t                  GetPointer              ();
    size_t                  GetSize                 ();

    size_t                  SetPointer              ( size_t pos );
    void                    SetSize                 ( size_t size );

    void                    Flush                   ();
    size_t                  Read                    ( size_t size, char* pData );
    size_t                  Write                   ( size_t size, const char* pData );

protected:
    CFile*                  m_file;
};

#endif //_BASE_SCRIPTFILE_