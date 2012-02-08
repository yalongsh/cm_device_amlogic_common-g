#ifndef AML_RIL_H
#define AML_RIL_H 1

#include <pthread.h>

#include "atchannel.h"
#include "at_tok.h"
#include "misc.h"
#include "aml-ril-config.h"


#ifdef RIL_SHLIB
extern const struct RIL_Env *s_rilenv;
extern int s_closed;

#define RIL_onRequestComplete(t, e, response, responselen) s_rilenv->OnRequestComplete(t,e, response, responselen)
#define RIL_onUnsolicitedResponse(a,b,c) s_rilenv->OnUnsolicitedResponse(a,b,c)
#define RIL_requestTimedCallback(a,b,c) s_rilenv->RequestTimedCallback(a,b,c)
#endif

extern  unsigned int s_arch;
extern  int s_modemtype ;
extern  int s_cur_device_idx;
extern  const char * s_service_device;
extern  const char * s_modem_device;
#endif
