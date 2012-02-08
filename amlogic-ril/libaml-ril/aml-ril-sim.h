#include <telephony/ril.h>
#include "aml-ril-config.h"

#ifndef AML_RIL_SIM_H
#define AML_RIL_SIM_H

typedef enum {
    SIM_ABSENT = 0,
    SIM_NOT_READY = 1,
    SIM_READY = 2, /* SIM_READY means the radio state is RADIO_STATE_SIM_READY */
    SIM_PIN = 3,
    SIM_PUK = 4,
    SIM_NETWORK_PERSONALIZATION = 5
} SIM_Status; 


void onSimStateChanged(const char *s);
void onSIMReady();

void requestGetSimStatus(void *data, size_t datalen, RIL_Token t);
void requestSIM_IO(void *data, size_t datalen, RIL_Token t);
void requestEnterSimPin(void *data, size_t datalen, RIL_Token t);
void requestChangePassword(void *data, size_t datalen, RIL_Token t) ; // , char *facility);
void requestSetFacilityLock(void *data, size_t datalen, RIL_Token t);
void requestQueryFacilityLock(void *data, size_t datalen, RIL_Token t);

void pollSIMState(void *param);
SIM_Status getSIMStatus(int falce);

//#ifdef ARCH_GSM
//#define RIL_APPTYPE RIL_APPTYPE_SIM
//#define RIL_PERSOSUBSTATE_NETWORK  RIL_PERSOSUBSTATE_SIM_NETWORK

//#endif

//#ifdef ARCH_CDMA
//#define RIL_APPTYPE RIL_APPTYPE_RUIM  
//#define RIL_PERSOSUBSTATE_NETWORK      RIL_PERSOSUBSTATE_RUIM_NETWORK1
//#endif

#endif
