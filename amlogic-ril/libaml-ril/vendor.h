#ifndef _VENDOR_H

#include "aml-ril-config.h"

#ifdef VENDOR_PROWAVE
#define PROWAVE_FILE "/sys/class/modem/i850_modem/enable"
int setModemSysInterface(const char* sys_path, int value);
#endif

#endif
