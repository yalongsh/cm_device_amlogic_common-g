#ifndef AML_RIL_OEM_H
#define AML_RIL_OEM_H

void requestOEMHookRaw(void *data, size_t datalen, RIL_Token t);
void requestOEMHookStrings(void *data, size_t datalen, RIL_Token t);

#endif
