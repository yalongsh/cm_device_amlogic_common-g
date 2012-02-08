#include <stdio.h>
#include <telephony/ril.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/route.h>
#include <cutils/properties.h>
#include <pthread.h>
#include <sys/wait.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

#include "ril-compatible.h"
#include "aml-ril.h"
#include "aml-ril-config.h"
#include "mulevent.h"
#include "aml-ril-pdp.h"
#include "liteping.h"

#define LOG_TAG "AML_RIL"
#include <utils/Log.h>
#include "debug.h"

#define USE_32_SUBNET_ROUTE_WORKAROUND 1
#define PPP_UP         1
#define PPP_DOWN       0
#define PPPD_PID_PROP_NAME "net.gprs.pppd_pid"
#define PPPD_APN_USER  "ril.ppp.user"
#define PPPD_APN_PWD   "ril.ppp.pwd"
#define PPPD_APN_NAME  "ril.ppp.apn_name"

extern RIL_RadioState sState;

/* Last pdp fail cause, obtained by *ECAV. */
static int s_lastPdpFailCause = PDP_FAIL_ERROR_UNSPECIFIED;// PDP_FAIL_USER_AUTHENTICATION ;


static pthread_t s_tid_connect_dog;
static mulevent_t dog_event;
static sem_t dog_off_sem;
static sem_t dog_on_sem;

static pthread_t s_tid_pppd_manager;
static mulevent_t pppd_event;
static int redial = 0;

static char g_apn[128]={0,} ;
static char  username[128]={0,};
static char pwd[128]={0,};

static int pdp_setup_result;
static int pdp_deactive_result;

static sem_t pdp_setup_end;
static sem_t pdp_deactive_end;

static int request_connect = 0;
static int net_connect = 0;

static void dog_off(void);
static void dog_on(void);

void onATReaderClosed();

void requestOrSendPDPContextList(RIL_Token *t)
{
    ATResponse *p_response;
    ATLine *p_cur;
    int err;
    int n = 0;
    char *out;

    err = at_send_command_multiline ("AT+CGACT?", "+CGACT:", &p_response);
    if (err != 0 || p_response->success == 0) {
        if (t != NULL)
            RIL_onRequestComplete(*t, RIL_E_GENERIC_FAILURE, NULL, 0);
        else
            RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                      NULL, 0);
        return;
    }

    for (p_cur = p_response->p_intermediates; p_cur != NULL;
         p_cur = p_cur->p_next)
        n++;

    RIL_Data_Call_Response *responses =
        alloca(n * sizeof(RIL_Data_Call_Response));

    int i;
    for (i = 0; i < n; i++) {
        responses[i].cid = -1;
        responses[i].active = -1;
        responses[i].type = "";
        responses[i].apn = "";
        responses[i].address = "";
    }

    RIL_Data_Call_Response *response = responses;
    for (p_cur = p_response->p_intermediates; p_cur != NULL;
         p_cur = p_cur->p_next) {
        char *line = p_cur->line;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &response->cid);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &response->active);
        if (err < 0)
            goto error;

        response++;
    }

    at_response_free(p_response);

    err = at_send_command_multiline ("AT+CGDCONT?", "+CGDCONT:", &p_response);
    if (err != 0 || p_response->success == 0) {
        if (t != NULL)
            RIL_onRequestComplete(*t, RIL_E_GENERIC_FAILURE, NULL, 0);
        else
            RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                      NULL, 0);
        return;
    }

    for (p_cur = p_response->p_intermediates; p_cur != NULL;
         p_cur = p_cur->p_next) {
        char *line = p_cur->line;
        int cid;
        char *type;
        char *apn;
        char *address;


        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &cid);
        if (err < 0)
            goto error;

        for (i = 0; i < n; i++) {
            if (responses[i].cid == cid)
                break;
        }

        if (i >= n) {
            /* details for a context we didn't hear about in the last request */
            continue;
        }

        err = at_tok_nextstr(&line, &out);
        if (err < 0)
            goto error;

        responses[i].type = alloca(strlen(out) + 1);
        strcpy(responses[i].type, out);

        err = at_tok_nextstr(&line, &out);
        if (err < 0)
            goto error;

        responses[i].apn = alloca(strlen(out) + 1);
        strcpy(responses[i].apn, out);

        err = at_tok_nextstr(&line, &out);
        if (err < 0)
            goto error;

        responses[i].address = alloca(strlen(out) + 1);
        strcpy(responses[i].address, out);
    }

    at_response_free(p_response);

    if (t != NULL)
        RIL_onRequestComplete(*t, RIL_E_SUCCESS, responses,
                              n * sizeof(RIL_Data_Call_Response));
    else
        RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                  responses,
                                  n * sizeof(RIL_Data_Call_Response));

    return;

error:
    if (t != NULL)
        RIL_onRequestComplete(*t, RIL_E_GENERIC_FAILURE, NULL, 0);
    else
        RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                  NULL, 0);

    at_response_free(p_response);
}

/**
 * RIL_UNSOL_PDP_CONTEXT_LIST_CHANGED
 *
 * Indicate a PDP context state has changed, or a new context
 * has been activated or deactivated.
*
 * See also: RIL_REQUEST_PDP_CONTEXT_LIST
 */
void onPDPContextListChanged(void *param)
{
    requestOrSendPDPContextList(NULL);
}

/**
 * RIL_REQUEST_PDP_CONTEXT_LIST
 *
 * Queries the status of PDP contexts, returning for each
 * its CID, whether or not it is active, and its PDP type,
 * APN, and PDP adddress.
*/
void requestPDPContextList(void *data, size_t datalen, RIL_Token t)
{
    requestOrSendPDPContextList(&t);
}

/**
 * RIL_REQUEST_SETUP_DATA_CALL
 *
 * Configure and activate PDP context for default IP connection.
 *
 ok
 */
void requestSetupDefaultPDP(void *data, size_t datalen, RIL_Token t)
{
	setPPPScript(s_arch, NULL);
	
    const char *apn;
    int err=0;
    const char *response[2] = { "1", s_modem_device};
    
    apn = ((const char **)data)[2];

     if(((const char **)data)[3] && ((const char **)data)[4]){
   	 strncpy(username,((const char **)data)[3],127);
    	strncpy(pwd,((const char **)data)[4],127);
     }else{
   	strcpy(username,"card");
  	 strcpy(pwd,"card");
     }

    dbg_info(DBG_PDP,"requesting data connection to APN '%s',user:\%s pwd:%s", apn,username,pwd);

    if(apn!=NULL && apn[0]!=0){
        property_set(PPPD_APN_NAME, apn );
    }
    else{
        apn = "cmnet";
        property_set(PPPD_APN_NAME, apn );
    }
    
    strncpy(g_apn,apn, sizeof(g_apn));
    // Start data on PDP context 1

    request_connect = 1;
    sem_init(&pdp_setup_end,0,0);
     if(mulevent_set_event(&pppd_event,EVENT_PPPD_MANAGER_START) == 0){
		 sem_wait(&pdp_setup_end);
     	}
     
     if(pdp_setup_result == 0){
     		dbg_info(DBG_PDP,"SetupDefaultPDP SUCCESS.");
    		RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));
    		return;
     }
    
error:
	dbg_err(DBG_PDP,"SetupDefaultPDP failed.");
    	RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_DEACTIVATE_DEFAULT_PDP
 *
 * Deactivate PDP context created by RIL_REQUEST_SETUP_DEFAULT_PDP.
 *
 * See also: RIL_REQUEST_SETUP_DEFAULT_PDP.
 */
void requestDeactivateDefaultPDP(void *data, size_t datalen, RIL_Token t)
{
    	struct timespec ts;
    	ts.tv_sec=time(NULL)+STOP_PPPD_TIMEOUT;
    	ts.tv_nsec=0;
    	request_connect = 0;
    	 if(mulevent_set_event(&pppd_event,EVENT_PPPD_MANAGER_STOP)==0){
     	 	 sem_init(&pdp_deactive_end,0,0);
    		 sem_timedwait(&pdp_deactive_end,&ts);
    	 }
     
    if(pdp_deactive_result == 0){
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        return;
    }
error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_LAST_PDP_FAIL_CAUSE
 * 
 * Requests the failure cause code for the most recently failed PDP 
 * context activate.
 *
 * See also: RIL_REQUEST_LAST_CALL_FAIL_CAUSE.
 *  
 ok
 */
void requestLastPDPFailCause(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_lastPdpFailCause,sizeof(int));
}

//----------------------------pppd----------------------------------------------
static int query_ifc_info( const char *interface, in_addr_t *addr, in_addr_t *mask, unsigned *flags)
{  
  DIR  *dir_path = NULL;
  struct ifreq ifr;
  struct dirent *de;
  int query_sock = -1;
  int ret = -1;
   
  query_sock = socket(AF_INET, SOCK_DGRAM, 0);
   if(query_sock < 0){
       dbg_err(DBG_PDP,"AML_RIL: failed to create query interface socket");
   	goto result;
  }
  // open the sys/class/net
  dir_path = opendir(SYS_NET_PATH);
  if(dir_path == 0){
  	dbg_err(DBG_PDP,"failed to opendir %s",SYS_NET_PATH);
    	goto result;
  }

  while((de = readdir(dir_path))){
    if(strcmp(de->d_name, interface) == 0){
      memset(&ifr, 0x00, sizeof(struct ifreq));
      strncpy((char *)&(ifr.ifr_name), interface, IFNAMSIZ);
      ifr.ifr_name[IFNAMSIZ -1] = 0x00;
      /* if (addr != NULL) {
	if(ioctl(query_sock, SIOCGIFADDR, &ifr) < 0) {
	  *addr = 0;
	} else {
	  *addr = ((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr;
	}
      }
      
      if (mask != NULL) {
	if(ioctl(query_sock, SIOCGIFNETMASK, &ifr) < 0) {
	  *mask = 0;
	} else {
	  *mask = ((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr;
	}
      }*/
      if (flags != NULL) {
	if(ioctl(query_sock, SIOCGIFFLAGS, &ifr) < 0) {
	  *flags = 0;
	} else {
	  *flags = ifr.ifr_flags;
	}
      }
      ret = 0;
      goto result;
    }
  }  
  // interface information is not found
 result:
	 if(query_sock != -1){
    		close(query_sock);
    		query_sock = -1;
	 }
	 if(dir_path){
	 	closedir(dir_path);
	 	dir_path = NULL;
	 }
 
  return ret;
}


static int checkPPPConnection( int sTimeout, unsigned int flag)
{
  int     ret = -1;
  int count = sTimeout;
  unsigned         pppInterFlags;
  assert(count >= 0);
  do{
      		if(query_ifc_info(PPP_INTERFACE, NULL, NULL, &pppInterFlags) == 0){
			if(flag == (pppInterFlags & 0x01)){
	  			ret = 0;
	  			break;
			}
  		}
      		//sched_yield();
    		sleep(2);
  }while(--count > 0);
  if(ret !=0 ){
  	    dbg_err(DBG_PDP,"checkPPPConnection: interface[%s] information is not found", PPP_INTERFACE);
  }
  return ret;
}

static int start_pppd(char * user,char * pwd,int * pid,int  ms)
{
     static char last_user[128]={0,};
     static char last_pwd[128]={0,};
     char cmd[256]={0,};
     unsigned char interface;
     
    dbg_info(DBG_PDP,"start pppd...........................");

	 /*
	 char    *ppp_argv[16];
	if( RIL_MODEM_ARCH_TYPE_CDMA == s_arch ){
	  	ppp_argv = {"pppd",(char *)s_modem_device, "115200",\
	  	 	"crtscts", "nodetach", "connect",\
	  	 	"/system/bin/chat ABORT 'NO CARRIER' ABORT 'NODIALTONE' ABORT 'ERROR' ABORT 'NO ANSWER' ABORT 'BUSY' TIMEOUT 120 '' AT OK ATDT#777 CONNECT",\
	  	 	"ipcp-accept-local", "ipcp-accept-remote", \
	  	 	"defaultroute", "usepeerdns", "user", NULL, "password", NULL, NULL};

		at_send_command("AT+CRM=2", NULL);
	  	at_send_command("AT+ZPCNT=1", NULL);

	  	if(strcmp(last_user,user) || strcmp( last_pwd,pwd)){
			strncpy(last_user,user,128);
			strncpy(last_pwd,pwd,128);
			sprintf(cmd,"AT+ZSETP=%d,%s,%d,%s",strlen(user),user,strlen(pwd),pwd);
			dbg_info(DBG_PDP,"set user,pwd:%s",cmd);
			at_send_command(cmd,NULL);
		}
	}
	else{
	  	ppp_argv = {"pppd",(char *)s_modem_device, "115200",\
	  	 	"crtscts", "nodetach", "connect",\
	  	 	"/system/bin/chat  ABORT 'NODIALTONE' ABORT 'ERROR' ABORT 'NO ANSWER' ABORT 'BUSY' TIMEOUT 120 '' AT OK ATDT*99# CONNECT",\
	  	 	"ipcp-accept-local", "ipcp-accept-remote", \
	  	 	"defaultroute", "usepeerdns", "user", NULL, "password", NULL, NULL};
	}
 	
	assert(*pid == 0);

    ppp_argv[12] = user;
    ppp_argv[14] = pwd;
   
  	if( (temp_pid = fork()) < 0){
    		dbg_err(DBG_PDP,"Failed to fork the pppd process\n");
    		return -1;
  	} else if (temp_pid == 0){ // child
    		dbg_info(DBG_PDP,"child  process running.....  (apn user:%s , passwd:%s)", ppp_argv[12], ppp_argv[14]);
    		if(execve("/system/bin/pppd", ppp_argv, environ) < 0 ){
    		    dbg_err(DBG_PDP,"Failed to execute the pppd process %d\n",errno);
            return -1;
        }
  	}else {//father
    		*pid  = temp_pid ;
    } */
    if( RIL_MODEM_ARCH_TYPE_CDMA == s_arch ){
        at_send_command("AT+CRM=2", NULL);
        at_send_command("AT+ZPCNT=1", NULL);
        if(strcmp(last_user,user) || strcmp( last_pwd,pwd)){
            strncpy(last_user,user,sizeof(last_user));
            strncpy(last_pwd,pwd,sizeof(last_user));
            sprintf(cmd,"AT+ZSETP=%d,%s,%d,%s",strlen(user),user,strlen(pwd),pwd);
            dbg_info(DBG_PDP,"set user,pwd:%s",cmd);
            at_send_command(cmd,NULL);
        }
    }
    else{
        if(strlen(g_apn) != 0){
            sprintf(cmd, "AT+CGDCONT=1,\"IP\",\"%s\"", g_apn);
            at_send_command(cmd,NULL);
        }
    }
    
    dbg_info(DBG_PDP,"pppd start with  args  (apn user:%s , passwd:%s) \n", user, pwd);
    property_set(PPPD_APN_USER, user );
    property_set(PPPD_APN_PWD, pwd );
    redial++;
    
    if (property_set("ctl.start", "pppd_gprs") < 0) {
        dbg_err(DBG_PDP,"Failed to execute the pppd process %d\n",errno);
        return -1;
    }

    if(checkPPPConnection(ms, PPP_UP) != 0){
        dbg_err(DBG_PDP,"start_pppd:   failed to check PPP interface UP");
        return -1;
    }

    *pid = -1;

    dbg_info(DBG_PDP,"connect internet  success.");
    return 0;
}


static int is_pppd_alive(void)
{
    char value[PROPERTY_VALUE_MAX];
    int ret;    
    int pppd_pid = 0 ;

    property_get(PPPD_PID_PROP_NAME, value, "0");
    LOGD("getprop %s:%s !!!!!!!!!!!!!@@@@@@@@@@\n",PPPD_PID_PROP_NAME,value);
    pppd_pid = atoi(value);
    if(pppd_pid > 0)
    {
    	char tmpbuff[128]={0};
    	snprintf(tmpbuff,sizeof(tmpbuff),"/proc/%d",pppd_pid);
    	if(0>access(tmpbuff, X_OK)){
    		LOGD(" process[ pppd_pid:%d] is not existing ?? .............\n",pppd_pid);
    		return -1; 
    	}
    }
    else{
        return -1 ;
    }

    return pppd_pid ;
}

static void clear_pppd_id(void)
{
    char value[PROPERTY_VALUE_MAX];
    memset(value, 0, sizeof(value));
    value[0]='0' ;
    property_set(PPPD_PID_PROP_NAME, value);
}


static int stop_pppd(int  pid)
{    
	int ret;    
	int pppd_pid = 0 ;
	if(pid == 0) 
		return 0;
   
	dbg_info(DBG_PDP,"stop pppd...>>>>>>>> \n");

       pppd_pid =  is_pppd_alive() ;
	if(pppd_pid > 0)
	{
		LOGD(" is going to kill  pppd_pid:%d .............\n",pppd_pid);
		if(0==(ret=kill(pppd_pid,SIGINT)))
		{
			LOGD(" kill pppd_pid:%d successfuly.............\n",pppd_pid);
		}
	}
	else
	{
		LOGE(" getprop %s:%d is wrong???????\n", PPPD_PID_PROP_NAME, pppd_pid);

		LOGD(" ...so killall pppd !!!!!\n");
             if(0==access("/system/bin/busybox",F_OK))
                system("/system/bin/busybox killall pppd");
             else
                system("/system/xbin/busybox killall pppd");
	}

     if (RIL_MODEM_ARCH_TYPE_GSM == s_arch) {
        char *cmd =  "AT+CGACT=0,1" ;
        ATResponse *p_response = NULL;

        ret = at_send_command(cmd, &p_response);

        if (ret < 0 || p_response->success == 0) {
            at_response_free(p_response);
            goto EXIT;
        }
        at_response_free(p_response);
    } else {
        
    }
    
EXIT:	
	clear_pppd_id();

	redial-- ;
	
	return 0;
}

static int pause_pppd()
{
	return -1;
}


static int cont_pppd()
{
	return -1;
}

static void pppd_exit_handle(int sig)
{
  dbg_info(DBG_PDP,"get exit sig %d",sig);
  mulevent_set_event(&pppd_event,EVENT_PPPD_MANAGER_PPPD_EXIT);
}

static void *pppd_manager(void *arg)
{
	unsigned int sig=-1;
	int  stop = 0;
	int  status = 0;
    int  ppp_pid=0;

	dbg_info(DBG_PDP,"PPPD manager started .");
	signal(SIGCHLD, pppd_exit_handle);
	while(!stop){
		sig = mulevent_wait(&pppd_event);
		switch(sig){
			case EVENT_PPPD_MANAGER_DISCONNECT:
				dbg_info(DBG_PDP,"EVENT_PPPD_MANAGER_DISCONNECT");
				if(ppp_pid !=0 ){
					stop_pppd(ppp_pid);
					ppp_pid =0;
				}else{
				
					if(net_connect){
						 start_pppd(username,pwd,&ppp_pid,START_PPPD_TIMEOUT);
						 dbg_info(DBG_PDP,"pppd exit. restart pppd result = %d",pdp_setup_result);			
						dog_on();
					}
				}
				break;
				
			case EVENT_PPPD_MANAGER_PPPD_EXIT:
				  dbg_info(DBG_PDP,"EVENT_PPPD_MANAGER_PPPD_EXIT,ppp_pid=%d",ppp_pid);
				 //if(ppp_pid !=0 )
                            {
				 		ppp_pid = 0;
				 		if(net_connect==1 ){
				 			dog_off();
							 start_pppd(username,pwd,&ppp_pid,START_PPPD_TIMEOUT);
							 dbg_info(DBG_PDP,"pppd exit. restart pppd result = %d",pdp_setup_result);			
							dog_on();
				 		}else{
				 			sem_post(&pdp_deactive_end);
				 			dbg_info(DBG_PDP,"pppd exit . no restart pppd .");
				 		}
				  }
				break;
			
			case EVENT_PPPD_MANAGER_START:
				
				LOGD("EVENT_PPPD_MANAGER_START \n");
				
				if(ppp_pid == 0){
					pdp_setup_result = start_pppd(username,pwd,&ppp_pid,START_PPPD_TIMEOUT);
					if(pdp_setup_result == 0){
						net_connect = 1;
						dog_on();
					}
                                 else{
                                    if(is_pppd_alive()>0){
                                        ppp_pid = -1 ;
                                        stop_pppd(ppp_pid);
                                    }
                                 }
				}
				sem_post(&pdp_setup_end);
				break;
				
			case EVENT_PPPD_MANAGER_STOP:
				LOGD("EVENT_PPPD_MANAGER_STOP !!!!!!!! \n");
				dbg_info(DBG_PDP,"EVENT_PPPD_MANAGER_STOP");
				assert(ppp_pid != 0);

				dog_off();
				
				net_connect  = 0;

				if(ppp_pid != 0){
					pdp_deactive_result=stop_pppd(ppp_pid);
					ppp_pid = 0 ;
                                if(pdp_deactive_result == 0){
                                    sem_post(&pdp_deactive_end);
                                }
				}else{
					pdp_deactive_result = 0;
					sem_post(&pdp_deactive_end);
				}
				
				break;

			case EVENT_PPPD_MANAGER_PAUSE:
				dbg_info(DBG_PDP,"EVENT_PPPD_MANAGER_PAUSE");
				dog_off();
				pause_pppd();
				break;
				
			case EVENT_PPPD_MANAGER_CONT:
				dbg_info(DBG_PDP,"EVENT_PPPD_MANAGER_CONT");
				cont_pppd();
				dog_on();
				break;
				
			case EVENT_PPPD_MANAGER_EXIT:
				dbg_info(DBG_PDP,"EVENT_PPPD_MANAGER_EXIT");
				net_connect = 0;
				if(ppp_pid != 0 && \
					(pdp_deactive_result=stop_pppd(ppp_pid)==0)){
						//waitpid(ppp_pid, &status, WNOHANG);
						ppp_pid = 0;
				}else{
					pdp_deactive_result = 0;
				}
				stop = 1;
				break;
				
			default:
				dbg_err(DBG_PDP,"Undefine EVENT");
				assert(false);	
		}
	}
	signal(SIGCHLD, SIG_DFL);
	dbg_info(DBG_PDP,"PPPD manager EXIT .");
	
  	return 0;
}

static void dog_off(void)
{
	sem_init(&dog_off_sem,0,0);
	mulevent_set_event(&dog_event,EVENT_DOG_OFF);
	sem_wait(&dog_off_sem);
}

static void dog_on(void)
{
	sem_init(&dog_on_sem,0,0);
	mulevent_set_event(&dog_event,EVENT_DOG_ON);
	sem_wait(&dog_on_sem);
}

 void dog_ping(void)
{
	mulevent_set_event(&dog_event,EVENT_DOG_PING);
}

static int is_net_connet()
{

    return 0 ;

    
    int err;
    char * line = NULL;
    ATResponse *p_response = NULL;
    int on;
    int response[4]={0,};
    int skip ;
    int i = 0 ;


    if(RIL_MODEM_ARCH_TYPE_GSM == s_arch ){
        //err = at_send_command_singleline ("AT+ZPS?","+ZPS:", &p_response);
        err = at_send_command_singleline ("AT+CREG?","+CREG:", &p_response);
        if (err != 0 || p_response->success == 0 ||  p_response->p_intermediates == NULL) {
        	goto error;
        }
        line = p_response->p_intermediates->line;
        err = at_tok_start (&line);
        if (err < 0) 
            goto error;


        int  commas = 0;
        char *p = NULL ;
        for (p = line ; *p != '\0' ;p++) {
            if (*p == ',') commas++;
        }


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
            break;
            default:
                goto error;
        }

        if(response[0] != 1 && response[0] != 5)  // 1 is Local network is registered ,5 is Roaming network is registered !!
            goto error;
    }   
    else{
        /* this is CDMA  */
        for( i=0; i<2; i++)
        {
            err = at_send_command_singleline("AT^SYSINFO", "^SYSINFO:", &p_response);

            if (err != 0 || p_response->success == 0 ||p_response->p_intermediates ==NULL||p_response->p_intermediates->line==NULL)
                goto error;

            line = p_response->p_intermediates->line;

            //dbg_info(DBG_NET," registered state %s",line);

            err = at_tok_start(&line);
            if (err < 0) 
                goto error;

            err = at_tok_nextint(&line, &skip);
            if (err < 0) 
                goto error;

            err = at_tok_nextint(&line, &skip);
            if (err < 0) 
                goto error;

            err = at_tok_nextint(&line, &skip);
            if (err < 0)
                goto error;

            err = at_tok_nextint(&line, &skip);
            if (err < 0) 
                goto error;
            if(skip != 0)
                break;
            else
                sleep(1);
        }
    }
    
    //+CREG: 2,1, A80A, AA2AA0
/*    at_tok_nextint(&line,&on);
    at_tok_nextint(&line,&on);
    if (err < 0 || on == 0) {
    	dbg_err(DBG_PDP,"net disconnect on:%d",on);
        goto error;
    }
    */

    at_response_free(p_response);
    return 0;

error:
    if(p_response != NULL)
        at_response_free(p_response);
    
    return 1;
}

static void *connect_dog(void *arg)
{
	long  sig;
	int  stop = 0;
	int stat = DOG_STAT_STOP;
	int ping = 0;
	int disconnect = 0;
	char value[64]={0,};

	struct timespec ts;
       
	dbg_info(DBG_PDP,"dog started .");
	while(!stop){
		ts.tv_sec=time(NULL)+CONNECT_DOG_TIMEOUT;
       	ts.tv_nsec=0;
		sig = mulevent_wait_timeout(&dog_event,& ts);
		switch(sig){
			case EVENT_DOG_ON:
				dbg_info(DBG_PDP,"EVENT_DOG_ON");
				stat  = DOG_STAT_START;
				sem_post(&dog_on_sem);
				break;
			
			case EVENT_DOG_OFF:
				dbg_info(DBG_PDP,"EVENT_DOG_OFF");
				stat  = DOG_STAT_STOP;
				sem_post(&dog_off_sem);
				break;
				
			case EVENT_DOG_PING:
				dbg_info(DBG_PDP,"EVENT_DOG_PING");
				ping = 1;
				break;
					
			case EVENT_DOG_ETIMEDOUT:
				//dbg_info(DBG_PDP,"EVENT_DOG_ETIMEDOUT stat=%d,redial=%d,ping=%d",stat,redial,ping);
                           if(redial>=5 && stat==DOG_STAT_STOP){
                                onATReaderClosed();
				}
				if(stat == DOG_STAT_START){
                                /*
                      if(ping){
						ping = 0;
						disconnect = is_net_connet();
						if(disconnect == 0){
							disconnect = get_host_by_name("www.baidu.com");
						}
					}else{
						disconnect = is_net_connet();
					}
					if(disconnect){
						dbg_info(DBG_PDP,"dog: disconnect.");
						stat  = DOG_STAT_STOP;
						mulevent_set_event(&pppd_event,EVENT_PPPD_MANAGER_DISCONNECT);
					}*/
					
					if(0 > is_pppd_alive()){
                                    mulevent_set_event(&pppd_event,EVENT_PPPD_MANAGER_PPPD_EXIT) ;
                                    //onATReaderClosed();
                                }
				}	
				get_debug_prop(&DebugMask,&DebugLevel);
				break;
				
			 case EVENT_DOG_EXIT:
				dbg_info(DBG_PDP,"EVENT_DOG_EXIT");
				stop = 1;
				break;
				
			default:
				dbg_info(DBG_PDP,"Undefine EVENT");
				assert(false);	
		}
	}
	dbg_info(DBG_PDP,"dog  exit .");
  	return 0;
}
int pdp_handle_init()
{
	int ret;
	pthread_attr_t attr,attr2;

	mulevent_init(&pppd_event);
	sem_init(&pdp_setup_end,0,0);
	sem_init(&pdp_deactive_end,0,0);
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	mulevent_init(&dog_event);
	sem_init(&dog_on_sem,0,0);
	sem_init(&dog_off_sem,0,0);
	pthread_attr_init (&attr2);
	pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);

	ret = pthread_create(&s_tid_pppd_manager, &attr,pppd_manager, NULL);

	if (ret < 0) {
		dbg_info(DBG_PDP,"PPPD manager thread create failed!");
		pthread_attr_destroy(&attr);
		mulevent_destroy(&pppd_event);
		return -1;
	}

	ret = pthread_create(&s_tid_connect_dog, &attr2,connect_dog, NULL);

	if (ret < 0) {
		dbg_info(DBG_PDP,"screen  manager thread create failed!");
		mulevent_destroy(&dog_event);
		pthread_attr_destroy(&attr2);
		return -1;
	}

	pthread_attr_destroy(&attr);
	pthread_attr_destroy(&attr2);

	dbg_info(DBG_PDP,"PPPD manager thread create success!");

	return 0;
}

void pdp_handle_uninit()
{
	//pppd manager
	
	if(s_tid_pppd_manager ){
	
		mulevent_set_event(&pppd_event,EVENT_PPPD_MANAGER_EXIT);
		pthread_join(s_tid_pppd_manager,NULL);
		mulevent_destroy(&pppd_event);
		sem_destroy(&pdp_setup_end);
		sem_destroy(&pdp_deactive_end);
	}
	
	//screen manager

	if(s_tid_connect_dog){
		
		mulevent_set_event(&dog_event,EVENT_DOG_EXIT);
		pthread_join(s_tid_connect_dog,NULL);
		mulevent_destroy(&dog_event);
		sem_destroy(&dog_on_sem);
		sem_destroy(&dog_off_sem);
	}
}


void requestScreenState(void *data, size_t datalen, RIL_Token t)
{
	int on =  ((int *)data)[0];
	dbg_info(DBG_PDP,"requestScreenState = %s",on ?"Screen On":"Screen Off");
     	RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
}
