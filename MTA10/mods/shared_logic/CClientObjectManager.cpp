/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientObjectManager.cpp
*  PURPOSE:     Physical object entity manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Kevin Whiteside <kevuwk@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::vector;

// Breakable model list
//  Generated by: 
//      1. Extracting model names from object.dat which have a 'Collision Damage Effect' greater than 0
//      2. Converting model names to IDs using objects.xml
static unsigned short g_usBreakableModelList[] = {
           625,    626,    627,    628,    629,    630,    631,    632,    633,    642, 
           643,    644,    646,    650,    716,    717,    737,    738,    792,    858, 
           881,    882,    883,    884,    885,    886,    887,    888,    889,    890, 
           891,    892,    893,    894,    895,    904,    905,    941,    955,    956, 
           959,    961,    990,    993,    996,   1209,   1211,   1213,   1219,   1220, 
          1221,   1223,   1224,   1225,   1226,   1227,   1228,   1229,   1230,   1231, 
          1232,   1235,   1238,   1244,   1251,   1255,   1257,   1262,   1264,   1265, 
          1270,   1280,   1281,   1282,   1283,   1284,   1285,   1286,   1287,   1288, 
          1289,   1290,   1291,   1293,   1294,   1297,   1300,   1302,   1315,   1328, 
          1329,   1330,   1338,   1350,   1351,   1352,   1370,   1373,   1374,   1375, 
          1407,   1408,   1409,   1410,   1411,   1412,   1413,   1414,   1415,   1417, 
          1418,   1419,   1420,   1421,   1422,   1423,   1424,   1425,   1426,   1428, 
          1429,   1431,   1432,   1433,   1436,   1437,   1438,   1440,   1441,   1443, 
          1444,   1445,   1446,   1447,   1448,   1449,   1450,   1451,   1452,   1456, 
          1457,   1458,   1459,   1460,   1461,   1462,   1463,   1464,   1465,   1466, 
          1467,   1468,   1469,   1470,   1471,   1472,   1473,   1474,   1475,   1476, 
          1477,   1478,   1479,   1480,   1481,   1482,   1483,   1514,   1517,   1520, 
          1534,   1543,   1544,   1545,   1551,   1553,   1554,   1558,   1564,   1568, 
          1582,   1583,   1584,   1588,   1589,   1590,   1591,   1592,   1645,   1646, 
          1647,   1654,   1664,   1666,   1667,   1668,   1669,   1670,   1672,   1676, 
          1684,   1686,   1775,   1776,   1949,   1950,   1951,   1960,   1961,   1962, 
          1975,   1976,   1977,   2647,   2663,   2682,   2683,   2885,   2886,   2887, 
          2900,   2918,   2920,   2925,   2932,   2933,   2942,   2943,   2945,   2947, 
          2958,   2959,   2966,   2968,   2971,   2977,   2987,   2988,   2989,   2991, 
          2994,   3006,   3018,   3019,   3020,   3021,   3022,   3023,   3024,   3029, 
          3032,   3036,   3058,   3059,   3067,   3083,   3091,   3221,   3260,   3261, 
          3262,   3263,   3264,   3265,   3267,   3275,   3276,   3278,   3280,   3281, 
          3282,   3302,   3374,   3409,   3460,   3516,   3794,   3795,   3797,   3853, 
          3855,   3864,   3872,   3884,  11103,  12840,  16627,  16628,  16629,  16630, 
         16631,  16632,  16633,  16634,  16635,  16636,  16732,  17968, 
};

unsigned int CClientObjectManager::m_iEntryInfoNodeEntries = 0;
unsigned int CClientObjectManager::m_iPointerNodeSingleLinkEntries = 0;
unsigned int CClientObjectManager::m_iPointerNodeDoubleLinkEntries = 0;

CClientObjectManager::CClientObjectManager( CClientManager* pManager )
{
    // Initialize members
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
}

CClientObjectManager::~CClientObjectManager()
{
    // Destroy all objects
    DeleteAll();
}

void CClientObjectManager::DoPulse()
{
    UpdateLimitInfo();

    CClientObject *pObject;
    // Loop through all our streamed-in objects
    vector <CClientObject*>::iterator iter = m_StreamedIn.begin();

    for ( ; iter != m_StreamedIn.end(); ++iter )
    {
        pObject = *iter;

        // We should have a game-object here
        assert( pObject->GetGameObject() );

        pObject->StreamedInPulse();
    }
}

void CClientObjectManager::DeleteAll()
{
    // Delete all the objects
    m_bCanRemoveFromList = false;
    objects_t::const_iterator iter = m_Objects.begin();

    for ( ; iter != m_Objects.end(); iter++ )
        (*iter)->Delete();

    // Clear the list
    m_Objects.clear();
    m_bCanRemoveFromList = true;
}

CClientObject* CClientObjectManager::Get( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity *pEntity = CElementIDs::GetElement( ID );

    if ( pEntity && pEntity->GetType() == CCLIENTOBJECT )
    {
        return (CClientObject*)pEntity;
    }

    return NULL;
}

CClientObject* CClientObjectManager::Get( CObject* pObject, bool bValidatePointer )
{
    if ( !pObject )
        return NULL;

    if ( bValidatePointer )
    {
        vector <CClientObject*>::const_iterator iter = m_StreamedIn.begin();

        for ( ; iter != m_StreamedIn.end(); iter++ )
        {
            if ( (*iter)->GetGameObject() == pObject )
                return *iter;
        }
        return NULL;
    }

    return (CClientObject*)pObject->GetStoredPointer();
}

CClientObject* CClientObjectManager::GetSafe( CEntity *pEntity )
{
    if ( !pEntity )
        return NULL;

    vector <CClientObject*> ::const_iterator iter = m_StreamedIn.begin();

    for ( ; iter != m_StreamedIn.end(); iter++ )
    {
        if ( dynamic_cast <CEntity*> ( (*iter)->GetGameObject() ) == pEntity )
        {
            return *iter;
        }
    }
    return NULL;
}

bool CClientObjectManager::IsValidModel( unsigned short id )
{
    CModelInfo *info = g_pGame->GetModelInfo( id );

    return info != NULL && info->IsObject();
}

bool CClientObjectManager::IsBreakableModel( unsigned short id )
{
    static std::map <unsigned short, short> breakableModelMap;

    // Initialize map if required
    if ( breakableModelMap.size() == 0 )
    {
        for ( uint i = 0; i < sizeof( g_usBreakableModelList ) / sizeof( g_usBreakableModelList[0] ) ; i++ )
            breakableModelMap[ g_usBreakableModelList[i] ] = 0;
    }

    return breakableModelMap.find( id ) != breakableModelMap.end();
}

bool CClientObjectManager::ObjectsAroundPointLoaded( const CVector& vecPosition, float fRadius, unsigned short usDimension, SString* pstrStatus )
{
    // Get list of objects that may be intersecting the sphere
    CClientEntityResult result;
    GetClientSpatialDatabase()->SphereQuery( result, CSphere( vecPosition, fRadius ) );

    bool bResult = true;

    // Extract relevant types
    for ( CClientEntityResult::const_iterator it = result.begin() ; it != result.end(); ++it )
    {
        CClientEntity *pEntity = *it;

        if ( pEntity->GetType() == CCLIENTOBJECT )
        {
            CClientObject *pObject = (CClientObject*)pEntity;

            if ( !pObject->GetGameObject() || !pObject->GetModelInfo()->IsLoaded() || !pObject->IsStreamedIn() )
            {
                if ( pObject->GetDimension() == usDimension )
                {
                    // Final distance check
                    float fDistSquared = pObject->GetDistanceToBoundingBoxSquared( vecPosition );

                    if ( fDistSquared < fRadius * fRadius )
                        bResult = false;

                    if ( pstrStatus )
                    {
                        // Debugging information
                        *pstrStatus += SString( "ID:%05d  Dist:%4.1f  GetGameObject:%d  IsLoaded:%d  IsStreamedIn:%d\n"
                                                ,pObject->GetModel()
                                                ,sqrtf( fDistSquared )
                                                ,pObject->GetGameObject() ? 1 : 0
                                                ,pObject->GetModelInfo()->IsLoaded() ? 1 : 0
                                                ,pObject->IsStreamedIn() ? 1 : 0
                                              );
                    }
                    else if ( !bResult )
                        break;
                }
            }
        }
    }

    return bResult;
}

void CClientObjectManager::OnCreation( CClientObject * pObject )
{
    m_StreamedIn.push_back( pObject );
    UpdateLimitInfo();
}

void CClientObjectManager::OnDestruction( CClientObject * pObject )
{
    ListRemove( m_StreamedIn, pObject );
    UpdateLimitInfo();
}

void CClientObjectManager::UpdateLimitInfo()
{
    CPools *pPools = g_pGame->GetPools();
    m_iEntryInfoNodeEntries = pPools->GetNumberOfUsedSpaces( ENTRY_INFO_NODE_POOL );
    m_iPointerNodeSingleLinkEntries = pPools->GetNumberOfUsedSpaces( POINTER_SINGLE_LINK_POOL );
    m_iPointerNodeDoubleLinkEntries = pPools->GetNumberOfUsedSpaces( POINTER_DOUBLE_LINK_POOL );
}

bool CClientObjectManager::IsObjectLimitReached()
{
    // If we've run out of either of these limit, don't allow more objects
    if ( m_iEntryInfoNodeEntries >= g_pGame->GetPools()->GetPoolCapacity( ENTRY_INFO_NODE_POOL ) ||
         m_iPointerNodeSingleLinkEntries >= g_pGame->GetPools()->GetPoolCapacity( POINTER_SINGLE_LINK_POOL ) ||
         m_iPointerNodeDoubleLinkEntries >= g_pGame->GetPools()->GetPoolCapacity( POINTER_DOUBLE_LINK_POOL ) )
    {
        // Limit reached
        return true;
    }

    // Allow max 250 objects at once for now.
    // TODO: The real limit is up to 350 but we're limited by other limits.
    return g_pGame->GetPools()->GetNumberOfUsedSpaces( OBJECT_POOL ) == g_pGame->GetPools()->GetPoolCapacity( OBJECT_POOL );
}

void CClientObjectManager::RestreamObjects( unsigned short usModel )
{
    // Store the affected vehicles
    CClientObject *pObject;
    objects_t::const_iterator iter = IterBegin();

    for ( ; iter != IterEnd(); iter++ )
    {
        pObject = *iter;

        // Streamed in and same vehicle ID?
        if ( pObject->IsStreamedIn() && pObject->GetModel() == usModel )
        {
            // Stream it out for a while until streamed decides to stream it
            // back in eventually
            pObject->StreamOutForABit();
        }
    }
}

CClientObjectManager::objects_t::const_iterator CClientObjectManager::IterGet( CClientObject* pObject ) const
{
    // Find it in our list
    objects_t::const_iterator iter = m_Objects.begin();

    for ( ; iter != m_Objects.end(); iter++ )
    {
        if ( *iter == pObject )
            return iter;
    }

    // We couldn't find it
    return m_Objects.begin();
}

CClientObjectManager::objects_t::const_reverse_iterator CClientObjectManager::IterGetReverse( CClientObject* pObject ) const
{
    // Find it in our list
    objects_t::const_reverse_iterator iter = m_Objects.rbegin();

    for ( ; iter != m_Objects.rend(); iter++ )
    {
        if ( *iter == pObject )
            return iter;
    }

    // We couldn't find it
    return m_Objects.rbegin();
}

void CClientObjectManager::RemoveFromList( CClientObject* pObject )
{
    if ( !m_bCanRemoveFromList )
        return;

    if ( !m_Objects.empty() )
        m_Objects.remove( pObject );
}

bool CClientObjectManager::Exists( CClientObject *object ) const
{
    objects_t::const_iterator iter = m_Objects.begin();

    for ( ; iter != m_Objects.end(); iter++ )
    {
        if ( *iter == object )
            return true;
    }

    return false;
}
