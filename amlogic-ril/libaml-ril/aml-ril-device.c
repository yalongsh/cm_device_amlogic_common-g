#include <stdio.h>
#include <telephony/ril.h>
#include <pthread.h>
#include <string.h>

#include <assert.h>
#include "aml-ril.h"
#include "aml-ril-device.h"
#include "ril-compatible.h"

#define LOG_NDEBUG 0
#define LOG_TAG "AML_RIL"
#include <utils/Log.h>
#include "debug.h"

char g_imsi[16]={0};

#if 0
void requestGetIMSI(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command_numeric("AT+CIMI", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS,
                              atresponse->p_intermediates->line,
                              sizeof(char *));
    }
    at_response_free(atresponse);
    return;
}
#else 
void requestGetIMSI(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *p_response = NULL;
    char *imsi;
    char *line;
    char *response;
    char *part;
    int err;

    if(RIL_MODEM_ARCH_TYPE_GSM == s_arch) { 
        int loop = 0;
        int success = 0;
        /* We are looping here because the command fails on the first try.
        What needs to be done, is to trap the "+CME ERROR: 14" which means
        SIM BUSY and retry that. As a workaround for now, simply try, wait
        1 second, and try again, until a valid result is obtained. Usually only
        takes 2 tries.
        */
        while ( loop < 10) {
            err = at_send_command_numeric("AT+CIMI", &p_response);
            if (err < 0 || p_response->success == 0) {
                usleep(500*1000);
                loop++;
            }
            else {
                loop=10;
                success=1;
            }
        }

        if (success == 0)
            goto error;
        imsi = strdup(p_response->p_intermediates->line);
    }
    else {
	/*
		err = at_send_command_singleline("AT+COPS?", "+COPS:", &p_response);

		if (err < 0 || p_response->success == 0)
			goto error;
		line = p_response->p_intermediates->line;

		at_tok_start(&line);
		err = at_tok_nextstr(&line, &response);
		if (err < 0)
			goto error;
		err = at_tok_nextstr(&line, &response);
		if (err < 0)
			goto error;
		err = at_tok_nextstr(&line, &response);
		if (err < 0)
			goto error;

		part = strdup(response);

		at_response_free(p_response);

		err = at_send_command_singleline("AT+CNUM", "+CNUM:", &p_response);

		if (err < 0 || p_response->success == 0)
			goto error;
		line = p_response->p_intermediates->line;

		at_tok_start(&line);
		err = at_tok_nextstr(&line, &response);
		if (err < 0)
			goto error;
		err = at_tok_nextstr(&line, &response);
		if (err < 0)
			goto error;
		//FIXME make it work with the real IMSI: asprintf(&imsi, "%s%s", part, response); //Real opID
		asprintf(&imsi, "310260000000000"); //Fake opID

		free (part);
        */
		goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, imsi, sizeof(char *));
    free (imsi);
    at_response_free(p_response);
    return;

error:
    if(strlen(g_imsi)>5){
        imsi = g_imsi ;
    }
    else{
        LOGE("%s err !!!!!!!!!!!",__FUNCTION__);
        at_response_free(p_response);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return ;
    }
    LOGE("%s, %s, %d, get imsi failed, we make it :%s !!!!!!!!!!!!!!\n", __FILE__, __FUNCTION__, __LINE__,imsi); 
    //RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(p_response);
    
    RIL_onRequestComplete(t, RIL_E_SUCCESS, imsi, sizeof(char *));
}
#endif
/* RIL_REQUEST_DEVICE_IDENTITY
 *
 * RIL_REQUEST_DEVICE_IDENTITY
 *
 * Request the device ESN / MEID / IMEI / IMEISV.
 *
 * The request is always allowed and contains GSM and CDMA device identity;
 * it substitutes the deprecated requests RIL_REQUEST_GET_IMEI and
 * RIL_REQUEST_GET_IMEISV.
 *
 * If a NULL value is returned for any of the device id, it means that error
 * accessing the device.
 *
 * When CDMA subscription is changed the ESN/MEID may change.  The application
 * layer should re-issue the request to update the device identity in this case.
 *
 * "response" is const char **
 * ((const char **)response)[0] is IMEI if GSM subscription is available
 * ((const char **)response)[1] is IMEISV if GSM subscription is available
 * ((const char **)response)[2] is ESN if CDMA subscription is available
 * ((const char **)response)[3] is MEID if CDMA subscription is available
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
 
void requestDeviceIdentity_CDMA(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    char* response[4]={0};
    int err;
    int i=0;

    // ESN,IMEI
    err = at_send_command_numeric("AT+GSN", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    } else {
        asprintf(&response[2], "%s", atresponse->p_intermediates->line);
       // asprintf(&response[0], "%s", atresponse->p_intermediates->line);
        at_response_free(atresponse);
        atresponse = NULL;
    }


   //MEID 
    err = at_send_command_singleline("AT^MEID", "",&atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    } else {
        asprintf(&response[3], "%s", atresponse->p_intermediates->line);
        at_response_free(atresponse);
        atresponse = NULL;
    }
    
    // IMEISV 
   // asprintf(&response[1], "%d", RIL_IMEISV_VERSION);

        /* GSM not supported */
    response[0] = NULL;
    response[1] = NULL;
    RIL_onRequestComplete(t, RIL_E_SUCCESS,
                          &response,
                          4*sizeof(char *));
    

finally:
	for( i=0;i<4;i++){
		if(response[i])   free(response[i]);
	}
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/* Deprecated */
/**
 * RIL_REQUEST_GET_IMEI
 *
 * Get the device IMEI, including check digit.
*/

void requestGetIMEI_CDMA(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

 // ESN
    err = at_send_command_numeric("AT+GSN", &atresponse);

    if (err < 0 || atresponse->success == 0) {
         RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
          RIL_onRequestComplete(t, RIL_E_SUCCESS,
                              atresponse->p_intermediates->line,
                              sizeof(char *));
        at_response_free(atresponse);
        atresponse = NULL;
    }
    return ;
    //----------------------------------------
    err = at_send_command_singleline("AT^MEID", "",&atresponse);

    if (err < 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS,
                              atresponse->p_intermediates->line,
                              sizeof(char *));
    }
    at_response_free(atresponse);
    return;
}

void requestDeviceIdentity_GSM(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    char* response[4]={NULL,};
    int err,i=0;

    /* IMEI */ 
    err = at_send_command_numeric("AT+CGSN", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    } else {
        asprintf(&response[0], "%s", atresponse->p_intermediates->line);
    }

    /* IMEISV */
    asprintf(&response[1], "%02d", RIL_IMEISV_VERSION);

    /* CDMA not supported */
    response[2] = NULL;
    response[3] = NULL;

    RIL_onRequestComplete(t, RIL_E_SUCCESS,
                          &response,
                          sizeof(response));

   for(i=0;i<4;i++){
   	if(response[i]) free(response[i]);
   }

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/* Deprecated */
/**
 * RIL_REQUEST_GET_IMEI
 *
 * Get the device IMEI, including check digit.
*/
void requestGetIMEI_GSM(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command_numeric("AT+CGSN", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS,
                              atresponse->p_intermediates->line,
                              sizeof(char *));
    }
    at_response_free(atresponse);
    return;
}

void requestDeviceIdentity(void *data, size_t datalen, RIL_Token t){
	if( RIL_MODEM_ARCH_TYPE_GSM == s_arch ){
		requestDeviceIdentity_GSM(data, datalen, t);
	}
	else{
		requestDeviceIdentity_CDMA(data, datalen, t);
	}
}

void requestGetIMEI(void *data, size_t datalen, RIL_Token t){
	if( RIL_MODEM_ARCH_TYPE_GSM == s_arch ){
		requestGetIMEI_GSM(data,datalen,t);
	}
	else{
		requestGetIMEI_CDMA(data,datalen,t);
	}
}

/* Deprecated */
/**
 * RIL_REQUEST_GET_IMEISV
 *
 * Get the device IMEISV, which should be two decimal digits.
*/

void requestGetIMEISV(void *data, size_t datalen, RIL_Token t)
{
    char *response = NULL;

    asprintf(&response, "%02d", RIL_IMEISV_VERSION);

    RIL_onRequestComplete(t, RIL_E_SUCCESS,
                          response,
                          sizeof(char *));

    if (response)
        free(response);
}

/**
 * RIL_REQUEST_BASEBAND_VERSION
 *
 * Return string value indicating baseband version, eg
 * response from AT+CGMR.
*/

void requestBasebandVersion(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    char *line;

    err = at_send_command_singleline("AT+CGMR", "+CGMR:", &atresponse);

    if (err < 0 || 
        atresponse->success == 0 || 
        atresponse->p_intermediates == NULL) {
        goto error;
    }

    line = atresponse->p_intermediates->line;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, line+sizeof("+CGMR"), sizeof(char *));

finally:
    at_response_free(atresponse);
    return;

error:
    LOGE("Error in requestBasebandVersion()");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

int is_known_device()
{
	int err;
    ATResponse *atresponse = NULL;
    char *line=NULL,*find=NULL;
    int ret = 0;

    int i=0;
    for(i=0;i<3 && ret==0;i++){
    	err = at_send_command_singleline("AT+CGMI", "",&atresponse);
		
   	 	if (err < 0 || 
       		 atresponse->success == 0 || 
       		 atresponse->p_intermediates == NULL) {
       		 at_response_free(atresponse);
       		 atresponse = NULL;
       		 sleep(1);
        	continue;
    	}

		line = atresponse->p_intermediates->line;
		LOGE("is_available_device +CGMI(%s)",line);

		find = strcasestr(line,"QUALCOMM");
		if(find == NULL) find = strcasestr(line,"ZTE");
		if(find == NULL) find = strcasestr(line,"huawei");
		if(find == NULL) find = strcasestr(line,"Manufacturer");

		ret = (find != NULL) ? 1 : 0;
		at_response_free(atresponse);
		atresponse = NULL;
    }

    at_response_free(atresponse);
    return ret;
}
