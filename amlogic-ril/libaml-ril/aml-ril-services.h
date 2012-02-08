#ifndef AML_RIL_SERVICES_H
#define AML_RIL_SERVICES_H

void requestQueryClip(void *data, size_t datalen, RIL_Token t);
void requestCancelUSSD(void *data, size_t datalen, RIL_Token t);
void requestSendUSSD(void *data, size_t datalen, RIL_Token t);
void requestGetCLIR(void *data, size_t datalen, RIL_Token t);
void requestSetCLIR(void *data, size_t datalen, RIL_Token t);
void requestQueryCallForwardStatus(void *data, size_t datalen,
                                   RIL_Token t);
void requestSetCallForward(void *data, size_t datalen, RIL_Token t);
void requestQueryCallWaiting(void *data, size_t datalen, RIL_Token t);
void requestSetCallWaiting(void *data, size_t datalen, RIL_Token t);
void requestSetSuppSvcNotification(void *data, size_t datalen,
                                   RIL_Token t);

void onSuppServiceNotification(const char *s, int type);
void onUSSDReceived(const char *s);

#endif
