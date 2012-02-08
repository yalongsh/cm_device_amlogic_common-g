/** returns 1 if line starts with prefix, 0 if it does not */
#include <string.h>
#include<sys/types.h>
#include<fcntl.h>
#include <unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in_systm.h>
#include<netinet/ip.h>
#include <cutils/properties.h>

#include "misc.h"

#define LOG_TAG "AML_RIL"
#include <utils/Log.h>

int strStartsWith(const char *line, const char *prefix)
{
    for ( ; *line != '\0' && *prefix != '\0' ; line++, prefix++) {
        if (*line != *prefix) {
            return 0;
        }
    }

    return *prefix == '\0';
}

/**
  * Very simple function that extract and returns whats between ElementBeginTag
  * and ElementEndTag. 
  *
  * Optional ppszRemainingDocument that can return a pointer to the remaining
  * of the document to be "scanned". This can be used if subsequent
  * scanning/searching is desired.
  *
  * This function is used to extract the parameters from the XML result
  * returned by U3xx during a PDP Context setup, and used to parse the
  * tuples of operators returned from AT+COPS.
  *
  *  const char* document        - Document to be scanned
  *  const char* elementBeginTag - Begin tag to scan for, return whats
  *                                between begin/end tags
  *  const char* elementEndTag   - End tag to scan for, return whats
  *                                between begin/end tags
  *  char** remainingDocumen t   - Can return the a pointer to the remaining
  *                                of pszDocument, this parameter is optional
  *
  *  return char* containing whats between begin/end tags, allocated on the
  *               heap, need to free this. 
  *               return NULL if nothing is found.
  */
char *getFirstElementValue(const char* document,
                                  const char* elementBeginTag,
                                  const char* elementEndTag,
                                  char** remainingDocument)
{
    char* value = NULL;
    char* start = NULL;
    char* end = NULL;

    if (document != NULL && elementBeginTag != NULL && elementEndTag != NULL)
    {
        start = strstr(document, elementBeginTag);
        if (start != NULL)      
        {
            end = strstr(start, elementEndTag);
            if (end != NULL)
            {
                int n = strlen(elementBeginTag);
                int m = end - (start + n);
                value = (char*) malloc((m + 1) * sizeof(char));
                strncpy(value, (start + n), m);
                value[m] = (char) 0;
                
                /* Optional, return a pointer to the remaining document,
                   to be used when document contains many tags with same name. */
                if (remainingDocument != NULL)
                {
                    *remainingDocument = end + strlen(elementEndTag);
                }
            }
        }
    }
    return value;
}

char char2nib(char c)
{
    if (c >= 0x30 && c <= 0x39)
        return c - 0x30;

    if (c >= 0x41 && c <= 0x46)
        return c - 0x41 + 0xA;

    if (c >= 0x61 && c <= 0x66)
        return c - 0x61 + 0xA;

    return 0;
}


int get_host_by_name(const char* hostname)
{
	struct hostent *hostnm = gethostbyname(hostname);
	if(hostname == NULL)  return -1;
	if( hostnm == (struct hostent *)0){
			return -1;
	}
	//LOGD("host ip: %s \n", inet_ntoa(*(struct in_addr*)&hostnm->h_addr));
	return 0;
}

int ping_test(void)
{
	return 0;
}

void set_debug_prop(unsigned int mask,unsigned int level)
{
	char value[64]={0,};
	sprintf(value,"%d",mask);
	if (property_set( RIL_PROP_DEBUG_MASK, value)) {
		LOGE("Failed to set %s =%d", RIL_PROP_DEBUG_MASK, mask);
	}

	sprintf(value,"%d",level);
	if (property_set(RIL_PROP_DEBUG_LEVEL, value)) {
		LOGE("Failed to set %s =%d", RIL_PROP_DEBUG_LEVEL, level);
	}
}

void get_debug_prop(unsigned int * mask,unsigned int * level)
{
	char value[64]={0,};

	if (property_get( RIL_PROP_DEBUG_MASK, value,"0") == 0) {
		sscanf(value,"%d",mask);
	}
	
	if (property_get( RIL_PROP_DEBUG_LEVEL, value,"0") == 0) {
		sscanf(value,"%d",level);
	}
}
