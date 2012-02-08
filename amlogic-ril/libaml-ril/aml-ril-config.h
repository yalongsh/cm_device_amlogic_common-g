#ifndef _AML_RIL_CONFIG_H
#define _AML_RIL_CONFIG_H

#include <telephony/ril.h>

//#define  VENDOR_PROWAVE

//#define ARCH_CDMA
#define ARCH_GSM

#ifdef ARCH_GSM
#define ARCH "GSM"
#endif

#ifdef ARCH_CDMA
#define ARCH "CDMA"
#endif

#define RIL_IMEISV_VERSION  2
#define AMLOGIC_RIL_VERSION 2
#define RIL_DRIVER_VERSION "amlogic ril 2.0"
#define MAX_AT_RESPONSE (8 * 1024)
#define START_PPPD_TIMEOUT 	10 //20*2s

#define CONNECT_DOG_TIMEOUT		10//s

#define STOP_PPPD_TIMEOUT		30//s

#define SYS_NET_PATH   "/sys/class/net"
#define PPP_INTERFACE  "ppp0"
#endif

