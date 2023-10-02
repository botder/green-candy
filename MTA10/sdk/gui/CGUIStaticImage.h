/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIStaticImage.h
*  PURPOSE:     Static image widget interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUISTATICIMAGE_H
#define __CGUISTATICIMAGE_H

#include "CGUIElement.h"
#include "CGUITexture.h"

class CGUIStaticImage : public CGUIElement
{
public:
    virtual                             ~CGUIStaticImage        () {};

    virtual bool                        LoadFromFile            ( const char* szFilename ) = 0;
    virtual bool                        LoadFromTexture         ( CGUITexture* pTexture ) = 0;
    virtual void                        Clear                   () = 0;

    virtual void                        SetFrameEnabled         ( bool bFrameEnabled ) = 0;
    virtual bool                        IsFrameEnabled          () = 0;

    virtual void                        Render                  () = 0;
};

#endif
