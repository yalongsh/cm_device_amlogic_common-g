#ifndef _DEBUG_H
#define _DEBUG_H


#define DBG_CALL 	0x00000001
#define DBG_SMS  	0x00000002
#define DBG_NET  	0x00000004
#define DBG_PDP 	0x00000008
#define DBG_SIM		0x00000010
#define DBG_AT		0x00000020

#define DBG_MASK_ALL  DBG_CALL|DBG_SMS |DBG_NET|DBG_PDP|DBG_SIM|DBG_AT
#define DBG_MASK_ALL_NOAT  DBG_CALL|DBG_SMS |DBG_NET|DBG_PDP|DBG_SIM

#define DBG_LEVEL_FORCE    0x0
#define DBG_LEVEL_CRITICAL 0x1
#define DBG_LEVEL_ERROR    0x2
#define DBG_LEVEL_INFO     0x3
#define DBG_LEVEL_DETAIL   0x4
#define DBG_LEVEL_DATA     0x5
#define DBG_LEVEL_TRACE    0x6

#define AML_DEBUG

extern unsigned  int DebugMask;
extern unsigned  int DebugLevel;

#ifndef AML_DEBUG

#define dbg(mask,level,fmt,arg...)

#else

#define dbg(mask,level,fmt,arg...) do{\
        if ( ( (DebugMask & mask) && DebugLevel >= level)   || (level == DBG_LEVEL_FORCE) ){ \
              LOGD(fmt,## arg);\
        }\
}while(0);

#define dbg_err(mask,fmt,arg...) dbg(mask,DBG_LEVEL_ERROR,fmt,##arg)
#define dbg_info(mask,fmt,arg...) dbg(mask,DBG_LEVEL_INFO,fmt,##arg)
#define dbg_detail(mask,fmt,arg...) dbg(mask,DBG_LEVEL_DETAIL,fmt,##arg)


#endif

#endif
