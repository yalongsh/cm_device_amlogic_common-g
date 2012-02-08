#include <stdio.h>
#include <telephony/ril.h>
#include "aml-ril.h"

#define LOG_TAG "AML_RIL"
#include <utils/Log.h>
#include "debug.h"

static char s_outstanding_acknowledge = 0;

#define OUTSTANDING_SMS    0
#define OUTSTANDING_STATUS 1

/**
 * RIL_REQUEST_GSM_BROADCAST_GET_SMS_CONFIG
 ok
 */
void requestGSMGetBroadcastSMSConfig(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *cmd;
    char* line;
    RIL_GSM_BroadcastSmsConfigInfo* configInfo;

    err = at_send_command_singleline("AT+CSCB?", "+CSCB:", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    configInfo = malloc(sizeof(RIL_GSM_BroadcastSmsConfigInfo));
    memset(&configInfo, 0, sizeof(RIL_GSM_BroadcastSmsConfigInfo));

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /* Get the string that yields the service ids.
       We expect the form: "fromServiceId-toServiceId". */    
    err = at_tok_nextstr(&line, &line);
    if (err < 0)
        goto error;

    line = strsep(&line, "\"");
    if (line == NULL)
        goto error;

    err = at_tok_nextint(&line, &configInfo->fromServiceId);
    if (err < 0)
        goto error;

    line = strsep(&line, "-");
    if (line == NULL)
        goto error;

    err = at_tok_nextint(&line, &configInfo->toServiceId);
    if (err < 0)
        goto error;

    /* Get the string that yields the coding schemes.   
       We expect the form: "fromCodeScheme-toCodeScheme". */    
    err = at_tok_nextstr(&line, &line);
    if (err < 0)
        goto error;

    line = strsep(&line, "\"");

    if (line == NULL)
        goto error;

    err = at_tok_nextint(&line, &configInfo->fromCodeScheme);
    if (err < 0)
        goto error;

    line = strsep(&line, "-");
    if (line == NULL)
        goto error;

    err = at_tok_nextint(&line, &configInfo->toCodeScheme);
    if (err < 0)
        goto error;

    err = at_tok_nextbool(&line, (char*)&configInfo->selected);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &configInfo, sizeof(RIL_GSM_BroadcastSmsConfigInfo));

finally:
    free(configInfo);
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_GSM_BROADCAST_SET_SMS_CONFIG
 ok
 */
void requestGSMSetBroadcastSMSConfig(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *cmd;
    RIL_GSM_BroadcastSmsConfigInfo* configInfo = (RIL_GSM_BroadcastSmsConfigInfo*)data;

    if (!configInfo->selected)
        goto error;

    /* TODO Should this test be done or shall we just let the modem return error. */       
    if ((configInfo->toServiceId - configInfo->fromServiceId) > 10)
        goto error;
        
    asprintf(&cmd, "AT+CSCB=0,\"%d-%d\",\"%d-%d\"", configInfo->fromServiceId, configInfo->toServiceId, configInfo->fromCodeScheme, configInfo->toCodeScheme);

    err = at_send_command(cmd, &atresponse);

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

/**
 * RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION
 ok
 */
void requestGSMSMSBroadcastActivation(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *cmd;

    err = at_send_command("AT+CNMI=2,0,2,0,0", &atresponse); 

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

/**
 * RIL_REQUEST_SEND_SMS
 * 
 * Sends an SMS message.
 ok
*/
void requestSendSMS(void *data, size_t datalen, RIL_Token t)
{
    int err;
    const char *smsc;
    const char *pdu;
    int tpLayerLength;
    char *cmd1, *cmd2;
    RIL_SMS_Response response;
    ATResponse *p_response = NULL;

    smsc = ((const char **)data)[0];
    pdu = ((const char **)data)[1];

    tpLayerLength = strlen(pdu)/2;

    // "NULL for default SMSC"
    if (smsc == NULL) {
        smsc= "00";
    }

    asprintf(&cmd1, "AT+CMGS=%d", tpLayerLength);
    asprintf(&cmd2, "%s%s", smsc, pdu);

    err = at_send_command_sms(cmd1, cmd2, "+CMGS:", &p_response);

    if (err != 0 || p_response->success == 0) goto error;

    memset(&response, 0, sizeof(response));

    /* FIXME fill in messageRef and ackPDU */

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(response));
    at_response_free(p_response);

    return;
error:
    LOGE("requestSendSMS failed !!!!!!!!!!!!!!");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(p_response);
}


/**
 * RIL_REQUEST_SEND_SMS_EXPECT_MORE
 * 
 * Send an SMS message. Identical to RIL_REQUEST_SEND_SMS,
 * except that more messages are expected to be sent soon. If possible,
 * keep SMS relay protocol link open (eg TS 27.005 AT+CMMS command).
 ok
*/
void requestSendSMSExpectMore(void *data, size_t datalen, RIL_Token t)
{
    /* Throw the command on the channel and ignore any errors, since we
       need to send the SMS anyway and subsequent SMSes will be sent anyway. */
    at_send_command("AT+CMMS=1", NULL);

    requestSendSMS(data, datalen, t);
}

/**
 * RIL_REQUEST_SMS_ACKNOWLEDGE
 *
 * Acknowledge successful or failed receipt of SMS previously indicated
 * via RIL_UNSOL_RESPONSE_NEW_SMS .
 ok
*/
void requestSMSAcknowledge(void *data, size_t datalen, RIL_Token t)
{
    int ackSuccess;
    int err;

    ackSuccess = ((int *)data)[0];

    if (ackSuccess == 1) {
        err = at_send_command("AT+CNMA=1", NULL);
    } else if (ackSuccess == 0)  {
        err = at_send_command("AT+CNMA=2", NULL);
    } else {
        LOGE("unsupported arg to RIL_REQUEST_SMS_ACKNOWLEDGE\n");
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

}

/**
 * RIL_REQUEST_WRITE_SMS_TO_SIM
 *
 * Stores a SMS message to SIM memory.
 ok
*/
void requestWriteSmsToSim(void *data, size_t datalen, RIL_Token t)
{
    RIL_SMS_WriteArgs *p_args;
    char *cmd;
    int length;
    int err;
    ATResponse *p_response = NULL;

    p_args = (RIL_SMS_WriteArgs *)data;

    length = strlen(p_args->pdu)/2;
    asprintf(&cmd, "AT+CMGW=%d,%d", length, p_args->status);

    err = at_send_command_sms(cmd, p_args->pdu, "+CMGW:", &p_response);

    if (err != 0 || p_response->success == 0) goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    at_response_free(p_response);

    return;
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    at_response_free(p_response);
}




/**
 * RIL_REQUEST_DELETE_SMS_ON_SIM
 *
 * Deletes a SMS message from SIM memory.
 ok
*/
void requestDeleteSmsOnSim(void *data, size_t datalen, RIL_Token t)
{
       char * cmd;
	 int err;
       ATResponse * p_response = NULL;
       asprintf(&cmd, "AT+CMGD=%d", ((int *)data)[0]);
       err = at_send_command(cmd, &p_response);
       free(cmd);
       if (err < 0 || p_response->success == 0) {
                RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        } else {
                RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        }
        at_response_free(p_response);
}

/**
 * RIL_REQUEST_GET_SMSC_ADDRESS
 ok
 */
void requestGetSMSCAddress(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *line;
    char *response;

    err = at_send_command_singleline("AT+CSCA?", "+CSCA:", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&line, &response);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(char *));

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_SMSC_ADDRESS
 ok
 */
void requestSetSMSCAddress(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *cmd;
    const char *smsc = ((const char *)data);

    asprintf(&cmd, "AT+CSCA=\"%s\"", smsc);
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
