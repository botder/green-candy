/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.XML.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define MAX_STRING_LENGTH 2048

#ifdef MTA_CLIENT

namespace LuaFunctionDefs
{
    LUA_DECLARE( xmlFindChild )
    {
        // xmlNode*, node name, index
        if ( lua_type ( L, 1 ) != LUA_TLIGHTUSERDATA ||
            lua_type ( L, 2 ) != LUA_TSTRING || 
            lua_type ( L, 3 ) != LUA_TNUMBER )
        {
            debug->LogBadType( "xmlNodeFindChild" );

            lua_pushboolean ( L, false );
            return 1;
        }

        // Grab the XML node
        CXMLNode* pNode = lua_toxmlnode ( L, 1 );
        if ( pNode )
        {
            const char * szTagName = lua_tostring ( L, 2 );
            unsigned int iIndex = static_cast < unsigned int > ( lua_tonumber ( L, 3 ) );
            CXMLNode * pFoundNode = pNode->FindSubNode ( szTagName, iIndex ); 
            if ( pFoundNode )
            {
                lua_pushxmlnode ( L, pFoundNode );
                return 1;
            }
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeGetChildren )
    {
        // xmlNode*, [index]
        if ( lua_type ( L, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CXMLNode* pNode = lua_toxmlnode ( L, 1 );
            if ( pNode )
            {
                unsigned int uiIndex = 0;
                bool bGetAllChildren = true;
                int iArgument2 = lua_type ( L, 2 );
                if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
                {
                    uiIndex = static_cast < unsigned int > ( lua_tonumber ( L, 2 ) );
                    bGetAllChildren = false;
                }
                CXMLNode * pFoundNode = NULL;
                if ( !bGetAllChildren )
                {
                    pFoundNode = pNode->GetSubNode ( uiIndex );
                    if ( pFoundNode )
                    {
                        lua_pushxmlnode ( L, pFoundNode );
                        return 1;
                    }
                }
                else
                {
                    lua_newtable ( L );
                    uiIndex = 0;
                    std::list < CXMLNode * > ::iterator iter = pNode->ChildrenBegin ();
                    for ( ; iter != pNode->ChildrenEnd () ; ++iter )
                    {                
                        lua_pushnumber ( L, ++uiIndex );
                        lua_pushxmlnode ( L, ( CXMLNode * ) ( *iter ) );
                        lua_settable ( L, -3 );
                    }
                    return 1;
                }
            }
        }
        else
            debug->LogBadType( "xmlNodeGetChildren" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeGetValue )
    {
        // pNode, [Buffer Size]
        int iSecondVariableType = lua_type ( L, 2 );
        if ( lua_type ( L, 1 ) != LUA_TLIGHTUSERDATA || 
            !( iSecondVariableType == LUA_TNONE || iSecondVariableType == LUA_TNUMBER ) )
        {
            debug->LogBadType( "xmlNodeGetValue" );

            lua_pushboolean ( L, false );
            return 1;
        }

        CXMLNode* pNode = lua_toxmlnode ( L, 1 );
        if ( pNode )
        {  
            unsigned int iBufferSize = 1024;
            if ( iSecondVariableType == LUA_TNUMBER )
            {
                iBufferSize = static_cast < unsigned int > ( lua_tonumber ( L, 2 ) );
                if ( iBufferSize > 1024 )
                    iBufferSize = 255;
            }

            lua_pushstring ( L, pNode->GetTagContent () );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeSetValue )
    {
        // pNode, Value
        if ( lua_type ( L, 1 ) != LUA_TLIGHTUSERDATA || 
            lua_type ( L, 2 ) != LUA_TSTRING )
        {
            debug->LogBadType( "xmlNodeSetValue" );

            lua_pushboolean ( L, false );
            return 1;
        }

        CXMLNode* pNode = lua_toxmlnode ( L, 1 );
        if ( pNode )
        {  
            const char * szTagContents = lua_tostring ( L, 2 );
            pNode->SetTagContent ( szTagContents );
            lua_pushboolean ( L, true );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeGetName )
    {
        if ( lua_type( L, 1 ) != LUA_TLIGHTUSERDATA )
        {
            debug->LogBadType( "xmlNodeGetName" );

            lua_pushboolean( L, false );
            return 1;
        }

        CXMLNode *pNode = lua_toxmlnode( L, 1 );
        if ( pNode )
        {
            lua_pushstring( L, pNode->GetTagName().c_str() );
            return 1;
        }

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeSetName )
    {
        int iType1 = lua_type ( L, 1 );
        int iType2 = lua_type ( L, 2 );
        if ( ( iType1 == LUA_TLIGHTUSERDATA ) && 
            ( iType2 == LUA_TSTRING ) )
        {
            CXMLNode* pNode = lua_toxmlnode ( L, 1 );
            if ( pNode )
            {
                std::string strTagName ( lua_tostring ( L, 2 ) );
                pNode->SetTagName ( strTagName );
                lua_pushboolean ( L, true );
                return 1;
            }
            else
                debug->LogBadPointer( "xmlNodeSetName", "xml-node", 1 );
        }
        else
            debug->LogBadType( "xmlNodeSetName" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeGetAttributes )
    {
        // pNode, Attribute Name, [Buffer Size]
        int iThirdVariableType = lua_type ( L, 3 );
        if ( lua_type ( L, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CXMLNode* pNode = lua_toxmlnode ( L, 1 );
            if ( pNode )
            {
                lua_newtable ( L );
                unsigned int uiIndex = 0;
                std::list < CXMLAttribute * > ::iterator iter = pNode->GetAttributes ().ListBegin ();
                for ( ; iter != pNode->GetAttributes ().ListEnd () ; ++iter )
                {
                    lua_pushstring ( L, ( *iter )->GetName ().c_str () );
                    lua_pushstring ( L, ( *iter )->GetValue ().c_str () );
                    lua_settable ( L, -3 );
                }
                return 1;
            }
            else
                debug->LogBadType( "xmlNodeGetAttributes" );
        }
        else
            debug->LogBadType( "xmlNodeGetAttributes" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeGetAttribute )
    {
        // pNode, Attribute Name, [Buffer Size]
        int iThirdVariableType = lua_type ( L, 3 );
        if ( lua_type ( L, 1 ) != LUA_TLIGHTUSERDATA || 
            !( iThirdVariableType == LUA_TNONE || iThirdVariableType == LUA_TNUMBER ) ||
            lua_type ( L, 2 ) != LUA_TSTRING )
        {
            debug->LogBadType( "xmlNodeGetAttribute" );

            lua_pushboolean ( L, false );
            return 1;
        }

        // Grab the XML node
        CXMLNode* pNode = lua_toxmlnode ( L, 1 );
        if ( pNode )
        {  
            // Find the attribute with that name
            const char * szAttributeName = lua_tostring ( L, 2 );
            CXMLAttribute * pAttribute = pNode->GetAttributes ().Find ( szAttributeName ); 
            if ( pAttribute )
            {
                // Limit reading to that many characters
                unsigned int iBufferSize = 255;
                if ( iThirdVariableType == LUA_TNUMBER )
                {
                    iBufferSize = static_cast < unsigned int > ( lua_tonumber ( L, 3 ) );
                    if ( iBufferSize > 1024 )
                        iBufferSize = 255;
                }

                // Read the attribute and return the string
                lua_pushstring ( L, pAttribute->GetValue ().c_str () );
                return 1;
            }
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeSetAttribute )
    {
        // pNode, Attribute Name, Value
        int iType1 = lua_type ( L, 1 );
        int iType2 = lua_type ( L, 2 );
        int iType3 = lua_type ( L, 3 );
        if ( ( iType1 == LUA_TLIGHTUSERDATA ) && 
            ( iType2 == LUA_TSTRING ) &&
            ( iType3 == LUA_TSTRING || iType3 == LUA_TNUMBER || iType3 == LUA_TNIL ) )
        {
            // Grab the xml node
            CXMLNode* pNode = lua_toxmlnode ( L, 1 );
            if ( pNode )
            {
                // Grab the attribute name and value
                const char * szAttributeName = lua_tostring ( L, 2 );

                // Are we going to set it to a value?
                if ( iType3 == LUA_TSTRING || iType3 == LUA_TNUMBER )
                {
                    const char * szAttributeValue = lua_tostring ( L, 3 );

                    // Write the node
                    CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( szAttributeName );
                    if ( pAttribute )
                    {
                        pAttribute->SetValue ( szAttributeValue );

                        lua_pushboolean ( L, true );
                        return 1;
                    }
                }
                else
                {
                    // Delete the attribute if it exists
                    CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( szAttributeName );
                    if ( pAttribute )
                    {
                        delete pAttribute;

                        lua_pushboolean ( L, true );
                        return 1;
                    }
                }
            }
        }
        else
            debug->LogBadType( "xmlNodeSetAttribute" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlNodeGetParent )
    {
        // xmlNode*, node name, index
        if ( lua_type ( L, 1 ) == LUA_TLIGHTUSERDATA  )
        {
            // Grab the XML node
            CXMLNode* pNode = lua_toxmlnode ( L, 1 );
            if ( pNode )
            {            
                CXMLNode * pParent = pNode->GetParent ();
                if ( pParent )
                {
                    lua_pushxmlnode ( L, pParent );
                    return 1;
                }
            }
        }
        else
            debug->LogBadType( "xmlNodeGetParent" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlLoadFile )
    {
    //  xmlnode xmlLoadFile ( string filePath )
        SString path;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( path );

        if ( !argStream.HasErrors () )
        {
            LuaMain *luaMain = lua_readcontext( L );
            Resource* pResource = luaMain->GetResource();
            filePath strFilename;
            const char *meta;
            if ( resManager->ParseResourceFullPath( pResource, path, meta, strFilename ) )
            {
                // Create the XML
                std::string ansiFilename = strFilename.convert_ansi();

                CXMLFile * xmlFile = luaMain->CreateXML ( ansiFilename.c_str() );
                if ( xmlFile )
                {
                    // Parse it
                    if ( xmlFile->Parse() )
                    {
                        // Create the root node if it doesn't exist
                        CXMLNode* pRootNode = xmlFile->GetRootNode ();
                        if ( !pRootNode )
                            pRootNode = xmlFile->CreateRootNode ( "root" );

                        // Got a root node?
                        if ( pRootNode )
                        {
                            lua_pushxmlnode ( L, pRootNode );
                            return 1;
                        }
                    }

                    // Destroy the XML
                    luaMain->DestroyXML ( xmlFile );
                }   
            }
        }
        else
            debug->LogCustom( SString ( "Bad argument @ '%s' [%s]", "xmlLoadFile", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlCreateFile )
    {
    //  xmlnode xmlCreateFile ( string filePath, string rootNodeName )
        SString path; SString rootNodeName;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( path );
        argStream.ReadString ( rootNodeName );

        if ( argStream.HasErrors() )
        {
            debug->LogCustom( SString( "Bad argument @ '%s' [%s]", "xmlCreateFile", *argStream.GetErrorMessage() ) );
            lua_pushboolean( L, false );
            return 1;
        }

        LuaMain* pLuaMain = lua_readcontext( L );
        Resource* pResource = pLuaMain->GetResource();
        filePath strFile;
        const char *meta;

        if ( resManager->ParseResourceFullPath( pResource, path, meta, strFile ) )
        {
            // Create the XML
            CXMLFile *xmlFile = pLuaMain->CreateXML( strFile.c_str () );
            if ( xmlFile )
            {
                // Create its root node
                CXMLNode* pRootNode = xmlFile->CreateRootNode ( rootNodeName );
                if ( pRootNode )
                {
                    lua_pushxmlnode ( L, pRootNode );
                    return 1;
                }

                // Delete the XML again
                pLuaMain->DestroyXML ( xmlFile );
            }
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlSaveFile )
    {
        // CXMLNode*
        if ( lua_type ( L, 1 ) != LUA_TLIGHTUSERDATA )
        {
            debug->LogBadType( "xmlSaveFile" );

            lua_pushboolean ( L, false );
            return 1;
        }

        CXMLNode *pNode = lua_toxmlnode( L, 1 );
        if ( pNode )
        {
            LuaMain *luaMain = lua_readcontext( L );
            luaMain->SaveXML( pNode );
            lua_pushboolean( L, true );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlUnloadFile )
    {
        // CXMLNode*
        if ( lua_type ( L, 1 ) != LUA_TLIGHTUSERDATA )
        {
            debug->LogBadType( "xmlUnloadFile" );

            lua_pushboolean ( L, false );
            return 1;
        }

        CXMLNode* pNode = lua_toxmlnode ( L, 1 );
        if ( pNode )
        {
            LuaMain * luaMain = lua_readcontext( L );
            luaMain->DestroyXML ( pNode );
            lua_pushboolean ( L, true );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlCreateChild )
    {
        // Node name
        if ( lua_type ( L, 1 ) != LUA_TLIGHTUSERDATA ||
            lua_type ( L, 2 ) != LUA_TSTRING )
        {
            debug->LogBadType( "xmlCreateChild" );

            lua_pushboolean ( L, false );
            return 1;
        }

        // Get the Node
        CXMLNode* pXMLNode = lua_toxmlnode ( L, 1 );
        if ( pXMLNode )
        {
            SString strSubNodeName ( "%s", lua_tostring ( L, 2 ) );
            CXMLNode* pXMLSubNode = pXMLNode->CreateSubNode ( strSubNodeName );
            if ( pXMLSubNode )
            {
                lua_pushxmlnode ( L, pXMLSubNode );
                return 1;
            }
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlDestroyNode )
    {
        // Verify the argument type
        if ( lua_type ( L, 1 ) == LUA_TLIGHTUSERDATA )
        {
            // Get the Node
            CXMLNode* pXMLNode = lua_toxmlnode ( L, 1 );
            if ( pXMLNode )
            {
                // Grab the parent so that we can delete this node from it
                CXMLNode* pParent = pXMLNode->GetParent ();
                if ( pParent )
                {
                    // Delete it
                    pParent->DeleteSubNode ( pXMLNode );

                    // Return success
                    lua_pushboolean ( L, true );
                    return 1;
                }
            }
        }
        else
            debug->LogBadType( "xmlDestroyNode" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( xmlCopyFile )
    {
    //  xmlnode xmlCopyFile ( xmlnode nodeToCopy, string newFilePath )
        CXMLNode* pSourceNode; SString newFilePath;

        CScriptArgReader argStream ( L );
        argStream.ReadUserData ( pSourceNode );
        argStream.ReadString ( newFilePath );

        if ( !argStream.HasErrors () )
        {
            // Grab the virtual machine for this luastate
            LuaMain* pLUA = lua_readcontext( L );
            Resource* pResource = pLUA->GetResource();
            filePath strFilename;
            const char *meta;
            if ( resManager->ParseResourceFullPath( (Resource*&)pResource, newFilePath, meta, strFilename ) )
            {
                // Grab the source node
                CXMLNode* pSourceNode = lua_toxmlnode ( L, 1 );
                if ( pSourceNode )
                {
                    // Grab the roots tag name
                    std::string strRootTagName;
                    strRootTagName = pSourceNode->GetTagName ();

                    // Grab our lua VM
                    LuaMain* pLUA = lua_readcontext( L );

                    // Create the new XML file and its root node
                    std::string ansiFilename = strFilename.c_str();

                    CXMLFile* pNewXML = pLUA->CreateXML ( ansiFilename.c_str() );
                    if ( pNewXML )
                    {
                        // Create root for new XML
                        CXMLNode* pNewRoot = pNewXML->CreateRootNode ( strRootTagName );
                        if ( pNewRoot )
                        {
                            // Copy over the attributes from the root
                            int iAttributeCount = pSourceNode->GetAttributes ().Count ();
                            int i = 0;
                            CXMLAttribute* pAttribute;
                            for ( ; i < iAttributeCount; i++ )
                            {
                                pAttribute = pSourceNode->GetAttributes ().Get ( i );
                                if ( pAttribute )
                                    pNewRoot->GetAttributes ().Create ( *pAttribute );
                            }

                            // Copy the stuff from the given source node to the destination root
                            if ( pSourceNode->CopyChildrenInto ( pNewRoot, true ) )
                            {
                                lua_pushxmlnode ( L, pNewRoot );
                                return 1;
                            }
                        }

                        // Delete the XML again
                        pLUA->DestroyXML ( pNewXML );
                    }
                }
            }
            else
                Logger::ErrorPrintf ( "Unable to copy xml file; bad filepath" );
        }
        else
            debug->LogCustom( SString ( "Bad argument @ '%s' [%s]", "xmlCopyFile", *argStream.GetErrorMessage () ) );

        // Error
        lua_pushboolean ( L, false );
        return 1;
    }
}

#endif //MTA_CLIENT