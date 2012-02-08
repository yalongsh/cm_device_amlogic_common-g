#ifndef AML_RIL_MESSAGING_H
#define AML_RIL_MESSAGING_H

void requestSendSMS(void *data, size_t datalen, RIL_Token t);
void requestSendSMSExpectMore(void *data, size_t datalen, RIL_Token t);
void requestSMSAcknowledge(void *data, size_t datalen, RIL_Token t);
void requestWriteSmsToSim(void *data, size_t datalen, RIL_Token t);
void requestDeleteSmsOnSim(void *data, size_t datalen, RIL_Token t);
void requestGetSMSCAddress(void *data, size_t datalen, RIL_Token t);
void requestSetSMSCAddress(void *data, size_t datalen, RIL_Token t);
void requestGSMGetBroadcastSMSConfig(void *data, size_t datalen, RIL_Token t);
void requestGSMSetBroadcastSMSConfig(void *data, size_t datalen, RIL_Token t);
void requestGSMSMSBroadcastActivation(void *data, size_t datalen, RIL_Token t);

#endif
