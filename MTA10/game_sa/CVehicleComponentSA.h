/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleComponentSA.h
*  PURPOSE:     Vehicle component part interface
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _VEHICLE_COMPONENT_SA_
#define _VEHICLE_COMPONENT_SA_

typedef std::map <std::string, class CVehicleComponentSA*> vehComponents_t;

class CVehicleComponentSA : public CVehicleComponent
{
public:
                                CVehicleComponentSA( vehComponents_t& container, RpClump *clump, RwFrame *item );
                                ~CVehicleComponentSA();

    const char*                 GetName() const;

    void                        SetMatrix( const RwMatrix& mat );
    const RwMatrix&             GetMatrix() const;
    const RwMatrix&             GetWorldMatrix() const;

    void                        SetPosition( const CVector& pos );
    const CVector&              GetPosition() const;
    const CVector&              GetWorldPosition() const;

    void                        SetActive( bool active );
    bool                        IsActive() const;

    unsigned int                AddAtomic( CRpAtomic *atomic );
    CRpAtomic*                  CloneAtomic( unsigned int idx ) const;
    unsigned int                GetAtomicCount() const;
    bool                        RemoveAtomic( unsigned int idx );

    RpClump*                    m_clump;
    RwFrame*                    m_frame;
    
    vehComponents_t&            m_container;
};

#endif //_VEHICLE_COMPONENT_SA_