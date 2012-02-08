/*
 *  Copyright 2010 by AMLOGIC INC.
 *
 *  Ril compatible layer
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 *  Author:  Frank Chen <frank.chen@amlogic.com>
 */

#include <string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in_systm.h>
#include<netinet/ip.h>
#include <cutils/properties.h>

#include "ril-compatible.h"

#define LOG_NDEBUG 0
#define LOG_TAG "AML_RIL"
#include <utils/Log.h>


static MdmInfo g_KnownMdms[] ={
	#include "modem_support_list.h"
};

//this modules don't surport 
static MdmInfo g_SpecialMdms[] ={
    //ruisibo WH700G
    {
            0x05c6, 0x6000,
            "WH700G",
            RIL_MODEM_TYPE_PCIE_MODULE,
            "/dev/ttyUSB0", "/dev/ttyUSB1", NULL,           NULL,           NULL,"0","1",
            RIL_MODEM_NETWORK_TYPE_GSM|RIL_MODEM_NETWORK_TYPE_WCDMA,
            RIL_SERVICE_TYPE_DATA_CALL|RIL_SERVICE_TYPE_SIM|RIL_SERVICE_TYPE_SMS|RIL_SERVICE_TYPE_NETWORK|RIL_SERVICE_TYPE_DEVICE
    },
};


int getDeviceIndexByVidPid(int vid, int pid){
    int index = -1 ;
    int i=0;

    if( vid <= 0 && pid <= 0) 
        return -1;
    
    for(i = 0; i< sizeof(g_KnownMdms)/sizeof(MdmInfo); i++){
    	if((vid == g_KnownMdms[i].vid) && (pid ==g_KnownMdms[i].pid)){
    		index = i;
    		break;
    	}
    }

    return index;
}

int getDeviceVidPid(int *p_vid, int *p_pid)
{
       char value[PROPERTY_VALUE_MAX];
	int vid = 0;
	int pid = 0;
	char *port = NULL;

       if(p_vid==NULL && p_pid==NULL)
            return -1 ;

	property_get( RIL_PROP_USBMODEM_VID, value, "0");
	vid = atoi(value);
	property_get( RIL_PROP_USBMODEM_PID, value, "0");
	pid = atoi(value);

       *p_vid = vid ;
       *p_pid = pid ;

       return 0 ;
}

int getDeviceIndex(){
	int vid = 0;
	int pid = 0;
    
	getDeviceVidPid(&vid, &pid) ;

	return getDeviceIndexByVidPid(vid,pid);
}

const char * getDeviceByIndex(int index, unsigned int type){
	const char *port = NULL;
	if(index < 0 || index > sizeof(g_KnownMdms)/sizeof(MdmInfo) )
		return NULL;

	switch(type){
		case AMLOGIC_USBSERIAL_MODEM:
			port = g_KnownMdms[index].modem;
			break;
		case AMLOGIC_USBSERIAL_SERVICE:
			port = g_KnownMdms[index].service;
			break;
		case AMLOGIC_USBSERIAL_VOICE:
			port = g_KnownMdms[index].voice;
			break;
		case AMLOGIC_USBSERIAL_DIAG:
			port = g_KnownMdms[index].diag;
			break;
		case AMLOGIC_USBSERIAL_GPS:
			port = g_KnownMdms[index].gps;
			break;
             case AMLOGIC_USBSERIAL_MODEM_EX :
                    port = g_KnownMdms[index].modem_ex;
			break;
             case AMLOGIC_USBSERIAL_SERVICE_EX:
                    port = g_KnownMdms[index].service_ex;
			break;
            
		default:
			port = NULL;
	}

	return port;
}

int getArchByIndex(int index){
	unsigned int networktype = 0;
	if(index < 0 || index > sizeof(g_KnownMdms)/sizeof(MdmInfo) )
		return -1;

	networktype = g_KnownMdms[index].networktype;
	if( (networktype & RIL_MODEM_NETWORK_TYPE_GSM) && (networktype & RIL_MODEM_NETWORK_TYPE_CDMA))
		return RIL_MODEM_ARCH_TYPE_DEPENDED;
	if( networktype & RIL_MODEM_NETWORK_TYPE_CDMA )
		return RIL_MODEM_ARCH_TYPE_CDMA;
	
	return RIL_MODEM_ARCH_TYPE_GSM;
}

int getModemTypeByIndex(int index)
{
    if(index < 0 || index > sizeof(g_KnownMdms)/sizeof(MdmInfo) )
        return -1;

    return g_KnownMdms[index].type ;
}

const char * getDevice(unsigned int type)
{	
	int index = -1;

	index = getDeviceIndex();

	return getDeviceByIndex(index, type) ;
}

int isModemSpecial(void)
{
    int i = 0 ;
    int index = -1 ;
    int vid = 0;
    int pid = 0;
    getDeviceVidPid(&vid, &pid) ;

    for(i = 0; i< sizeof(g_SpecialMdms)/sizeof(MdmInfo); i++){
    	if((vid == g_SpecialMdms[i].vid) && (pid ==g_SpecialMdms[i].pid)){
    		index = i;
    		break;
    	}
    }

    return index ;
}

int getModemNetworkType()
{
	int index = -1;

	index = getDeviceIndex();

	if(index == -1) 
            return -2;

	return g_KnownMdms[index].networktype;
}

int getSupportServices()
{	
	int index = -1;

	index = getDeviceIndex();

	if(index == -1) return -2;

	return g_KnownMdms[index].services;
}

void dumpModemSupportList(){
	LOGE("\n\nDumping modem support list\n");

	int i = 0;
	for(i = 0; i< (int)(sizeof(g_KnownMdms)/sizeof(MdmInfo)); i++){
		LOGE("%d)    %s, %s",i, g_KnownMdms[i].name,(g_KnownMdms[i].type == RIL_MODEM_TYPE_USB_DONGLE)?"USB dongle":"PCIE module" );
		LOGE("       vid(%d) pid(%d)",g_KnownMdms[i].vid, g_KnownMdms[i].vid);
		LOGE("       modem(%s)",g_KnownMdms[i].modem);
		LOGE("       service(%s)",g_KnownMdms[i].service);
		LOGE("       diag(%s)",g_KnownMdms[i].diag);
		LOGE("       voice(%s)",g_KnownMdms[i].voice);
		LOGE("       gps(%s)",g_KnownMdms[i].gps);
		LOGE("       network type(0x%x)",g_KnownMdms[i].networktype);
		LOGE("       services(0x%x)",g_KnownMdms[i].services);
	}
}

unsigned int getModemSupportList( MdmInfo *mdmInfo ){
	mdmInfo = &g_KnownMdms; 
	return sizeof(g_KnownMdms)/sizeof(MdmInfo);
}

void setPPPScript(int arch, const char * operater){
	if(RIL_MODEM_ARCH_TYPE_GSM == arch){
		property_set(RIL_PROP_PPP_CHAT_SCRIPT,"mcli-gsm");
	}
	else{
		property_set(RIL_PROP_PPP_CHAT_SCRIPT,"mcli-cdma");
	}
}