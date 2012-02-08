#include <stdio.h>
#include <assert.h>
#include <telephony/ril.h>
#include "aml-ril.h"

#define LOG_TAG "AML_RIL"
#include <utils/Log.h>
#include "debug.h"

/**
 * RIL_REQUEST_QUERY_CLIP
 *
 * Queries the status of the CLIP supplementary service.
 *
 * (for MMI code "*#30#")
 ok
 */
void requestQueryClip(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
}

/**
 * RIL_REQUEST_CANCEL_USSD
 * 
 * Cancel the current USSD session if one exists.
 */
void requestCancelUSSD(void *data, size_t datalen, RIL_Token t)
{
	ATResponse *p_response;
    	int err;
	p_response = NULL;
      err = at_send_command_numeric("AT+CUSD=2", &p_response);
      if (err < 0 || p_response->success == 0) {
             RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
      } else {
                RIL_onRequestComplete(t, RIL_E_SUCCESS,
                    p_response->p_intermediates->line, sizeof(char *));
      }
      at_response_free(p_response);
}

/**
 * RIL_REQUEST_SEND_USSD
 *
 * Send a USSD message.
 *
 * See also: RIL_REQUEST_CANCEL_USSD, RIL_UNSOL_ON_USSD.
 */
void  requestSendUSSD(void *data, size_t datalen, RIL_Token t)
{
    const char *ussdRequest;

    ussdRequest = (char *)(data);


    RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);

// @@@ TODO

}


/**
 * RIL_UNSOL_SUPP_SVC_NOTIFICATION
 *
 * Reports supplementary service related notification from the network.
 */
void onSuppServiceNotification(const char *s, int type)
{
    RIL_SuppSvcNotification ssnResponse;
    char *line;
    char *tok;
    int err;

    line = tok = strdup(s);

    memset(&ssnResponse, 0, sizeof(ssnResponse));
    ssnResponse.notificationType = type;

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&tok, &ssnResponse.code);
    if (err < 0)
        goto error;

    if (ssnResponse.code == 16 || 
        (type == 0 && ssnResponse.code == 4) ||
        (type == 1 && ssnResponse.code == 1)) {
        err = at_tok_nextint(&tok, &ssnResponse.index);
        if (err < 0)
            goto error;
    }

    /* RIL_SuppSvcNotification has two more members that we won't
       get from the +CSSI/+CSSU. Where do we get them, if we ever do? */

    RIL_onUnsolicitedResponse(RIL_UNSOL_SUPP_SVC_NOTIFICATION,
                              &ssnResponse, sizeof(ssnResponse));

error:
    free(line);
}

/**
 * RIL_UNSOL_ON_USSD
 *
 * Called when a new USSD message is received.
 */
void onUSSDReceived(const char *s)
{
    char **response;
    char *line;
    int err = -1;
    int i = 0;
    int n = 0;

    line = alloca(strlen(s) + 1);
    strcpy(line, s);
    line[strlen(s)] = 0;

    response = alloca(2 * sizeof(char *));
    response[0] = NULL;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &i);
    if (err < 0)
        goto error;

    if (i < 0 || i > 5)
        goto error;

    response[0] = alloca(2);
    sprintf(response[0], "%d", i);

    n = 1;

    if (i < 2) {
        n = 2;

        err = at_tok_nextstr(&line, &response[1]);
        if (err < 0)
            goto error;
    }

    /* TODO: We ignore the <dcs> parameter, might need this some day. */

    RIL_onUnsolicitedResponse(RIL_UNSOL_ON_USSD, response,
                              n * sizeof(char *));

error:
    return;
}

/**  
 * RIL_REQUEST_GET_CLIR
 *
 * Gets current CLIR status.
 ok
 */
void requestGetCLIR(void *data, size_t datalen, RIL_Token t)
{
   RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
}

/**
 * RIL_REQUEST_SET_CLIR
 ok
 */
void requestSetCLIR(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
}

/**
 * RIL_REQUEST_QUERY_CALL_FORWARD_STATUS
 ok
 */
void requestQueryCallForwardStatus(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
}

/**
 * RIL_REQUEST_SET_CALL_FORWARD
 *
 * Configure call forward rule.
 ok
 */
void requestSetCallForward(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
}

/**
 * RIL_REQUEST_QUERY_CALL_WAITING
 *
 * Query current call waiting state.
 ok
 */
void requestQueryCallWaiting(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
}

/**
 * RIL_REQUEST_SET_CALL_WAITING
 *
 * Configure current call waiting state.
 ok
 */
void requestSetCallWaiting(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
}

/**
 * RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION
 *
 * Enables/disables supplementary service related notifications
 * from the network.
 *
 * Notifications are reported via RIL_UNSOL_SUPP_SVC_NOTIFICATION.
 *
 * See also: RIL_UNSOL_SUPP_SVC_NOTIFICATION.
 */
void requestSetSuppSvcNotification(void *data, size_t datalen, RIL_Token t)
{
    int err;
    int ssn = ((int *) data)[0];
    char *cmd;

    assert(ssn == 0 || ssn == 1);

    asprintf(&cmd, "AT+CSSN=%d,%d", ssn, ssn);

    err = at_send_command(cmd, NULL);
    free(cmd);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}
