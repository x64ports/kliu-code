#include "..\version.h"

#define SETUP_VERSION_FULL  CMDOPEN_VERSION_FULL
#define SETUP_VERSION_STR   CMDOPEN_VERSION_STR
#define SETUP_VERSION_LABEL CMDOPEN_VERSION_LABEL
#define SETUP_TITLE_STR     CMDOPEN_SETUP_STR
#define SETUP_AUTHOR_STR    CMDOPEN_AUTHOR_STR
#define SETUP_COPYRIGHT_STR CMDOPEN_COPYRIGHT_STR
#define SETUP_PAYLOAD_STR   CMDOPEN_FILENAME_STR

#ifndef SETUP_LITE
#define REBOOT_MESSAGE TEXT("You may need to log off and log back on in order for these changes to take effect.")
#else
#undef REBOOT_MESSAGE
#endif
