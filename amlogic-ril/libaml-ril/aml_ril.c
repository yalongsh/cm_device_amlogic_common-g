#include <telephony/ril.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <alloca.h>
#include <getopt.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>

#include "aml-ril.h"
#include "aml-ril-config.h"
#include "aml-ril-callhandling.h"
#include "aml-ril-messaging.h"
#include "aml-ril-network.h"
#include "aml-ril-pdp.h"
#include "aml-ril-services.h"
#include "aml-ril-sim.h"
#include "aml-ril-oem.h"
#include "aml-ril-requestdatahandler.h"
#include "aml-ril-device.h"
#include "ril-compatible.h"
#define LOG_NDEBUG 0
#define LOG_TAG "AML_RIL"
#include <utils/Log.h>
#include "debug.h"

static void onRequest (int request, void *data, size_t datalen, RIL_Token t);
static RIL_RadioState currentState();
static int onSupports (int requestCode);
static void onCancel (RIL_Token t);
static const char *getVersion();

unsigned  int DebugMask = DBG_MASK_ALL_NOAT;
unsigned  int DebugLevel = DBG_LEVEL_INFO;

typedef void (*request_function)(void *data, size_t datalen, RIL_Token t);
typedef struct S_Request_Item
{
    int request;
    request_function func;
}Request_Item,*PRequest_Item;

#define REQUEST_ITEM(X,Y) {X,Y}

Request_Item Request_Table[]=
{

//Basic Voice Call
REQUEST_ITEM(RIL_REQUEST_LAST_CALL_FAIL_CAUSE,requestLastCallFailCause),
REQUEST_ITEM(RIL_REQUEST_GET_CURRENT_CALLS,requestGetCurrentCalls),
REQUEST_ITEM(RIL_REQUEST_DIAL,requestDial),
REQUEST_ITEM(RIL_REQUEST_HANGUP,requestHangup),
REQUEST_ITEM(RIL_REQUEST_ANSWER,requestAnswer),

/* Advanced Voice Call */

/*REQUEST_ITEM(RIL_REQUEST_GET_CLIR,requestGetCLIR),
REQUEST_ITEM(RIL_REQUEST_SET_CLIR,requestSetCLIR),
REQUEST_ITEM(RIL_REQUEST_QUERY_CALL_FORWARD_STATUS,requestQueryCallForwardStatus),
REQUEST_ITEM(RIL_REQUEST_SET_CALL_FORWARD,requestSetCallForward),
REQUEST_ITEM(RIL_REQUEST_QUERY_CALL_WAITING,requestQueryCallWaiting),
REQUEST_ITEM(RIL_REQUEST_SET_CALL_WAITING,requestSetCallWaiting),
REQUEST_ITEM(RIL_REQUEST_UDUB,requestUDUB),
REQUEST_ITEM(RIL_REQUEST_GET_MUTE,requestGetMute),
REQUEST_ITEM(RIL_REQUEST_SET_MUTE,requestSetMute),

//REQUEST_ITEM(RIL_REQUEST_SCREEN_STATE,RIL_onRequestComplete),//modify
REQUEST_ITEM(RIL_REQUEST_QUERY_CLIP,requestQueryClip),
REQUEST_ITEM(RIL_REQUEST_DTMF,requestDTMF),
REQUEST_ITEM(RIL_REQUEST_DTMF_START,requestDTMFStart),
REQUEST_ITEM(RIL_REQUEST_DTMF_STOP,requestDTMFStop),
*/

/* Multiparty Voice Call */
/*REQUEST_ITEM(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,requestHangupWaitingOrBackground),
REQUEST_ITEM(RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,requestHangupForegroundResumeBackground),
REQUEST_ITEM(RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,requestSwitchWaitingOrHoldingAndActive),
REQUEST_ITEM(RIL_REQUEST_CONFERENCE,requestConference),
REQUEST_ITEM(RIL_REQUEST_SEPARATE_CONNECTION,requestSeparateConnection),
REQUEST_ITEM(RIL_REQUEST_EXPLICIT_CALL_TRANSFER,requestExplicitCallTransfer),*/

/* Data Call Requests */

REQUEST_ITEM(RIL_REQUEST_SETUP_DATA_CALL,requestSetupDefaultPDP),
REQUEST_ITEM(RIL_REQUEST_DEACTIVATE_DATA_CALL,requestDeactivateDefaultPDP),
REQUEST_ITEM(RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE,requestLastPDPFailCause),
REQUEST_ITEM(RIL_REQUEST_DATA_CALL_LIST,requestPDPContextList),


 /* SMS Requests */
/*REQUEST_ITEM(RIL_REQUEST_SEND_SMS,requestSendSMS),
REQUEST_ITEM(RIL_REQUEST_WRITE_SMS_TO_SIM,requestWriteSmsToSim),
REQUEST_ITEM(RIL_REQUEST_DELETE_SMS_ON_SIM,requestDeleteSmsOnSim),
REQUEST_ITEM(RIL_REQUEST_SMS_ACKNOWLEDGE,requestSMSAcknowledge),
REQUEST_ITEM(RIL_REQUEST_SEND_SMS_EXPECT_MORE,requestSendSMSExpectMore),
REQUEST_ITEM(RIL_REQUEST_GET_SMSC_ADDRESS,requestGetSMSCAddress),
REQUEST_ITEM(RIL_REQUEST_SET_SMSC_ADDRESS,requestSetSMSCAddress),
REQUEST_ITEM(RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG,requestGSMGetBroadcastSMSConfig),
REQUEST_ITEM(RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG,requestGSMSetBroadcastSMSConfig),
REQUEST_ITEM(RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION,requestGSMSMSBroadcastActivation),
REQUEST_ITEM(RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION,requestSetSuppSvcNotification),*/

/* SIM Handling Requests */

REQUEST_ITEM(RIL_REQUEST_GET_SIM_STATUS,requestGetSimStatus),
REQUEST_ITEM(RIL_REQUEST_SIM_IO,requestSIM_IO),
REQUEST_ITEM(RIL_REQUEST_ENTER_SIM_PIN,requestEnterSimPin),
REQUEST_ITEM(RIL_REQUEST_ENTER_SIM_PUK,requestEnterSimPin),
REQUEST_ITEM(RIL_REQUEST_ENTER_SIM_PIN2,requestEnterSimPin),
REQUEST_ITEM(RIL_REQUEST_ENTER_SIM_PUK2,requestEnterSimPin),
REQUEST_ITEM(RIL_REQUEST_CHANGE_SIM_PIN,requestChangePassword),
REQUEST_ITEM(RIL_REQUEST_CHANGE_SIM_PIN2,requestEnterSimPin),


/* USSD Requests */
/*REQUEST_ITEM(RIL_REQUEST_SEND_USSD,requestSendUSSD),
REQUEST_ITEM(RIL_REQUEST_CANCEL_USSD,requestCancelUSSD),*/

/* Network */

REQUEST_ITEM(RIL_REQUEST_REGISTRATION_STATE,requestRegistrationState),

//#ifdef ARCH_GSM
REQUEST_ITEM(RIL_REQUEST_GPRS_REGISTRATION_STATE,requestGPRSRegistrationState),
//#endif

REQUEST_ITEM(RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE,requestQueryNetworkSelectionMode),
REQUEST_ITEM(RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC,requestSetNetworkSelectionAutomatic),
REQUEST_ITEM(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL,requestSetNetworkSelectionManual),
REQUEST_ITEM(RIL_REQUEST_QUERY_AVAILABLE_NETWORKS,requestQueryAvailableNetworks),
REQUEST_ITEM(RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE,requestSetPreferredNetworkType),
REQUEST_ITEM(RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE,requestGetPreferredNetworkType),
REQUEST_ITEM(RIL_REQUEST_SIGNAL_STRENGTH,requestSignalStrength),
REQUEST_ITEM(RIL_REQUEST_OPERATOR,requestOperator),
REQUEST_ITEM(RIL_REQUEST_RADIO_POWER,requestRadioPower),

//#ifdef ARCH_CDMA
REQUEST_ITEM(RIL_REQUEST_CDMA_SUBSCRIPTION,requestCDMASubScription),
//#endif

REQUEST_ITEM(RIL_REQUEST_QUERY_FACILITY_LOCK,requestQueryFacilityLock),
REQUEST_ITEM(RIL_REQUEST_SET_FACILITY_LOCK,requestSetFacilityLock),

/* OEM */

/*REQUEST_ITEM(RIL_REQUEST_OEM_HOOK_RAW,requestOEMHookRaw),
REQUEST_ITEM(RIL_REQUEST_OEM_HOOK_STRINGS,requestOEMHookStrings),
REQUEST_ITEM(RIL_REQUEST_LAST_CALL_FAIL_CAUSE,requestLastCallFailCause),*/


/* Device */

REQUEST_ITEM(RIL_REQUEST_GET_IMSI,requestGetIMSI),
REQUEST_ITEM(RIL_REQUEST_GET_IMEI,requestGetIMEI),
REQUEST_ITEM(RIL_REQUEST_GET_IMEISV,requestGetIMEISV),
REQUEST_ITEM(RIL_REQUEST_DEVICE_IDENTITY,requestDeviceIdentity),
REQUEST_ITEM(RIL_REQUEST_BASEBAND_VERSION,requestBasebandVersion),

//other
REQUEST_ITEM( RIL_REQUEST_SCREEN_STATE,requestScreenState),
};

int Ignore_List_cdma[]= {
RIL_REQUEST_GPRS_REGISTRATION_STATE,
};

#define REQUEST_TABLE_LENG  sizeof(Request_Table)/sizeof(Request_Table[0])

extern const char * requestToString(int request);

/*** Static Variables ***/
static const RIL_RadioFunctions s_callbacks = {
    AMLOGIC_RIL_VERSION,
    onRequest,
    currentState,
    onSupports,
    onCancel,
    getVersion
};

const struct RIL_Env *s_rilenv;


static int s_port = -1;
int s_cur_device_idx = -1;
unsigned int s_arch = RIL_MODEM_ARCH_TYPE_GSM;
int s_modemtype = RIL_MODEM_TYPE_PCIE_MODULE ;
int simcard_detect_loop = 1 ;
const char * s_service_device = NULL;
const char * s_modem_device  = NULL;
static int          s_device_socket = 0;

/* trigger change to this with s_state_cond */
int s_closed = 0;

static int sFD;     /* file desc of AT channel */
static char sATBuffer[MAX_AT_RESPONSE+1];
static char *sATBufferCur = NULL;

static const struct timeval TIMEVAL_0 = {0,0};

static void sendCallStateChanged(void *param)
{
    RIL_onUnsolicitedResponse (
        RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
        NULL, 0);
}


/*** Callback methods from the RIL library to us ***/

/**
 * Call from RIL to us to make a RIL_REQUEST
 *
 * Must be completed with a call to RIL_onRequestComplete()
 *
 * RIL_onRequestComplete() may be called from any thread, before or after
 * this function returns.
 *
 * Will always be called from the same thread, so returning here implies
 * that the radio is ready to process another command (whether or not
 * the previous command has completed).
 */
static void
onRequest (int request, void *data, size_t datalen, RIL_Token t)
{
    ATResponse *p_response;
    int err;
    unsigned  int i =0;


	if(RIL_REQUEST_SIGNAL_STRENGTH != request){
	    LOGD("onRequest: %s", requestToString(request));
	}

    /* Ignore all requests except RIL_REQUEST_GET_SIM_STATUS
     * when RADIO_STATE_UNAVAILABLE.
     */
    if (sState == RADIO_STATE_UNAVAILABLE
        &&!( request == RIL_REQUEST_GET_SIM_STATUS)
     ) {
   	 	LOGD("onRequest: RIL_E_RADIO_NOT_AVAILABLE 1.");
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /* Ignore all non-power requests when RADIO_STATE_OFF
     * (except RIL_REQUEST_GET_SIM_STATUS)
     */
    if (sState == RADIO_STATE_OFF
        && !(request == RIL_REQUEST_RADIO_POWER
            || request == RIL_REQUEST_GET_SIM_STATUS
            ||request ==  RIL_REQUEST_DEACTIVATE_DATA_CALL)
    ) {
    	LOGD("onRequest: RIL_E_RADIO_NOT_AVAILABLE 2.");
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

	if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
		for(i=0; i< sizeof(Ignore_List_cdma)/sizeof(Ignore_List_cdma[0]); i++){
			if(Ignore_List_cdma[i] == request){
				RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
			}
		}
	}

	for(i=0;i<REQUEST_TABLE_LENG;i++){
		if(Request_Table[i].request == request && Request_Table[i].func){
			(*Request_Table[i].func)(data, datalen, t);
   			break;
		}
	}

	if(i >= REQUEST_TABLE_LENG){
		RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
	}
}

/**
 * Synchronous call from the RIL to us to return current radio state.
 * RADIO_STATE_UNAVAILABLE should be the initial state.
 */
static RIL_RadioState
currentState()
{
    return sState;
}
/**
 * Call from RIL to us to find out whether a specific request code
 * is supported by this implementation.
 *
 * Return 1 for "supported" and 0 for "unsupported"
 */

static int
onSupports (int requestCode)
{
    //@@@ todo

    return 1;
}

static void onCancel (RIL_Token t)
{
    //@@@todo

}

static const char * getVersion(void)
{
    return RIL_DRIVER_VERSION ARCH;
}



/**
 * Initialize everything that can be configured while we're still in
 * AT+CFUN=0
 */
static void initializeCallback(void *param)
{
    ATResponse *p_response = NULL;
    int err;
        
    setRadioState (RADIO_STATE_OFF);

    at_handshake();

    /* note: we don't check errors here. Everything important will
       be handled in onATTimeout and onATReaderClosed */

    /*  atchannel is tolerant of echo but it must */
    /*  have verbose result codes */
    at_send_command("ATE0", NULL);//决定是否回显字符ATE0Q0V1

    /*  No auto-answer */
    at_send_command("ATS0=0", NULL);// 关闭自动应答

    /*  Extended errors */
    //报告移动设备的错误。
    //这个命令决定允许或不允许用结果码

    at_send_command("AT+CMEE=1", NULL);

	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch ){
	    /*  Network registration events */
	    err = at_send_command("AT+CREG=2", &p_response);
		
	    /* some handsets -- in tethered mode -- don't support CREG=2 */
	    if (err < 0 || p_response->success == 0) {
	        at_send_command("AT+CREG=1", NULL);
		}
	}
	else if(RIL_MODEM_ARCH_TYPE_DEPENDED== s_arch){
            /*  Network registration events */
            err = at_send_command("AT+CREG=2", &p_response);
            if (err < 0 || p_response->success == 0) {

                if(p_response->atCmdNotSupport ){
                    s_arch = RIL_MODEM_ARCH_TYPE_CDMA;
                }
                else if(p_response->finalResponse && !strcmp("ERROR",p_response->finalResponse)){
                    s_arch = RIL_MODEM_ARCH_TYPE_CDMA;
                }
                else{
                    at_send_command("AT+CREG=1", NULL);
                    s_arch = RIL_MODEM_ARCH_TYPE_GSM;
                }
            }else{
                s_arch = RIL_MODEM_ARCH_TYPE_GSM;
            }
	}

    	LOGE("initializeCallback arch(%d)\n",s_arch);
    
	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){

		at_response_free(p_response);

		/*  GPRS registration events */
		at_send_command("AT+CGREG=1", NULL);

		/*  Call Waiting notifications */
		at_send_command("AT+CCWA=1", NULL);

		/*  Alternating voice/data off */
		at_send_command("AT+CMOD=0", NULL);

		/*  Not muted */
		//at_send_command("AT+CMUT=0", NULL);

		/*  +CSSU unsolicited supp service notifications */
		at_send_command("AT+CSSN=0,1", NULL);

		/*  no connected line identification */
		at_send_command("AT+COLP=0", NULL);

		/*  HEX character set */
		at_send_command("AT+CSCS=\"HEX\"", NULL);

		/*  USSD unsolicited */
		at_send_command("AT+CUSD=1", NULL);

		/*  Enable +CGEV GPRS event notifications, but don't buffer */
		at_send_command("AT+CGEREP=1,0", NULL);

		at_send_command("AT+CTA=5", NULL);

	    if( isRadioOn() > 0 ){
			setRadioState(RADIO_STATE_SIM_NOT_READY);
	    }
	}

    /*  SMS 1: TEXT;0:PDU mode */
    at_send_command("AT+CMGF=0", NULL);

	if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
	    at_send_command("AT+CRM=2", NULL);
	    at_send_command("AT+ZPCNT=1", NULL);
	 
	    at_send_command("AT+CTA=5", NULL);

		setRadioState(RADIO_STATE_RUIM_NOT_READY);

	}
 
}

static void waitForClose()
{
    pthread_mutex_lock(&s_state_mutex);

    while (s_closed == 0) {
        pthread_cond_wait(&s_state_cond, &s_state_mutex);
    }

    pthread_mutex_unlock(&s_state_mutex);
}

/**
 * Called by atchannel when an unsolicited line appears
 * This is called on atchannel's reader thread. AT commands may
 * not be issued here
 */
static void onUnsolicited (const char *s, const char *sms_pdu)
{
    char *line = NULL;
    int err;

    /* Ignore unsolicited responses until we're initialized.
     * This is OK because the RIL library will poll for initial state
     */
    if (sState == RADIO_STATE_UNAVAILABLE) {
        return;
    }

    if (strStartsWith(s, "%CTZV:")) {
        /* TI specific -- NITZ time */
        char *response;

        line = strdup(s);
        at_tok_start(&line);

        err = at_tok_nextstr(&line, &response);

        if (err != 0) {
            LOGE("invalid NITZ line %s\n", s);
        } else {
            RIL_onUnsolicitedResponse (
                RIL_UNSOL_NITZ_TIME_RECEIVED,
                response, strlen(response));
        }
         free(line);
    } else if (strStartsWith(s,"+CRING:")
                || strStartsWith(s,"RING")
                || strStartsWith(s,"NO CARRIER")
                || strStartsWith(s,"+CCWA")
    ) {
        RIL_onUnsolicitedResponse (
            RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
            NULL, 0);
    } else if (strStartsWith(s,"+CREG:")
                || strStartsWith(s,"+CGREG:")
                || strStartsWith(s,"^MODE:")
    ) {
       // RIL_onUnsolicitedResponse ( //@@
            //RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED,
            //NULL, 0);
    } else if (strStartsWith(s, "+CMT:")) {
        RIL_onUnsolicitedResponse (
            RIL_UNSOL_RESPONSE_NEW_SMS,
            sms_pdu, strlen(sms_pdu));
    } else if (strStartsWith(s, "+CDS:")) {
        RIL_onUnsolicitedResponse (
            RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT,
            sms_pdu, strlen(sms_pdu));
    } else if (strStartsWith(s, "+CGEV:")) {
        /* Really, we can ignore NW CLASS and ME CLASS events here,
         * but right now we don't since extranous
         * RIL_UNSOL_DATA_CALL_LIST_CHANGED calls are tolerated
         */
        /* can't issue AT commands here -- call on main thread */
        RIL_requestTimedCallback (onPDPContextListChanged, NULL, NULL);
    }else if (strStartsWith(s, "^RESUME:")) {
        dog_ping();
    }else if ( strStartsWith(s, "^HRSSILVL:")){
        int dbm=0;
        RIL_SignalStrength response={{0,0},{0,0},{0,0,0}};
         line = strdup(s);
        at_tok_start(&line);
        err = at_tok_nextint(&line, &dbm);

        if (err != 0) {
            LOGE("invalid HRSSILVL  line %s\n",s);
        } else {
          	 response.EVDO_SignalStrength.dbm = switch_signal_strength(dbm,1);
    		 response.EVDO_SignalStrength.ecio = 0;//ecio;
   		 response.EVDO_SignalStrength.signalNoiseRatio =8;
           	 RIL_onUnsolicitedResponse (
                	RIL_UNSOL_SIGNAL_STRENGTH,
               		& response, sizeof(response));
       	 }
        free(line);
    }
}

/* Called on command or reader thread */
void onATReaderClosed()
{
    LOGI("AT channel closed\n");
    at_close();
    s_closed = 1;

    setRadioState (RADIO_STATE_UNAVAILABLE);

    simcard_detect_loop = 0;
}

/* Called on command thread */
static void onATTimeout()
{
    LOGI("AT channel timeout; closing\n");
    at_close();

    s_closed = 1;

    setRadioState (RADIO_STATE_UNAVAILABLE);
    
    simcard_detect_loop = 0;
}

static void usage(char *s)
{
#ifdef RIL_SHLIB
    fprintf(stderr, "reference-ril requires: -p <tcp port> or -d /dev/tty_device\n");
#else
    fprintf(stderr, "usage: %s [-p <tcp port>] [-d /dev/tty_device]\n", s);
    exit(-1);
#endif
}

static int get_ttyUSBDev(char *ttyUSBArray[], int *ttyUSBNum)
{
    DIR *pDevDir = NULL ;
    struct dirent *dirp = NULL;
    int cnt = 0 ;

    *ttyUSBNum = 0;
    
    pDevDir = opendir("/dev");
    if(pDevDir == NULL){
        LOGE("failed to opendir(/dev),err:%s",strerror(errno));
        return -1 ;
    }
    
    while((dirp=readdir(pDevDir)) != NULL){
        if(strstr(dirp->d_name, "ttyUSB") != NULL){
            if(0 < snprintf(ttyUSBArray[cnt], 64,"/dev/%s", dirp->d_name) ){
                cnt++ ;
            }
            
            LOGD("%s, cnt = %d",dirp->d_name, cnt);
        }
    }
    closedir(pDevDir);
    *ttyUSBNum = cnt ;
    if(cnt >= 2)
        LOGD("get_ttyUSBDev OK !!!!!!!!!!!!!\n");
    return 0 ;
}

static int creatFIFO(void){
    int ret = 0;
    if(0==access(RIL_FIFO,  R_OK)){
        if(0>remove(RIL_FIFO)){
            LOGE("failed to remove %s,err: %s",RIL_FIFO,strerror(errno));
        }
    }
    ret = mkfifo(RIL_FIFO, 0660);
    if(ret < 0){
        LOGE("failed to mkfifo(%s) ,err:%s",RIL_FIFO,strerror(errno));
        return  0 ; // when ctlfd=0 , we don't open RIL_FIFO .
    }
    else{
        LOGD("secced to mkfifo(%s) !!!!!!!!!\n",RIL_FIFO);
    }

    return 1;
}

static int selectFIFO(int *p_ctlfd)
{
    int ret = 0 ;
    int ctlfd = 0 ;
    fd_set readfds;
    char readbuff[12];
    struct timeval timeout = {5,0} ;

    FD_ZERO(&readfds);

    if(p_ctlfd != NULL)
        ctlfd = *p_ctlfd ;
    
    if(ctlfd > 0){
        FD_ZERO(&readfds);
        FD_SET(ctlfd, &readfds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0 ; 
        
        ret = select(ctlfd+1,&readfds,NULL,NULL,&timeout);
        if(ret<0){
            LOGE("failed to select(%d) ,err:%s",ctlfd,strerror(errno)) ;
            close(ctlfd);
            ctlfd = -1 ;
        }
        else if(ret>0){
            if(FD_ISSET(ctlfd, &readfds)){
                ret  = read(ctlfd, readbuff,10);
                if(ret <=0 ){
                    LOGE("failed to read(RIL_FIFO) ,err:%s",strerror(errno)) ;
                    close(ctlfd);
                    ctlfd = -1 ;
                    sleep(1);
                }
            }
        }
        else{
            //LOGD("select %s timeout !!!!!!\n", RIL_FIFO);
        }
    }
    if(ctlfd<0)
        *p_ctlfd = ctlfd ;
        

    return ctlfd ;
}

static int getSimstatusFd(void)
{
    int fd = 0 ;
    if(0>access(SIMSTATUS, F_OK)){
        return -1 ;
    }
    
    fd = open(SIMSTATUS, O_RDONLY);
    if(fd <= 0 ){
        dbg_info(DBG_PDP,"getSimstatusFd open %s failed, err:%s \n ",SIMSTATUS,strerror(errno));
    }

    return fd ;
}

static int getSimcardstatus(int fd)
{
    char buff[8]={0};
    if(fd <= 0){
        return -1 ;
    }
    if(0 >= read(fd,buff, 1)){
        dbg_info(DBG_PDP,"getSimstatus read %s failed, err:%s \n ",SIMSTATUS,strerror(errno));
        return -1 ;
    }
    //dbg_info(DBG_PDP,"getSimstatus read buff: %s \n ",buff);

    return atoi(buff);
}

static void closeSimstatusFd(int fd)
{
    if(fd <= 0)
        return ;
    close(fd);
}
static void *simcard_detect(void *arg)
{
    pthread_detach(pthread_self());
    
    int simstatus_fd = 0 ;
    int ret = -1 ;
    int modem_power = 0;

    LOGD("simcard_detect start !!!!!!!!!!! ");
    
    while(simcard_detect_loop){
        if( RIL_MODEM_TYPE_PCIE_MODULE == s_modemtype)
        {
            simstatus_fd = getSimstatusFd();
            if(simstatus_fd < 0){
                sleep(5);
                continue ;
            }
            ret = getSimcardstatus(simstatus_fd);
            if(0==ret && 1==modem_power){
                LOGD("simcard_detect: sim status is out ");
                modem_power = 0;
                system("echo 0 > /sys/class/rfkill/rfkill1/state");
                sleep(1);
            }
            else if(1==ret  && 0==modem_power){
                LOGD("simcard_detect: sim status is in ");
                modem_power = 1 ;
                system("echo 1 > /sys/class/rfkill/rfkill1/state");
                sleep(1);
            }

            //LOGD("simcard_detect: sim status is %d modem_power is %d", ret,modem_power);
            closeSimstatusFd(simstatus_fd) ;

            sleep(3);
        }
        else {
            sleep(5) ;
        }
    }

    LOGD("simcard_detect: busybox killall rild !!!!!!!!!!!!!\n");
    system("busybox killall rild");

    return NULL ;
}

static int simcarddetect_init(void)
{
    pthread_t tid_simcarddetect;
    int ret = pthread_create(&tid_simcarddetect, NULL, simcard_detect, NULL);
    if (ret < 0) {
        LOGD("failed to pthread_create simcard_detect !!!!!!!");
        return -1 ;
    }
    
    return 0 ;
}

static void * mainLoop(void *param)
{
    int fd,mod_fd = -1;
    int ret, ret_fifo;
    int ctlfd = 0;
    int cnt = 0 ;    
    int again_cnt = 0 ;
    int service_port_id = AMLOGIC_USBSERIAL_SERVICE ;
    int modem_port_id = AMLOGIC_USBSERIAL_MODEM ;
    char ttyUSBArray[10][64] ;
    char *ppttyUSB[10] ;
    int ttyUSBNum = 0 ;
    int i = 0 ;
    
    for(i =0; i<10; i++){
        ppttyUSB[i] = ttyUSBArray[i] ;
    }

    AT_DUMP("== ", "entering mainLoop()", -1 );
    at_set_on_reader_closed(onATReaderClosed);
    at_set_on_timeout(onATTimeout);
    pdp_handle_init();

    ret_fifo = creatFIFO() ; // when creatFIFO return 0 , we don't select RIL_FIFO .
    if(ret_fifo > 0){
        ctlfd = open(RIL_FIFO, O_NONBLOCK|O_RDWR);
        if(ctlfd < 0){
            LOGE("failed to open(%s) ,err:%s",RIL_FIFO,strerror(errno));
        }
    }
    simcarddetect_init();

    for (;;) {
        fd = -1;
        while  (fd < 0) {
            if(ctlfd > 0)
                ret = selectFIFO(&ctlfd); 
            
            again_cnt = 0 ;
            service_port_id = AMLOGIC_USBSERIAL_SERVICE ;
            modem_port_id = AMLOGIC_USBSERIAL_MODEM ;
            s_cur_device_idx = getDeviceIndex();
            if(s_cur_device_idx < 0){
                //LOGD("there is no modem in OTG !!!!!!\n");
                if(ctlfd <= 0)
                {
                    sleep(5) ;
                }
                continue ;
            }

GETTTYPORT: ;      
            get_ttyUSBDev(ppttyUSB , &ttyUSBNum);
            
            s_service_device = getDeviceByIndex(s_cur_device_idx, service_port_id);
            s_modem_device = getDeviceByIndex(s_cur_device_idx, modem_port_id);
            s_arch = getArchByIndex(s_cur_device_idx);
            s_modemtype = getModemTypeByIndex(s_cur_device_idx);
            // TODO add dual mode module process
            if(s_service_device == NULL){
                sleep(5);
                continue ;
            }
            if(AMLOGIC_USBSERIAL_SERVICE_EX == service_port_id  ){
                int id = atoi(s_service_device) ;
                if(id < ttyUSBNum){
                     s_service_device = ppttyUSB[ttyUSBNum-id-1] ;
                }
                id = atoi(s_modem_device) ;
                if(id < ttyUSBNum){
                     s_modem_device = ppttyUSB[ttyUSBNum-id-1] ;
                }
            }

            if(0>access(s_service_device, F_OK)){
                LOGD ("service port(%s) is not exist \n",s_service_device);
                service_port_id = AMLOGIC_USBSERIAL_SERVICE_EX;
                modem_port_id = AMLOGIC_USBSERIAL_MODEM_EX;
                if(again_cnt++ < 1){  
                    goto  GETTTYPORT;
                }
            }
            
            LOGD("s_cur_device_idx is %d ,s_arch = %d !!!!!!!!!!!!!!#############\n",s_cur_device_idx,s_arch);
            if(s_modem_device != NULL )
            {
                LOGD("AML_RIL  symlink %s with %s !!!!!!######\n",s_modem_device, MODEM_PROPHET);
                if(0>symlink(s_modem_device, MODEM_PROPHET))
                {
                	LOGD("unlink %s,then symlink %s with %s \n",MODEM_PROPHET,s_modem_device,MODEM_PROPHET);
                	unlink(MODEM_PROPHET);
                	symlink(s_modem_device, MODEM_PROPHET);
                }
            }
			
            if ( s_service_device != NULL ) {
                LOGD ("AML_RIL : get service port(%s)",s_service_device);
                fd = open (s_service_device, O_RDWR);
                if ( fd >= 0 ) {
                    /* disable echo on serial ports */
                    struct termios  ios;
                    tcgetattr( fd, &ios );
                    ios.c_lflag = 0;  /* disable ECHO, ICANON, etc... */
                    tcsetattr( fd, TCSANOW, &ios );
                }
            }

            if (fd < 0) {
                LOGD ("AML_RIL : opening AT interface: %s;err:%s,continue.... \n",s_service_device,strerror(errno));
                sleep(5);
                /* never returns */
            }  
        }

        LOGD ("open AT interface success! \n");
	
        s_closed = 0;
        ret = at_open(fd,onUnsolicited);

        if (ret < 0) {
            LOGE ("AT error %d on at_open\n", ret);
            pdp_handle_uninit();
            return 0;
        }
        sleep(2) ;
        RIL_requestTimedCallback(initializeCallback, NULL, &TIMEVAL_0);

        // Give initializeCallback a chance to dispatched, since
        // we don't presently have a cancellation mechanism
        sleep(1);
        waitForClose();
        LOGI("Re-opening after close");
    }
}

#ifdef RIL_SHLIB

pthread_t s_tid_mainloop;

const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc, char **argv)
{
    int ret;
    int fd = -1;
    int opt;
    pthread_attr_t attr;

    s_rilenv = env;

    while ( -1 != (opt = getopt(argc, argv, "p:d:s:"))) {
        switch (opt) {
            case 'p':
                s_port = atoi(optarg);
                if (s_port == 0) {
                    usage(argv[0]);
                    return NULL;
                }
                LOGI("Opening loopback port %d\n", s_port);
            break;

            case 'd':
                s_service_device = optarg;
                LOGI("Opening tty device: %s\n", s_service_device);
            break;

            case 's':
                s_service_device   = optarg;
                s_device_socket = 1;
                LOGI("Opening socket %s\n", s_service_device);
            break;

             case 'm':
                DebugMask = atoi(optarg);
                LOGI("Opening DebugMask = %d\n", DebugMask);
            break;

            case 'l':
                DebugMask = atoi(optarg);
                LOGI("Opening DebugLevel =  %d\n", DebugLevel);
            break;
            
            default:
                usage(argv[0]);
                return NULL;
        }
    }

    set_debug_prop(DebugMask,DebugLevel);
	
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&s_tid_mainloop, &attr, mainLoop, NULL);

    return &s_callbacks;
}
#else /* RIL_SHLIB */
int main (int argc, char **argv)
{
    int ret;
    int fd = -1;
    int opt;

    while ( -1 != (opt = getopt(argc, argv, "p:d:"))) {
        switch (opt) {
            case 'p':
                s_port = atoi(optarg);
                if (s_port == 0) {
                    usage(argv[0]);
                }
                LOGI("Opening loopback port %d\n", s_port);
            break;

            case 'd':
                s_service_device = optarg;
            break;

            case 's':
                s_service_device   = optarg;
                s_device_socket = 1;
                LOGI("Opening socket %s\n", s_service_device);
            break;

            default:
                usage(argv[0]);
        }
    }

    if (s_port < 0 && s_service_device == NULL) {
        usage(argv[0]);
    }
    
    LOGI("Opening tty device %s\n", s_service_device);
    
    RIL_register(&s_callbacks);

    mainLoop(NULL);

    return 0;
}

#endif /* RIL_SHLIB */
