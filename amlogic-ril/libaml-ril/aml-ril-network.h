#ifndef AML_RIL_NETWORK_H
#define AML_RIL_NETWORK_H
#include <telephony/ril.h>
#include "aml-ril-config.h"


extern RIL_RadioState sState;
extern pthread_mutex_t s_state_mutex;
extern pthread_cond_t s_state_cond;

void setRadioState(RIL_RadioState newState);
int isRadioOn();

void requestSetNetworkSelectionAutomatic(void *data, size_t datalen,
                                         RIL_Token t);
void requestSetNetworkSelectionManual(void *data, size_t datalen,
                                      RIL_Token t);
void requestQueryAvailableNetworks(void *data, size_t datalen,
                                   RIL_Token t);
void requestSetPreferredNetworkType(void *data, size_t datalen,
                                    RIL_Token t);
void requestGetPreferredNetworkType(void *data, size_t datalen,
                                    RIL_Token t);
void requestQueryNetworkSelectionMode(void *data, size_t datalen,
                                      RIL_Token t);
void requestSignalStrength(void *data, size_t datalen, RIL_Token t);
void requestRegistrationState(void *data,size_t datalen, RIL_Token t);

//#ifdef ARCH_GSM
void requestGPRSRegistrationState(void *data, size_t datalen, RIL_Token t);
//#endif

void requestOperator(void *data, size_t datalen, RIL_Token t);
void requestRadioPower(void *data, size_t datalen, RIL_Token t);
void requestSetLocationUpdates(void *data, size_t datalen, RIL_Token t);
void requestCDMASubScription(void *data, size_t datalen, RIL_Token t);
void requestQueryFacilityLock(void *data, size_t datalen, RIL_Token t);
void requestSetFacilityLock(void *data, size_t datalen, RIL_Token t);
int switch_signal_strength(int rssi,int evdo);

//#ifdef ARCH_GSM

//#define RADIO_STATE_NOT_READY				RADIO_STATE_SIM_NOT_READY
//#define RADIO_STATE_READY	  				RADIO_STATE_SIM_READY       /* Radio is on and the RUIM interface is available */
//#define RADIO_STATE_LOCKED_OR_ABSENT 	RADIO_STATE_SIM_LOCKED_OR_ABSENT

//#endif

//#ifdef ARCH_CDMA

//#define RADIO_STATE_NOT_READY				RADIO_STATE_RUIM_NOT_READY
//#define RADIO_STATE_READY	  				RADIO_STATE_RUIM_READY       /* Radio is on and the RUIM interface is available */
//#define RADIO_STATE_LOCKED_OR_ABSENT 	RADIO_STATE_RUIM_LOCKED_OR_ABSENT
//#endif

#endif
