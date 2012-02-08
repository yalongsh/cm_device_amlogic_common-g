#include <stdio.h>
#include <telephony/ril.h>
#include <pthread.h>
#include <cutils/properties.h>

#include <assert.h>
#include "aml-ril.h"
#include "aml-ril-network.h"
#include "aml-ril-sim.h"
#include "vendor.h"

#define LOG_TAG "AML_RIL"
#include <utils/Log.h>
#include "debug.h"
#include "ril-compatible.h"

#define REPOLL_OPERATOR_SELECTED 30     /* 30 * 2 = 1M = ok? */
static const struct timeval TIMEVAL_OPERATOR_SELECT_POLL = { 2, 0 };

static int net_mode = 0;
static  int sys_mode = 8;
static int cgreg_state = -1 ;

RIL_RadioState sState = RADIO_STATE_UNAVAILABLE;
pthread_mutex_t s_state_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t s_state_cond = PTHREAD_COND_INITIALIZER;


//------------------------private function -----------------------------

static int get_reg_stat(int registered,int  roaming)
{
	if(registered == 0){
		return 0; //0 - Not registered, MT is not currently searching a new operator to register
	}else if(registered == 4){
		return 2; // 2 - Not registered, but MT is currently searching  a new operator to register
	}else if(roaming == 1){
	        return 5;// Registered, roaming
	 }else{
	 	return 1;// Registered, home network
	 }
}

/*

  0 - Unknown, 1 - GPRS, 2 - EDGE, 3 - UMTS,
 *                                  4 - IS95A, 5 - IS95B, 6 - 1xRTT,
 *                                  7 - EvDo Rev. 0, 8 - EvDo Rev. A,
 *                                  9 - HSDPA, 10 - HSUPA, 11 - HSPA,
 *                                  12 - EVDO Rev B


 <sys_mode>
0   no service
1   AMPS
2   CDMA
3   GSM/GPRS
4   HDR\u6a21\u5f0f
5   WCDMA\u6a21\u5f0f
6   GPS\u6a21\u5f0f
7   GSM/WCDMA
8   CDMA/HDR HYBRID

*/

static int get_sys_mode( int sysmode)
{
	int map[] = {0,8,6,1,8,3,3,3,8};
	if( sysmode >= 9 || sysmode < 0){
		return 0;
	}
	return map[sysmode];
}

/** do post-AT+CFUN=1 initialization */
static void onRadioPowerOn()
{
    pollSIMState(NULL);
}

static int is_cdma_net( int sysmode){
	int ret = 0;
	switch(sysmode){
		case 0:
		case 1:
		case 6:
			ret = 1;
			break;
		case 8:
		case 3:
			ret = 0;
			break;
	}
	return ret;
}

//#ifdef ARCH_CDMA
//-----------------------------------------------------------------------
/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC
 *
 * Specify that the network should be selected automatically.
 ok
*/
void requestSetNetworkSelectionAutomatic_cdma(void *data, size_t datalen,
                                         RIL_Token t)
{
    //at_send_command("AT+COPS=0", NULL);//
    net_mode  = 0;
   RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL,0);
    return;
}



/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL
 *
 * Manually select a specified network.
 *
 * The radio baseband/RIL implementation is expected to fall back to 
 * automatic selection mode if the manually selected network should go
 * out of range in the future.
 ok
 */
void requestSetNetworkSelectionManual_cdma(void *data, size_t datalen,
                                      RIL_Token t)
{

      net_mode  = 1;
      RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL,0);
      return;
    
    /* 
     * AT+COPS=[<mode>[,<format>[,<oper>[,<AcT>]]]]
     *    <mode>   = 4 = Manual (<oper> field shall be present and AcT optionally) with fallback to automatic if manual fails.
     *    <format> = 2 = Numeric <oper>, the number has structure:
     *                   (country code digit 3)(country code digit 2)(country code digit 1)
     *                   (network code digit 2)(network code digit 1) 
     */
    
    int err = 0;
    char *cmd = NULL;
    ATResponse *atresponse = NULL;
    const char *mccMnc = (const char *) data;

    /* Check inparameter. */
    if (mccMnc == NULL) {
        goto error;
    }
    /* Build and send command. */
    asprintf(&cmd, "AT+COPS=4,2,\"%s\"", mccMnc);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
finally:

    at_response_free(atresponse);

    if (cmd != NULL)
        free(cmd);

    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
 *
 * Requests to set the preferred network type for searching and registering
 * (CS/PS domain, RAT, and operation mode).
 ok
 */
void requestSetPreferredNetworkType_cdma(void *data, size_t datalen,
                                    RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err = 0;
    int rat;
    int arg;
    char *cmd = NULL;
    RIL_Errno errno = RIL_E_GENERIC_FAILURE;

    rat = ((int *) data)[0];
	if(sState == RADIO_STATE_OFF){
		RIL_onRequestComplete(t,RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
		return;
	}
    switch (rat) {
        case 0:
	case 2:
	case 6:
        	arg = 4;   // EVDO
       		 break;
    	case 1:
	case 5:
        	arg = 2;	// CDMA
       		 break;
	case 3:
	case 4:
	case 7:
		arg = 8;	// CDMA/EVDO auto mode
		break;
	
    default:
          RIL_onRequestComplete(t,RIL_E_MODE_NOT_SUPPORTED, NULL, 0);
	  return;
    }

    asprintf(&cmd, "AT^PREFMODE=%d", arg);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0){
		errno = RIL_E_GENERIC_FAILURE;
        goto error;
	}
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    free(cmd);
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, errno, NULL, 0);
    goto finally;
}


/**
 * RIL_REQUEST_SIGNAL_STRENGTH
 *
 * Requests current signal strength and bit error rate.
 *
 * Must succeed if radio is on.
 */
 
void requestSignalStrength_cdma(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *p_response = NULL,*p_response_evdo=NULL;
    int err;
    char *line;
    int dbm=0,ecio=0;
    RIL_SignalStrength response={{0,0},{0,0},{0,0,0}};
   if( is_cdma_net(sys_mode )){
    err = at_send_command_singleline("AT+CSQ", "+CSQ:", &p_response);
    if (err < 0 || p_response == NULL ||  p_response->success == 0) {
    	dbg_err(DBG_NET,"AT+CSQ failed p_response=%p ",p_response);
        goto error;
    }

    if(p_response->p_intermediates == NULL){
        goto error;
    }

    line = p_response->p_intermediates->line;

     if(line == NULL){
        goto error;
    }

    err = at_tok_start(&line);
    if (err < 0) goto error;


    err = at_tok_nextint(&line, &dbm);
    if (err < 0) goto error;

    err = at_tok_nextint(&line, &ecio);
    if (err < 0) goto error;

    response.GW_SignalStrength.signalStrength = dbm;
    response.GW_SignalStrength.bitErrorRate = ecio;


    //evdo 65,75,90,105 . cdma 90,110,130,150
     response.CDMA_SignalStrength.dbm = switch_signal_strength(dbm,0);
     response.CDMA_SignalStrength.ecio = 90;

      response.EVDO_SignalStrength.dbm = switch_signal_strength(dbm,0);
      response.EVDO_SignalStrength.ecio =90;//ecio;
      response.EVDO_SignalStrength.signalNoiseRatio =8;
   }else{
    	err = at_send_command_singleline("AT^HDRCSQ", "^HDRCSQ:", &p_response_evdo);
   	 if (err < 0 || p_response_evdo == NULL ||  p_response_evdo->success == 0) {
    		dbg_err(DBG_NET,"AT^HDRCSQ failed p_response=%p ",p_response_evdo);
       		 goto error;
   	 }

   	 if(p_response_evdo->p_intermediates == NULL){
        	goto error;
   	 }

   	 line = p_response_evdo->p_intermediates->line;

     	if(line == NULL){
       		 goto error;
    	}

   	 err = at_tok_start(&line);
    	if (err < 0) goto error;


    	err = at_tok_nextint(&line, &dbm);
   	 if (err < 0) goto error;
    
    	 response.EVDO_SignalStrength.dbm = switch_signal_strength(dbm,1);
     	 response.EVDO_SignalStrength.ecio = ecio*10;//ecio;
   	 response.EVDO_SignalStrength.signalNoiseRatio =8;
   }
   
    RIL_onRequestComplete(t, RIL_E_SUCCESS,& response, sizeof(response));

    at_response_free(p_response);
    at_response_free(p_response_evdo);
    return;

error:
    dbg_err(DBG_NET,"requestSignalStrength must never return an error when radio is on");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(p_response);
    at_response_free(p_response_evdo);
}

//#endif

//#ifdef ARCH_GSM
/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC
 *
 * Specify that the network should be selected automatically.
*/
void requestSetNetworkSelectionAutomatic_gsm(void *data, size_t datalen,
                                         RIL_Token t)
{
    int err = 0;

    err = at_send_command("AT+COPS=0", NULL);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    return;
}

/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL
 *
 * Manually select a specified network.
 *
 * The radio baseband/RIL implementation is expected to fall back to 
 * automatic selection mode if the manually selected network should go
 * out of range in the future.
 */
void requestSetNetworkSelectionManual_gsm(void *data, size_t datalen,
                                      RIL_Token t)
{
    /* 
     * AT+COPS=[<mode>[,<format>[,<oper>[,<AcT>]]]]
     *    <mode>   = 4 = Manual (<oper> field shall be present and AcT optionally) with fallback to automatic if manual fails.
     *    <format> = 2 = Numeric <oper>, the number has structure:
     *                   (country code digit 3)(country code digit 2)(country code digit 1)
     *                   (network code digit 2)(network code digit 1) 
     */

    int err = 0;
    char *cmd = NULL;
    ATResponse *atresponse = NULL;
    const char *mccMnc = (const char *) data;

    /* Check inparameter. */
    if (mccMnc == NULL) {
        goto error;
    }
    /* Build and send command. */
    asprintf(&cmd, "AT+COPS=4,2,\"%s\"", mccMnc);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
finally:

    at_response_free(atresponse);

    if (cmd != NULL)
        free(cmd);

    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}


/**
 * RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
 *
 * Requests to set the preferred network type for searching and registering
 * (CS/PS domain, RAT, and operation mode)
 *
 * "data" is int *
 *
 * ((int *)data)[0] is == 0 for GSM/WCDMA (WCDMA preferred)
 * ((int *)data)[0] is == 1 for GSM only
 * ((int *)data)[0] is == 2 for WCDMA only
 * ((int *)data)[0] is == 3 for GSM/WCDMA (auto mode, according to PRL)
 * ((int *)data)[0] is == 4 for CDMA and EvDo (auto mode, according to PRL)
 * ((int *)data)[0] is == 5 for CDMA only
 * ((int *)data)[0] is == 6 for EvDo only
 * ((int *)data)[0] is == 7 for GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  GENERIC_FAILURE
 *  MODE_NOT_SUPPORTED
 */
 
void requestSetPreferredNetworkType_gsm(void *data, size_t datalen,
                                    RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err = 0;
    int rat;
    int arg;
    char *cmd = NULL;
    RIL_Errno errno = RIL_E_GENERIC_FAILURE;

    rat = ((int *) data)[0];
	if(sState == RADIO_STATE_OFF){
		RIL_onRequestComplete(t,RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
		return;
	}
	
    switch (rat) {
        case 0:
	case 3:
	case 4:
	case 7:
        	arg = 4;   // atuo
       	break;
    	case 1:
	case 5:
        	arg = 13;	// GSM
       	break;
	case 2:
	case 6:
		arg = 14;	// WCDMA
		break;
	
    default:
          RIL_onRequestComplete(t,RIL_E_MODE_NOT_SUPPORTED, NULL, 0);
	  return;
    }

    asprintf(&cmd, "AT+ZMDS=%d", arg);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0){
		errno = RIL_E_GENERIC_FAILURE;
        goto error;
	}
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    free(cmd);
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, errno, NULL, 0);
    goto finally;
}


void requestSignalStrength_gsm(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *p_response = NULL;
    int err;
    int response[2];
    char *line;

    err = at_send_command_singleline("AT+CSQ", "+CSQ:", &p_response);

    if (err < 0 || p_response->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        goto error;
    }

    line = p_response->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0) goto error;

    err = at_tok_nextint(&line, &(response[0]));
    if (err < 0) goto error;

    err = at_tok_nextint(&line, &(response[1]));
    if (err < 0) goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));

    at_response_free(p_response);
    return;

error:
    LOGE("requestSignalStrength must never return an error when radio is on");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(p_response);
}

//#endif

void requestSetNetworkSelectionAutomatic(void *data, size_t datalen,RIL_Token t){

	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){
		requestSetNetworkSelectionAutomatic_gsm(data,datalen,t);
	}else if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
		requestSetNetworkSelectionAutomatic_cdma(data,datalen,t);
	}else{
		LOGE("requestSetNetworkSelectionAutomatic unexpected arch type(%d)", s_arch);
	}                                         
}

void requestSetNetworkSelectionManual(void *data, size_t datalen,
                                      RIL_Token t){
	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){
		requestSetNetworkSelectionManual_gsm(data,datalen,t);
	}else if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
		requestSetNetworkSelectionManual_cdma(data,datalen,t);
	}else{
		LOGE("requestSetNetworkSelectionManual unexpected arch type(%d)", s_arch);
	}
}

void requestSetPreferredNetworkType(void *data, size_t datalen,
                                    RIL_Token t){
	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){
		requestSetPreferredNetworkType_gsm(data,datalen,t);
	}else if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
		requestSetPreferredNetworkType_cdma(data,datalen,t);
	}else{
		LOGE("requestSetPreferredNetworkType unexpected arch type(%d)", s_arch);
	}
}

void requestSignalStrength(void *data, size_t datalen, RIL_Token t){
	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){
		requestSignalStrength_gsm(data,datalen,t);
	}else if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
		requestSignalStrength_cdma(data,datalen,t);
	}else{
		LOGE("requestSignalStrength unexpected arch type(%d)", s_arch);
	}
}


/**
 * RIL_REQUEST_QUERY_AVAILABLE_NETWORKS
 *
 * Scans for available networks.
 ok
*/
void requestQueryAvailableNetworks(void *data, size_t datalen, RIL_Token t)
{
    /* 
     * AT+COPS=?
     *   +COPS: [list of supported (<stat>,long alphanumeric <oper>
     *           ,short alphanumeric <oper>,numeric <oper>[,<AcT>])s]
     *          [,,(list of supported <mode>s),(list of supported <format>s)]
     *
     *   <stat>
     *     0 = unknown
     *     1 = available
     *     2 = current
     *     3 = forbidden 
     */

    int err = 0;
    ATResponse *atresponse = NULL;
    const char *statusTable[] =
        { "unknown", "available", "current", "forbidden" };
    char **responseArray = NULL;
    char *p;
    int n = 0;
    int i = 0;

    err = at_send_command_multiline("AT+COPS=?", "+COPS:", &atresponse);
    if (err < 0 || 
        atresponse->success == 0 || 
        atresponse->p_intermediates == NULL)
        goto error;

    p = atresponse->p_intermediates->line;
    while (*p != '\0') {
        if (*p == '(')
            n++;
        p++;
    }

    /* Allocate array of strings, blocks of 4 strings. */
    responseArray = alloca(n * 4 * sizeof(char *));

    p = atresponse->p_intermediates->line;

    /* Loop and collect response information into the response array. */
    for (i = 0; i < n; i++) {
        int status = 0;
        char *line = NULL;
        char *s = NULL;
        char *longAlphaNumeric = NULL;
        char *shortAlphaNumeric = NULL;
        char *numeric = NULL;
        char *remaining = NULL;

        s = line = getFirstElementValue(p, "(", ")", &remaining);
        p = remaining;

        if (line == NULL) {
            LOGE("Null pointer while parsing COPS response. This should not happen.");
            break;
        }
        /* <stat> */
        err = at_tok_nextint(&line, &status);
        if (err < 0)
            goto error;

        /* long alphanumeric <oper> */
        err = at_tok_nextstr(&line, &longAlphaNumeric);
        if (err < 0)
            goto error;

        /* short alphanumeric <oper> */            
        err = at_tok_nextstr(&line, &shortAlphaNumeric);
        if (err < 0)
            goto error;

        /* numeric <oper> */
        err = at_tok_nextstr(&line, &numeric);
        if (err < 0)
            goto error;

        responseArray[i * 4 + 0] = alloca(strlen(longAlphaNumeric) + 1);
        strcpy(responseArray[i * 4 + 0], longAlphaNumeric);

        responseArray[i * 4 + 1] = alloca(strlen(shortAlphaNumeric) + 1);
        strcpy(responseArray[i * 4 + 1], shortAlphaNumeric);

        responseArray[i * 4 + 2] = alloca(strlen(numeric) + 1);
        strcpy(responseArray[i * 4 + 2], numeric);

        free(s);

        /* 
         * Check if modem returned an empty string, and fill it with MNC/MMC 
         * if that's the case.
         */
        if (responseArray[i * 4 + 0] && strlen(responseArray[i * 4 + 0]) == 0) {
            responseArray[i * 4 + 0] = alloca(strlen(responseArray[i * 4 + 2])
                                              + 1);
            strcpy(responseArray[i * 4 + 0], responseArray[i * 4 + 2]);
        }

        if (responseArray[i * 4 + 1] && strlen(responseArray[i * 4 + 1]) == 0) {
            responseArray[i * 4 + 1] = alloca(strlen(responseArray[i * 4 + 2])
                                              + 1);
            strcpy(responseArray[i * 4 + 1], responseArray[i * 4 + 2]);
        }

        responseArray[i * 4 + 3] = alloca(strlen(statusTable[status]) + 1);
        sprintf(responseArray[i * 4 + 3], "%s", statusTable[status]);
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseArray,
                          i * 4 * sizeof(char *));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}







/**
 * RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE
 *
 * Query current network selectin mode.
 ok
 */
void requestQueryNetworkSelectionMode(
                void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *p_response = NULL;
    int response = 0;
    char *line;

//#ifdef ARCH_CDMA
    if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
        RIL_onRequestComplete(t, RIL_E_SUCCESS,&net_mode, sizeof(int));
        return;
    }
// #endif
    
    err = at_send_command_singleline("AT+COPS?", "+COPS:", &p_response);

    if (err < 0 || p_response->success == 0) {
        goto error;
    }

    line = p_response->p_intermediates->line;

    err = at_tok_start(&line);

    if (err < 0) {
        goto error;
    }

    err = at_tok_nextint(&line, &response);

    if (err < 0) {
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));
    at_response_free(p_response);
    return;
error:
    at_response_free(p_response);
    LOGE("requestQueryNetworkSelectionMode must never return error when radio is on");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

int switch_signal_strength(int rssi,int evdo)
{
	int dbm;
	
	if(evdo){//evdo
		if(rssi >= 80 ) return 60;
		if(rssi == 60) return 75;
		if(rssi == 40) return 90;
		if(rssi == 20) return 105;
		return 125;
	}else{//cdma
		if(16<=rssi && rssi <= 31 ) return 75;
		if(8<=rssi && rssi < 16) return 85;
		if(4<=rssi && rssi< 8) return 95;
		if(1<=rssi && rssi < 4) return 100;
		return 125;
	}
}




/**
 * RIL_REQUEST_REGISTRATION_STATE
 *
 * Request current registration state
 *
 * "data" is NULL
 * "response" is a "char **"
 * ((const char **)response)[0] is registration state 0-6,
 *              0 - Not registered, MT is not currently searching
 *                  a new operator to register
 *              1 - Registered, home network
 *              2 - Not registered, but MT is currently searching
 *                  a new operator to register
 *              3 - Registration denied
 *              4 - Unknown
 *              5 - Registered, roaming
 *             10 - Same as 0, but indicates that emergency calls
 *                  are enabled.
 *             12 - Same as 2, but indicates that emergency calls
 *                  are enabled.
 *             13 - Same as 3, but indicates that emergency calls
 *                  are enabled.
 *             14 - Same as 4, but indicates that emergency calls
 *                  are enabled.
 *
 * ((const char **)response)[1] is LAC if registered on a GSM/WCDMA system or
 *                              NULL if not.Valid LAC are 0x0000 - 0xffff
 * ((const char **)response)[2] is CID if registered on a * GSM/WCDMA or
 *                              NULL if not.
 *                                 Valid CID are 0x00000000 - 0xffffffff
 *                                    In GSM, CID is Cell ID (see TS 27.007)
 *                                            in 16 bits
 *                                    In UMTS, CID is UMTS Cell Identity
 *                                             (see TS 25.331) in 28 bits
 * ((const char **)response)[3] indicates the available radio technology 0-7,
 *                                  0 - Unknown, 1 - GPRS, 2 - EDGE, 3 - UMTS,
 *                                  4 - IS95A, 5 - IS95B, 6 - 1xRTT,
 *                                  7 - EvDo Rev. 0, 8 - EvDo Rev. A,
 *                                  9 - HSDPA, 10 - HSUPA, 11 - HSPA,
 *                                  12 - EVDO Rev B
 * ((const char **)response)[4] is Base Station ID if registered on a CDMA
 *                              system or NULL if not.  Base Station ID in
 *                              decimal format
 * ((const char **)response)[5] is Base Station latitude if registered on a
 *                              CDMA system or NULL if not. Base Station
 *                              latitude is a decimal number as specified in
 *                              3GPP2 C.S0005-A v6.0. It is represented in
 *                              units of 0.25 seconds and ranges from -1296000
 *                              to 1296000, both values inclusive (corresponding
 *                              to a range of -90?to +90?.
 * ((const char **)response)[6] is Base Station longitude if registered on a
 *                              CDMA system or NULL if not. Base Station
 *                              longitude is a decimal number as specified in
 *                              3GPP2 C.S0005-A v6.0. It is represented in
 *                              units of 0.25 seconds and ranges from -2592000
 *                              to 2592000, both values inclusive (corresponding
 *                              to a range of -180?to +180?.
 * ((const char **)response)[7] is concurrent services support indicator if
 *                              registered on a CDMA system 0-1.
 *                                   0 - Concurrent services not supported,
 *                                   1 - Concurrent services supported
 * ((const char **)response)[8] is System ID if registered on a CDMA system or
 *                              NULL if not. Valid System ID are 0 - 32767
 * ((const char **)response)[9] is Network ID if registered on a CDMA system or
 *                              NULL if not. Valid System ID are 0 - 65535
 * ((const char **)response)[10] is the TSB-58 Roaming Indicator if registered
 *                               on a CDMA or EVDO system or NULL if not. Valid values
 *                               are 0-255.
 * ((const char **)response)[11] indicates whether the current system is in the
 *                               PRL if registered on a CDMA or EVDO system or NULL if
 *                               not. 0=not in the PRL, 1=in the PRL
 * ((const char **)response)[12] is the default Roaming Indicator from the PRL,
 *                               if registered on a CDMA or EVDO system or NULL if not.
 *                               Valid values are 0-255.
 * ((const char **)response)[13] if registration state is 3 (Registration
 *                               denied) this is an enumerated reason why
 *                               registration was denied.  See 3GPP TS 24.008,
 *                               10.5.3.6 and Annex G.
 *                                 0 - General
 *                                 1 - Authentication Failure
 *                                 2 - IMSI unknown in HLR
 *                                 3 - Illegal MS
 *                                 4 - Illegal ME
 *                                 5 - PLMN not allowed
 *                                 6 - Location area not allowed
 *                                 7 - Roaming not allowed
 *                                 8 - No Suitable Cells in this Location Area
 *                                 9 - Network failure
 *                                10 - Persistent location update reject
 *
 * Please note that registration state 4 ("unknown") is treated
 * as "out of service" in the Android telephony system
 *
 * Registration state 3 can be returned if Location Update Reject
 * (with cause 17 - Network Failure) is received repeatedly from the network,
 * to facilitate "managed roaming"
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */

//#ifdef ARCH_CDMA
void requestRegistrationState_cdma(void *data,
                                        size_t datalen, RIL_Token t)
{
    int err,i;
    char * responseStr[14];
    ATResponse *p_response = NULL;
    char *line, *p;
    int skip;
    int registered = 0;
    int roaming = 0;

	for( i=0; i<3; i++){
	    err = at_send_command_singleline("AT^SYSINFO", "^SYSINFO:", &p_response);

	    if (err != 0 || p_response->success == 0 ||p_response->p_intermediates ==NULL||p_response->p_intermediates->line==NULL)
            goto error;

	    line = p_response->p_intermediates->line;

		dbg_info(DBG_NET," registered state %s",line);

	    err = at_tok_start(&line);
	    if (err < 0) goto error;

	    err = at_tok_nextint(&line, &skip);
	    if (err < 0) goto error;
	    
	    err = at_tok_nextint(&line, &registered);
	   if (err < 0) goto error;
	   
	    err = at_tok_nextint(&line, &roaming);
	    if (err < 0) goto error;

	    err = at_tok_nextint(&line, &sys_mode);
	    if (err < 0) goto error;
		if(sys_mode != 0)
			break;
		else
			sleep(1);
	}
    
    dbg_info(DBG_NET," registered = %d,roaming =%d,sys_mode=%d",registered, roaming,sys_mode);
    
    registered =  get_reg_stat(registered,roaming);
    sys_mode = get_sys_mode(sys_mode);
    dbg_info(DBG_NET," registered = %d,roaming =%d,sys_mode=%d",registered, roaming,sys_mode);

    asprintf(&responseStr[0], "%d", registered);
    asprintf(&responseStr[1], "%x",0);
    asprintf(&responseStr[2], "%x", 0);
    asprintf(&responseStr[3], "%d",sys_mode);//radio technology 8 - EvDo Rev. A
    asprintf(&responseStr[4], "%d",0);//Base Station ID
    asprintf(&responseStr[5], "%d",0);//Base Station latitude
    asprintf(&responseStr[6], "%d",0);// Base Station longitude
    
    /* * ((const char **)response)[7] is concurrent services support indicator if
 *                              registered on a CDMA system 0-1.
 *                                   0 - Concurrent services not supported,
 *                                   1 - Concurrent services supported*/
 
    asprintf(&responseStr[7], "%d",1);
    
    asprintf(&responseStr[8], "%d",0);//System ID
    asprintf(&responseStr[9], "%d",0);//Network ID
    
    /* * ((const char **)response)[10] is the TSB-58 Roaming Indicator if registered
 *                               on a CDMA or EVDO system or NULL if not. Valid values
 *                               are 0-255.*/
 
    asprintf(&responseStr[10], "%d",0);
    
    /* * ((const char **)response)[11] indicates whether the current system is in the
 *                               PRL if registered on a CDMA or EVDO system or NULL if
 *                               not. 0=not in the PRL, 1=in the PRL*/
 
    asprintf(&responseStr[11], "%d",0);

    /* * ((const char **)response)[12] is the default Roaming Indicator from the PRL,
 *                               if registered on a CDMA or EVDO system or NULL if not.
 *                               Valid values are 0-255.*/

    
    asprintf(&responseStr[12], "%d",0);

    /* * ((const char **)response)[13] if registration state is 3 (Registration
 *                               denied) this is an enumerated reason why
 *                               registration was denied.  See 3GPP TS 24.008,
 *                               10.5.3.6 and Annex G.
 *                                 0 - General
 *                                 1 - Authentication Failure
 *                                 2 - IMSI unknown in HLR
 *                                 3 - Illegal MS
 *                                 4 - Illegal ME
 *                                 5 - PLMN not allowed
 *                                 6 - Location area not allowed
 *                                 7 - Roaming not allowed
 *                                 8 - No Suitable Cells in this Location Area
 *                                 9 - Network failure
 *                                10 - Persistent location update reject*/
 
    asprintf(&responseStr[13], "%d",0);
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr, 14*sizeof(char*));
    for(i=0;i<14;i++){
    	  free(responseStr[i]);
    }
     at_response_free(p_response);
    return;
error:
    dbg_err(DBG_NET,"requestRegistrationState must never return an error when radio is on");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(p_response);
}
/**
 * RIL_REQUEST_OPERATOR
 *
 * Request current operator ONS or EONS.
 */
void requestOperator_cdma(void *data, size_t datalen, RIL_Token t)
{
    int err;
    int i;
    int skip;
    ATLine *p_cur;
    char *response[3];
     
    memset(response, 0, sizeof(response));

    ATResponse *p_response = NULL;

    asprintf(&response[0], "China Telecom");
    asprintf(&response[1], "Telecom");
    asprintf(&response[2], "46003");

    property_set("gsm.sim.operator.alpha", "China Telecom");
    property_set("gsm.operator.iso-country", "cn");

    if(response[2] != NULL){
        snprintf(g_imsi, sizeof(g_imsi), "%s123456789", response[2]);
    }
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));
	free(response[0]);
	free(response[1]);
	free(response[2]);
    return ;
    
    err = at_send_command_multiline(
        "AT+COPS=3,0;+COPS?;+COPS=3,1;+COPS?;+COPS=3,2;+COPS?",
        "+COPS:", &p_response);

    /* we expect 3 lines here:
     * +COPS: 0,0,"T - Mobile"
     * +COPS: 0,1,"TMO"
     * +COPS: 0,2,"310170"
     */

    if (err != 0) goto error;

    for (i = 0, p_cur = p_response->p_intermediates
            ; p_cur != NULL
            ; p_cur = p_cur->p_next, i++
    ) {
        char *line = p_cur->line;

        err = at_tok_start(&line);
        if (err < 0) goto error;

        err = at_tok_nextint(&line, &skip);
        if (err < 0) goto error;

        // If we're unregistered, we may just get
        // a "+COPS: 0" response
        if (!at_tok_hasmore(&line)) {
            response[i] = NULL;
            continue;
        }

        err = at_tok_nextint(&line, &skip);
        if (err < 0) goto error;

        // a "+COPS: 0, n" response is also possible
        if (!at_tok_hasmore(&line)) {
            response[i] = NULL;
            continue;
        }

        err = at_tok_nextstr(&line, &(response[i]));
        if (err < 0) goto error;
    }

    if (i != 3) {
        /* expect 3 lines exactly */
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));
    at_response_free(p_response);

    return;
error:
    dbg_err(DBG_NET,"requestOperator must not return error when radio is on");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(p_response);
}

/**
 * RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE
 *
 * Query the preferred network type (CS/PS domain, RAT, and operation mode)
 * for searching and registering.

 * RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE
 *
 * Query the preferred network type (CS/PS domain, RAT, and operation mode)
 * for searching and registering
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)response)[0] is == 0 for GSM/WCDMA (WCDMA preferred)
 * ((int *)response)[0] is == 1 for GSM only
 * ((int *)response)[0] is == 2 for WCDMA only
 * ((int *)response)[0] is == 3 for GSM/WCDMA (auto mode, according to PRL)
 * ((int *)response)[0] is == 4 for CDMA and EvDo (auto mode, according to PRL)
 * ((int *)response)[0] is == 5 for CDMA only
 * ((int *)response)[0] is == 6 for EvDo only
 * ((int *)response)[0] is == 7 for GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL)
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 *
 * See also: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
 
 */
void requestGetPreferredNetworkType_cdma(void *data, size_t datalen,
                                    RIL_Token t)
{
    int err = 0;
    int response = 0;
    int pref_mode;
    char *line;
    ATResponse *atresponse;

    err = at_send_command_singleline("AT^PREFMODE?", "^ PREFMODE:", &atresponse);
    if (err < 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &pref_mode);
    if (err < 0)
        goto error;

    assert(pref_mode >= 0 && pref_mode < 7);

    switch (pref_mode) {
    case 2://cdma
        response = 5;
        break;
    case 4://evdo
        response = 6;
        break;
     case 8://auto
     	 response = 4;
        break;
    default:
        response = 0;
        break;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

//#endif


//#ifdef ARCH_GSM


static int _requestRegistrationState_gsm(void *data,int request, size_t datalen, RIL_Token t)
{
    int err;
    int response[4]={0,};
    char * responseStr[4]={NULL,};
    ATResponse *p_response = NULL;
    const char *cmd;
    const char *prefix;
    char *line, *p;
    int commas;
    int skip;
    int count = 3;
    int i = 0;
    int ret = -1 ;
    int cnt =0;
AGAIN:    
    if (request == RIL_REQUEST_REGISTRATION_STATE) {
        cmd = "AT+CREG?";
        prefix = "+CREG:";
    } else if (request == RIL_REQUEST_GPRS_REGISTRATION_STATE) {
        cmd = "AT+CGREG?";
        prefix = "+CGREG:";
    } else {
        assert(0);
        goto error;
    }
    
    err = at_send_command_singleline(cmd, prefix, &p_response);

    if (err != 0 ||p_response->success == 0 ||p_response->p_intermediates ==NULL||p_response->p_intermediates->line==NULL)
    {
        ret = -10 ;
        goto error;
    }
   
    line = p_response->p_intermediates->line;
   
    err = at_tok_start(&line);
    if (err < 0)
    {
        ret = -11;
        goto error;
    }
    /* Ok you have to be careful here
     * The solicited version of the CREG response is
     * +CREG: n, stat, [lac, cid]
     * and the unsolicited version is
     * +CREG: stat, [lac, cid]
     * The <n> parameter is basically "is unsolicited creg on?"
     * which it should always be
     *
     * Now we should normally get the solicited version here,
     * but the unsolicited version could have snuck in
     * so we have to handle both
     *
     * Also since the LAC and CID are only reported when registered,
     * we can have 1, 2, 3, or 4 arguments here
     *
     * finally, a +CGREG: answer may have a fifth value that corresponds
     * to the network type, as in;
     *
     *   +CGREG: n, stat [,lac, cid [,networkType]]
     */

    /* count number of commas */
    commas = 0;
    for (p = line ; *p != '\0' ;p++) {
        if (*p == ',') commas++;
    }
    ret = -1 ;
    
    switch (commas) {
        case 0: /* +CREG: <stat> */
            err = at_tok_nextint(&line, &response[0]);
            if (err < 0) goto error;
        break;

        case 1: /* +CREG: <n>, <stat> */
            err = at_tok_nextint(&line, &skip);
            if (err < 0) goto error;
            err = at_tok_nextint(&line, &response[0]);
            if (err < 0) goto error;
            response[1] = 1;
            response[2] = 1;
        break;

        case 2: /* +CREG: <stat>, <lac>, <cid> */
            err = at_tok_nextint(&line, &response[0]);
            if (err < 0) goto error;
            err = at_tok_nexthexint(&line, &response[1]);
            if (err < 0) goto error;
            err = at_tok_nexthexint(&line, &response[2]);
            if (err < 0) goto error;
        break;
        case 3: /* +CREG: <n>, <stat>, <lac>, <cid> */
            err = at_tok_nextint(&line, &skip);
            if (err < 0) goto error;
            err = at_tok_nextint(&line, &response[0]);
            if (err < 0) goto error;
            err = at_tok_nexthexint(&line, &response[1]);
            if (err < 0) goto error;
            err = at_tok_nexthexint(&line, &response[2]);
            if (err < 0) goto error;
        break;
        /* special case for CGREG, there is a fourth parameter
         * that is the network type (unknown/gprs/edge/umts)
         */
        case 4: /* +CGREG: <n>, <stat>, <lac>, <cid>, <networkType> */
            err = at_tok_nextint(&line, &skip);
            if (err < 0) goto error;
            err = at_tok_nextint(&line, &response[0]);
            if (err < 0) goto error;
            err = at_tok_nexthexint(&line, &response[1]);
            if (err < 0) goto error;
            err = at_tok_nexthexint(&line, &response[2]);
            if (err < 0) goto error;
            err = at_tok_nexthexint(&line, &response[3]);
            if (err < 0) goto error;
            count = 4;
        break;
        default:
            goto error;
    }

    LOGD("%s,%d , the responce of %s :stat is %d \n",__FUNCTION__,__LINE__,request == RIL_REQUEST_GPRS_REGISTRATION_STATE?"CGREG?":"CREG?",response[0] );
    if(response[0] != 1 && response[0] != 5){
        if(cgreg_state > 0){
            response[0] = cgreg_state ;
            response[1] = 1;
            response[2] = 1;
        }
    }
    cgreg_state = response[0] ;
   
    asprintf(&responseStr[0], "%d", response[0]);
    asprintf(&responseStr[1], "%x", response[1]);
    asprintf(&responseStr[2], "%x", response[2]);
    asprintf(&responseStr[3], "%d", 3);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr, 4*sizeof(char*));
    at_response_free(p_response);
    
    for(i=0;i<4;i++){
    	if(responseStr[i]) free(responseStr[i]);
    }

    return 0;
error:
    LOGE("%s,%s must never return an error when radio is on",__FUNCTION__,request == RIL_REQUEST_GPRS_REGISTRATION_STATE?"CGREG?":"CREG?");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(p_response);
     for(i=0;i<4;i++){
    	if(responseStr[i]) free(responseStr[i]);
    }

    cgreg_state = -1 ;
     
     return ret ;
}


void requestRegistrationState_gsm(void *data, size_t datalen, RIL_Token t)
{
	int wait = 20,ret = -1;
	while(wait-- && ret == -1){
		ret = _requestRegistrationState_gsm(data,RIL_REQUEST_REGISTRATION_STATE,datalen,t);
		LOGD("wait = %d",wait);
		if(ret ) 
                sleep(1);
	}
}

void requestGPRSRegistrationState(void *data, size_t datalen, RIL_Token t)
{	
	int wait = 20,ret = -1;
	while(wait-- && ret == -1){
		ret = _requestRegistrationState_gsm(data,RIL_REQUEST_GPRS_REGISTRATION_STATE,datalen,t);
		LOGD("wait = %d",wait);
		if(ret ) 
                sleep(1);
	}
}

void requestOperator_gsm(void *data, size_t datalen, RIL_Token t)
{
    int err;
    int i;
    int skip;
    ATLine *p_cur;
    char *response[4] = {NULL ,NULL, NULL,NULL};

    ATResponse *p_response = NULL;
    
    int cnt =0;
    int err_cnt = 0;

    int cpin_ret = -11 ;
    cpin_ret = getSIMStatus(1);
    LOGD("ret of getSIMStatus is %d !!!!!!!!!!!!!!!", cpin_ret);

AGAIN:
    err = at_send_command_multiline(
        "AT+COPS=3,0;+COPS?;+COPS=3,1;+COPS?;+COPS=3,2;+COPS?",
        "+COPS:", &p_response);

    /* we expect 3 lines here:
     * +COPS: 0,0,"T - Mobile"
     * +COPS: 0,1,"TMO"
     * +COPS: 0,2,"310170"
     */

    if (err != 0) goto error;

    for (i = 0, p_cur = p_response->p_intermediates
            ; p_cur != NULL
            ; p_cur = p_cur->p_next, i++
    ) {
        char *line = p_cur->line;

        err = at_tok_start(&line);
        if (err < 0) goto error;

        err = at_tok_nextint(&line, &skip);
        if (err < 0) goto error;

        // If we're unregistered, we may just get
        // a "+COPS: 0" response
        if (!at_tok_hasmore(&line)) {
            response[i] = NULL;
            continue;
        }

        err = at_tok_nextint(&line, &skip);
        if (err < 0) goto error;

        // a "+COPS: 0, n" response is also possible
        if (!at_tok_hasmore(&line)) {
            response[i] = NULL;
            continue;
        }

        err = at_tok_nextstr(&line, &(response[i]));
        if (err < 0) goto error;
    }

    if (i != 3) {
        /* expect 3 lines exactly */
        goto error;
    }

	for(i=0;i<3;i++)
	{
		LOGD("requestOperator_gsm return response[%d]:%s \n",i,response[i]);
	}

	if(response[0] != NULL )
	{
		if( 25<strlen(response[0]) )
		{
                    i =-1 ;
			response[0] = NULL ;
			response[1] = NULL ;
			
			if(0==strcmp(response[2],"46001") )
			{		
				asprintf(&response[0], "China Unicom");
			    	asprintf(&response[1], "China Unicom");
			}
			else  if(0==strcmp(response[2],"46000") )
			{	
				asprintf(&response[0], "China Mobile");
			    	asprintf(&response[1], "China Mobile");
			}
		}
	}

    if(response[2] != NULL){
        snprintf(g_imsi, sizeof(g_imsi), "%s123456789", response[2]);
    }
    response[3] = "\0" ;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));
    at_response_free(p_response);
    p_response = NULL ;
    if(i == -1){
        if(response[0] != NULL)
            free(response[0]) ;
        if(response[1] != NULL)
            free(response[1]) ;
    }

    if(response[2] == NULL && cnt++ < 25){
        LOGD("requestOperator_gsm repeat times:%d !!!!!!!!!!!!!!",cnt+1);
        sleep(2);
        goto AGAIN;
    }

    return;
error:
    LOGE("requestOperator must not return error when radio is on");
    at_response_free(p_response);
    p_response = NULL ;
    if( (SIM_PIN != cpin_ret)  && (err_cnt++ < 4) ){
        LOGD("requestOperator_gsm err,and repeat times:%d @@@@@@@@!!!!!!!!!!!!!!",err_cnt+1);
        sleep(3) ;
        goto AGAIN ;
    }
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    
}

/**
 * RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE
 *
 * Query the preferred network type (CS/PS domain, RAT, and operation mode)
 * for searching and registering.
 */
void requestGetPreferredNetworkType_gsm(void *data, size_t datalen,
                                    RIL_Token t)
{
    int err = 0;
    int response = 0;
    int cfun;
    char *line;
    ATResponse *atresponse=NULL;

    /*err = at_send_command_singleline("AT+CFUN?", "+CFUN:", &atresponse);
    if (err < 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &cfun);
    if (err < 0)
        goto error;

    assert(cfun >= 0 && cfun < 7);

    switch (cfun) {
    case 5:
        response = 1;
        break;
    case 6:
        response = 2;
        break;
    default:
        response = 0;
        break;
    }
*/
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

//#endif

void requestRegistrationState(void *data, size_t datalen, RIL_Token t){
	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){
		requestRegistrationState_gsm(data,datalen,t);
	}else if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
		requestRegistrationState_cdma(data,datalen,t);
	}else{
		LOGE("requestRegistrationState unexpected arch type(%d)", s_arch);
	}
}

void requestOperator(void *data, size_t datalen, RIL_Token t){
	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){
		requestOperator_gsm(data,datalen,t);
	}else if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
		requestOperator_cdma(data,datalen,t);
	}else{
		LOGE("requestOperator unexpected arch type(%d)", s_arch);
	}
}

void requestGetPreferredNetworkType(void *data, size_t datalen,RIL_Token t){
	if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){
		requestGetPreferredNetworkType_gsm(data,datalen,t);
	}else if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch){
		requestGetPreferredNetworkType_cdma(data,datalen,t);
	}else{
		LOGE("requestGetPreferredNetworkType unexpected arch type(%d)", s_arch);
	}
}

void requestSetFacilityLock(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    char *facility_string = NULL;
    int facility_mode = -1;
    char *facility_mode_str = NULL;
    char *facility_password = NULL;
    char *facility_class = NULL;
    int num_retries = -1;

    assert(datalen >= (4 * sizeof(char **)));

    facility_string = ((char **) data)[0];
    facility_mode_str = ((char **) data)[1];
    facility_password = ((char **) data)[2];
    facility_class = ((char **) data)[3];

    assert(*facility_mode_str == '0' || *facility_mode_str == '1');
    facility_mode = atoi(facility_mode_str);


//    asprintf(&cmd, "AT+CLCK=\"%s\",%d,\"%s\",%s", facility_string,
//             facility_mode, facility_password, facility_class);
    asprintf(&cmd, "AT+CLCK=\"%s\",%d,\"%s\"", facility_string,
             facility_mode, facility_password);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));
    at_response_free(atresponse);
    return;

error:
    LOGE("requestSetFacilityLock failed !!!!!!!!!!!!!!!!!!!");
    at_response_free(atresponse);
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_QUERY_FACILITY_LOCK
 *
 * Query the status of a facility lock state.
 */
void requestQueryFacilityLock(void *data, size_t datalen, RIL_Token t)
{
    int err, rat, response;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    char *line = NULL;
    char *facility_string = NULL;
    char *facility_password = NULL;
    char *facility_class = NULL;

    assert(datalen >= (3 * sizeof(char **)));

    facility_string = ((char **) data)[0];
    facility_password = ((char **) data)[1];
    facility_class = ((char **) data)[2];

    //asprintf(&cmd, "AT+CLCK=\"%s\",2,\"%s\",%s", facility_string,
    //         facility_password, facility_class);

	asprintf(&cmd, "AT+CLCK=\"%s\",2", facility_string);
		
    LOGD("requestQueryFacilityLock cmd:%s.....................\n",cmd);
	
    err = at_send_command_singleline(cmd, "+CLCK:", &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0) {
        goto error;
    }

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);

    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &response);

    if (err < 0)
        goto error;
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));

    at_response_free(atresponse);
    LOGE("requestQueryFacilityLock finish.....................\n");
    return;

error:
   
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(atresponse);
    LOGE("requestQueryFacilityLock error.....................\n");
    return ;
}


/**
 * RIL_REQUEST_CDMA_SUBSCRIPTION
 *
 * Request the device MDN / H_SID / H_NID.
 *
 * The request is only allowed when CDMA subscription is available.  When CDMA
 * subscription is changed, application layer should re-issue the request to
 * update the subscription information.
 *
 * If a NULL value is returned for any of the device id, it means that error
 * accessing the device.
 *
 * "response" is const char **
 * ((const char **)response)[0] is MDN if CDMA subscription is available
 * ((const char **)response)[1] is a comma separated list of H_SID (Home SID) if
 *                              CDMA subscription is available, in decimal format
 * ((const char **)response)[2] is a comma separated list of H_NID (Home NID) if
 *                              CDMA subscription is available, in decimal format
 * ((const char **)response)[3] is MIN (10 digits, MIN2+MIN1) if CDMA subscription is available
 * ((const char **)response)[4] is PRL version if CDMA subscription is available
 *
 * Valid errors:
 *  SUCCESS
 *  RIL_E_SUBSCRIPTION_NOT_AVAILABLE
 */
void requestCDMASubScription(void *data, size_t datalen, RIL_Token t)
{
	 char *response[5]={0,};
	 ATResponse *atresponse = NULL;
	 char * line = NULL;
   	 int err=0,i=0;
	 //MDN
	 
   	 err = at_send_command_singleline("AT^MDN", "^MDN:",&atresponse);

    if (err < 0 || atresponse->success == 0|| !atresponse->p_intermediates|| !atresponse->p_intermediates->line) {
        asprintf(&response[0], "111");
    } else {
        line = atresponse->p_intermediates->line;
    	err = at_tok_start(&line);
   	 if (err < 0) {
   	 	asprintf(&response[0], "111");
   	 } else{
        	asprintf(&response[0], "%s", line);
   	 }
    }

   at_response_free(atresponse);

   
    err = at_send_command_numeric("AT+CIMI", &atresponse);

    if (err < 0 || atresponse->success == 0|| !atresponse->p_intermediates|| !atresponse->p_intermediates->line) {
       asprintf(&response[3], "111");
    } else {
        	asprintf(&response[3], "%s",atresponse->p_intermediates->line+5);
    }
    at_response_free(atresponse);

       asprintf(&response[1], "22322");
       asprintf(&response[2], "23233");
       asprintf(&response[4], "1.0");
       
   RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));
   
finally:

    for(i=0;i<4;i++){
    	if(response[i]){
    		free(response[i]);
    	}
    }
    return;

error:
    RIL_onRequestComplete(t, RIL_E_SUBSCRIPTION_NOT_AVAILABLE, NULL, 0);
    goto finally;
}

void requestRadioPower(void *data, size_t datalen, RIL_Token t)
{
    int onOff;
	static int first = 1;
	
    int err=0;
    ATResponse *p_response = NULL;

    assert (datalen >= sizeof(int *));
    onOff = ((int *)data)[0];

    LOGD("requestRadioPower %s", onOff ? "on":"off");

 #ifdef VENDOR_PROWAVE
        LOGD("setModemSysInterface set %d > %s ", onOff,PROWAVE_FILE);
 	 setModemSysInterface(PROWAVE_FILE, onOff);
 #endif

    //some module don't surport AT+cfun=0
    if(isModemSpecial()>=0 ){
        LOGD("this modem don't surport at+cfun, onOff is %d",onOff);        
        if(!onOff && sState != RADIO_STATE_OFF){
            setRadioState(RADIO_STATE_OFF);
        }
        else if(sState == RADIO_STATE_OFF){
            if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch)
                setRadioState(RADIO_STATE_RUIM_NOT_READY);
	     else
		   setRadioState(RADIO_STATE_SIM_NOT_READY);
        }
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        return; 
    }
 
    if (onOff == 0 && sState != RADIO_STATE_OFF) {
		if(RIL_MODEM_ARCH_TYPE_GSM == s_arch){
			//if(first ==0)
                    {
				err = at_send_command("AT+CFUN=0", &p_response);
				if (err < 0 || p_response->success == 0) goto error;
			}
			first = 0 ;
		}else{
			err = at_send_command("AT+CFUN=0", &p_response);
			if (err < 0 || p_response->success == 0) goto error;
		}

        setRadioState(RADIO_STATE_OFF);
    } else if (onOff > 0 && sState == RADIO_STATE_OFF) {
      err = at_send_command("AT+CFUN=1", &p_response);
        if (err < 0|| p_response->success == 0) {
            // Some stacks return an error when there is no SIM,
            // but they really turn the RF portion on
            // So, if we get an error, let's check to see if it
            // turned on anyway

            if (isRadioOn() != 1) {
                LOGE("requestRadioPower fialed AT+CFUN=1 !!!!!!");
                goto error;
            }
        }
		if(RIL_MODEM_ARCH_TYPE_CDMA == s_arch)
			setRadioState(RADIO_STATE_RUIM_NOT_READY);
		else
			setRadioState(RADIO_STATE_SIM_NOT_READY);		
    }

    at_response_free(p_response);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;
error:
    LOGE("requestRadioPower err !!!!!!");
    at_response_free(p_response);
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_SET_LOCATION_UPDATES
 *
 * Enables/disables network state change notifications due to changes in
 * LAC and/or CID (basically, +CREG=2 vs. +CREG=1).  
 *
 * Note:  The RIL implementation should default to "updates enabled"
 * when the screen is on and "updates disabled" when the screen is off.
 *
 * See also: RIL_REQUEST_SCREEN_STATE, RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED.
 */
void requestSetLocationUpdates(void *data, size_t datalen, RIL_Token t)
{
    int enable = 0;
    int err = 0;
    char *cmd;
    ATResponse *atresponse = NULL;

    enable = ((int *) data)[0];
    assert(enable == 0 || enable == 1);

    asprintf(&cmd, "AT+CREG=%d", (enable == 0 ? 1 : 2));
    err = at_send_command(cmd, &atresponse);
    free(cmd);

    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}


void setRadioState(RIL_RadioState newState)
{
    RIL_RadioState oldState;

    pthread_mutex_lock(&s_state_mutex);

    oldState = sState;

    if (s_closed > 0) {
        // If we're closed, the only reasonable state is
        // RADIO_STATE_UNAVAILABLE
        // This is here because things on the main thread
        // may attempt to change the radio state after the closed
        // event happened in another thread
        newState = RADIO_STATE_UNAVAILABLE;
    }

    if (sState != newState || s_closed > 0) {
        sState = newState;

        pthread_cond_broadcast (&s_state_cond);
    }

    pthread_mutex_unlock(&s_state_mutex);


    /* do these outside of the mutex */
    if (sState != oldState) {
    	LOGD("RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED sState = %d,old = %d",sState,oldState);
        RIL_onUnsolicitedResponse (RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED,
                                    NULL, 0);

        /* FIXME onSimReady() and onRadioPowerOn() cannot be called
         * from the AT reader thread
         * Currently, this doesn't happen, but if that changes then these
         * will need to be dispatched on the request thread
         */
        if ( (RIL_MODEM_ARCH_TYPE_CDMA==s_arch && sState==RADIO_STATE_RUIM_READY) ||
                    (RIL_MODEM_ARCH_TYPE_GSM==s_arch && sState==RADIO_STATE_SIM_READY)) {
            onSIMReady();
        } else if ( (RIL_MODEM_ARCH_TYPE_CDMA==s_arch && sState==RADIO_STATE_RUIM_NOT_READY) ||
                    (RIL_MODEM_ARCH_TYPE_GSM==s_arch && sState==RADIO_STATE_SIM_NOT_READY)) {
            onRadioPowerOn();
        }
    }
}


/** returns 1 if on, 0 if off, and -1 on error */
int isRadioOn()
{
    ATResponse *p_response = NULL;
    int err;
    char *line;
    char ret;

    err = at_send_command_singleline("AT+CFUN?", "+CFUN:", &p_response);

    if (err < 0 || p_response->success == 0) {
        // assume radio is off
        goto error;
    }

    line = p_response->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0) goto error;

    err = at_tok_nextbool(&line, &ret);
    if (err < 0) goto error;

    at_response_free(p_response);

    return (int)ret;

error:

    at_response_free(p_response);
    return -1;
}
