#ifndef AML_RIL_PDP_H
#define AML_RIL_PDP_H

int   pdp_handle_init();
void pdp_handle_uninit();

void requestOrSendPDPContextList(RIL_Token *t);
void onPDPContextListChanged(void *param);
void requestPDPContextList(void *data, size_t datalen, RIL_Token t);
void requestSetupDefaultPDP(void *data, size_t datalen, RIL_Token t);
void requestDeactivateDefaultPDP(void *data, size_t datalen, RIL_Token t);
void requestLastPDPFailCause(void *data, size_t datalen, RIL_Token t);
void requestScreenState(void *data, size_t datalen, RIL_Token t);


void dog_ping(void);
//pppd event
#define EVENT_PPPD_MANAGER_DISCONNECT     		0x00
#define EVENT_PPPD_MANAGER_PPPD_EXIT    		0x01
#define EVENT_PPPD_MANAGER_START  				0x02
#define EVENT_PPPD_MANAGER_STOP    				0x03
#define EVENT_PPPD_MANAGER_PAUSE    			0x04
#define EVENT_PPPD_MANAGER_CONT    				0x05
#define EVENT_PPPD_MANAGER_EXIT     			0x06


//screen event
#define EVENT_DOG_ON		0x00
#define EVENT_DOG_OFF		0x01	
#define EVENT_DOG_PING	0x02
#define EVENT_DOG_EXIT		0x03
#define EVENT_DOG_ETIMEDOUT ETIMEDOUT


enum DOG_STAT{
    DOG_STAT_STOP = 0,
    DOG_STAT_START
};
#endif
