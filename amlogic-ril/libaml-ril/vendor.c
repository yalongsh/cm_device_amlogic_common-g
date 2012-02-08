#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "vendor.h"


#define LOG_TAG "AML_RIL"
#include <utils/Log.h>

#ifdef VENDOR_PROWAVE
int setModemSysInterface(const char* sys_path, int value)
{
		int   		sysFd;
		int   		ret;
		unsigned 	data = value;
		if(sys_path == NULL || (value > 1) || (value < 0)){
			LOGD("AML_RIL: wrong parameters\n");
			return -1;
		}
		sysFd = open(sys_path, O_WRONLY);
		if(sysFd == -1){
			LOGD(" failed to open the modem sys file:error-%d", errno);
			return -1;	
		}
		ret = write(sysFd, &data, sizeof(unsigned int));
		if( ret == -1){
			LOGD("failed to set modem sys interface:error- %d", errno);
			close(sysFd);
			return -1;
		}
		close(sysFd);
		return 0;		
}
#endif
