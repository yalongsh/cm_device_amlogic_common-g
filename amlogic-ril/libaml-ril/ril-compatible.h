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
 
#ifndef _AML_RIL_COMPATIBLE_H
#define _AML_RIL_COMPATIBLE_H

//prop defines
#define RIL_PROP_USBSERIAL_MODEM              "ril.usbmodem.modem"
#define RIL_PROP_USBSERIAL_SERVICE            "ril.usbmodem.service"
#define RIL_PROP_USBSERIAL_VOICE              "ril.usbmodem.voice"
#define RIL_PROP_USBSERIAL_DIAG               "ril.usbmodem.diag"
#define RIL_PROP_USBSERIAL_GPS                "ril.usbmodem.gps"

#define RIL_PROP_USBMODEM_TYPE                "ril.usbmodem.type"

#define RIL_PROP_USBMODEM_VID                 "ril.usbmodem.vid"
#define RIL_PROP_USBMODEM_PID                 "ril.usbmodem.pid"

#define RIL_PROP_EMBEDDED_MODEM               "ril.hw.embedded.modem"

#define RIL_PROP_PPP_CHAT_SCRIPT              "ril.ppp.script"

#define MODEM_PROPHET                         "/dev/ttyS20"
#define RIL_FIFO                              "/data/system/RIL_FIFO"
#define SIMSTATUS                          "/sys/class/simstatus/simstatus"

extern char g_imsi[16] ;

//modem type
enum AMLOGIC_MODEM_TYPE{
	RIL_MODEM_TYPE_USB_DONGLE = 0,
	RIL_MODEM_TYPE_PCIE_MODULE,
	RIL_MODEM_TYPE_MAX
};

//arch  type
enum AMLOGIC_MODEM_ARCH_TYPE{
	RIL_MODEM_ARCH_TYPE_GSM = 0,
	RIL_MODEM_ARCH_TYPE_CDMA,
	RIL_MODEM_ARCH_TYPE_DEPENDED,
	RIL_MODEM_ARCH_TYPE_MAX
};

//network type
#define RIL_MODEM_NETWORK_TYPE_GSM                    0x00000001
#define RIL_MODEM_NETWORK_TYPE_CDMA                   0x00000002
#define RIL_MODEM_NETWORK_TYPE_WCDMA                  0x00000004
#define RIL_MODEM_NETWORK_TYPE_EVDO                   0x00000008
#define RIL_MODEM_NETWORK_TYPE_TDCDMA                 0x00000010
#define RIL_MODEM_NETWORK_TYPE_HSDPA                  0x00000020
#define RIL_MODEM_NETWORK_TYPE_HSUPA                  0x00000040
#define RIL_MODEM_NETWORK_TYPE_LTE                    0x00000080
#define RIL_MODEM_NETWORK_TYPE_PHS                    0x00000100
#define RIL_MODEM_NETWORK_TYPE_GPRS                   0x00000200
#define RIL_MODEM_NETWORK_TYPE_EDGE                   0x00000400

//services type
#define RIL_SERVICE_TYPE_VOICE_CALL                   0x00000001
#define RIL_SERVICE_TYPE_DATA_CALL                    0x00000002
#define RIL_SERVICE_TYPE_SIM                          0x00000004
#define RIL_SERVICE_TYPE_STK                          0x00000008
#define RIL_SERVICE_TYPE_SMS                          0x00000010
#define RIL_SERVICE_TYPE_EMS                          0x00000020
#define RIL_SERVICE_TYPE_MMS                          0x00000040
#define RIL_SERVICE_TYPE_NETWORK                      0x00000080
#define RIL_SERVICE_TYPE_USSD                         0x00000100
#define RIL_SERVICE_TYPE_DEVICE                       0x00000200

//
enum AMLOGIC_USBERIAL{
	AMLOGIC_USBSERIAL_MIN =-1,
	AMLOGIC_USBSERIAL_MODEM = 0,
	AMLOGIC_USBSERIAL_SERVICE,
	AMLOGIC_USBSERIAL_VOICE,
	AMLOGIC_USBSERIAL_DIAG,
	AMLOGIC_USBSERIAL_GPS,
	
	AMLOGIC_USBSERIAL_MODEM_EX ,    
	AMLOGIC_USBSERIAL_SERVICE_EX,
	
	AMLOGIC_USBSERIAL_MAX
};

typedef struct {
    int vid;
    int pid;
    const char *name;
    unsigned int type;
    const char *modem; 
    const char *service;
    const char *diag;
    const char *voice;
    const char *gps;
	
    const char *modem_ex;  /* is the number of modem port in all ttyUSBs */
    const char *service_ex;

    unsigned int networktype;
    unsigned int services;
} MdmInfo;

int getDeviceIndexByVidPid(int vid, int pid);
int getDeviceIndex();
const char * getDeviceByIndex(int index, unsigned int type);
int getArchByIndex(int index);
const char * getDevice(unsigned int type);
int isModemSpecial(void);
int getModemType();
int getSupportServices();
void dumpModemSupportList();
unsigned int getModemSupportList( MdmInfo *mdmInfo );
void setPPPScript(int arch, const char * operater);

#endif
