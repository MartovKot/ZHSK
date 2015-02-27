#include "mysplashscreen.h"
#include "updater.h"

MySplashScreen::MySplashScreen()
{
    Updater upd(this);
    upd.RunUpdate();
}
