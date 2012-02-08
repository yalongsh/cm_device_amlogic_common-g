#ifndef _AML_RIL_DEVICE_H
#define  _AML_RIL_DEVICE_H

void requestGetIMSI(void *data, size_t datalen, RIL_Token t);
void requestGetIMEI(void *data, size_t datalen, RIL_Token t);
void requestGetIMEISV(void *data, size_t datalen, RIL_Token t);
void requestDeviceIdentity(void *data, size_t datalen, RIL_Token t);
void requestBasebandVersion(void *data, size_t datalen, RIL_Token t);

#endif
