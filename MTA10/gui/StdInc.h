#pragma message("Compiling precompiled header.\n")

#include <CEGUI.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include <dbgheap.h>

#include <list>
#include <map>
#include <string>
#include <vector>

#include <d3dx9.h>
#include <renderers/directx9GUIRenderer/d3d9renderer.h>
#include <renderers/directx9GUIRenderer/d3d9texture.h>

#define MTA_CLIENT
#include "SharedUtil.h"
#include <utils/CBuffer.h>
#include <core/CCoreInterface.h>
#include "CGUITabListItem.h"
#include "CGUITabList.h"
#include "CGUI_Impl.h"
#include "CGUIButton_Impl.h"
#include "CGUICheckBox_Impl.h"
#include "CGUIEdit_Impl.h"
#include "CGUIFont_Impl.h"
#include "CGUIGridList_Impl.h"
#include "CGUILabel_Impl.h"
#include "CGUIListItem_Impl.h"
#include "CGUIMemo_Impl.h"
#include "CGUIProgressBar_Impl.h"
#include "CGUIRadioButton_Impl.h"
#include "CGUIMessageBox_Impl.h"
#include "CGUIStaticImage_Impl.h"
#include "CGUIScrollBar_Impl.h"
#include "CGUIScrollPane_Impl.h"
#include "CGUITabPanel_Impl.h"
#include "CGUITexture_Impl.h"
#include "CGUIWindow_Impl.h"
#include "CGUIComboBox_Impl.h"

extern CFileTranslator *guiRoot;