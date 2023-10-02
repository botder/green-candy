/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEventSA.h
*  PURPOSE:     Header file for base event class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_EVENT
#define __CGAMESA_EVENT

#include <game/CEvent.h>

class CEventSAInterface
{
public:
    void*   operator new( size_t );
    void    operator delete( void *ptr );

    eEventEntityType    m_eEventType;           // 0
    eEventEntity        m_eEventEntity;         // 4
    DWORD               m_nEventEntityRef;      // 8
    CEntity*            m_pCommitedBy;          // 12
    CVector             m_vecEventPosition;     // 16
    DWORD               m_nEventExpiryTime;     // 28
    DWORD               m_nEventRemoveFrame;    // 32

    BYTE                m_pad[32];              // 36
};

class CEventSA : public CEvent
{
public:
                                CEventSA            ( CEventSAInterface * pInterface )  { m_pInterface = pInterface; }

    CEventSAInterface *         GetInterface        ( void )    { return m_pInterface; }

    eEventEntityType            GetType             ( void );
    CEntity *                   GetEntity           ( void );
    eEventEntity                GetEntityType       ( void );
    CEntity *                   GetCommittedBy      ( void );
    CVector *                   GetPosition         ( void );

protected:
    CEventSAInterface *         m_pInterface;
};

#endif
