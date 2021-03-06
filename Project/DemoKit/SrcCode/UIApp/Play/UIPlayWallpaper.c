#include "Type.h"
#include "PlaybackTsk.h"
#include "UIAppPlay.h"
#include "UIPlayWallpaper.h"


///////////////////////////////////////////////////////////////////////

void UIWallpaper_OpenView(BOOL bOpenFromPB)
{
    if(bOpenFromPB)
    {
        Ux_SendEvent(&UIPlayObjCtrl,NVTEVT_PLAYSINGLE, 1, PB_SINGLE_CURR);
    }
}

void UIWallpaper_CloseView(BOOL bReturnToPB)
{
    if(bReturnToPB)
    {
        Ux_SendEvent(&UIPlayObjCtrl,NVTEVT_PLAYSINGLE, 2, PB_SINGLE_CURR, 1);
    }
}

UINT32 UIWallpaper_PrepareUserWP(UINT32 addr, UINT32 size)
{
//#NT#2010/09/20#Scottie -begin
//#NT#Modify PB_CaptureScreen() to support capturing image in thumbnail mode & save to User spec size
#if 0
    UINT32 uiFileAddr, uiFileSize = size;

    PB_CaptureScreen(&uiFileAddr, &uiFileSize);
#endif
    PB_LOGO_INFO LogoInfo = {0};

    //#NT#2010/11/2#Jeah Yen -begin
    LogoInfo.uiDstWidth = 640;
    LogoInfo.uiDstHeight = 240;
    //#NT#2010/11/2#Jeah Yen -end
    
    //#NT#2011/01/17#Steven feng -begin
    //Avoid JPG size is larger than PStore section size
    LogoInfo.uiFileSize = size;
    //#NT#2011/01/17#Steven feng -end
        
    PB_CaptureScreen(&LogoInfo);
    if(PB_WaitCommandFinish(PB_WAIT_INFINITE) == PB_STA_DONE)
    {// OK, copy wallpaper
        memcpy((char*)addr, (char*)LogoInfo.uiFileAddr, LogoInfo.uiFileSize);

        return LogoInfo.uiFileSize;
    }
//#NT#2010/09/20#Scottie -end
    else
    {
        debug_err(("^RCapture screen image NG !\r\n"));

        return 0;
    }
}
