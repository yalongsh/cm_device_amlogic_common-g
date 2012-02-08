/** returns 1 if line starts with prefix, 0 if it does not */
#ifndef _AML_RIL_MISC_H
#define _AML_RIL_MISC_H

#define MANAGER_DEV_IOCTL_INDEX                   2000
#define IOCTL_MANAGER_DEV_GET_NUMBER  MANAGER_DEV_IOCTL_INDEX+1292

#define RIL_PROP_DEBUG_MASK                   "ril.debug.mask"
#define RIL_PROP_DEBUG_LEVEL                  "ril.debug.level"


int strStartsWith(const char *line, const char *prefix);
char *getFirstElementValue(const char* document,
                           const char* elementBeginTag,
                           const char* elementEndTag,
                           char** remainingDocument);
char char2nib(char c);
int ping_test(void);
int get_host_by_name(const char* hostname);
void set_debug_prop(unsigned int mask,unsigned int level);
void get_debug_prop(unsigned int * mask,unsigned int * level);
#endif
