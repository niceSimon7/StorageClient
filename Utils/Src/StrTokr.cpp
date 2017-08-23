//#include <winsock2.h>
//#include <windows.h>
#include <stdio.h>
//#include "strtok_r.h"

char *my_strtok_r(char *head, char del, char **ppctx)
{
	char c;
	char *str;
	if(head!=NULL){
		//first call.
		if((*head)=='\000'){
			*ppctx = head;
			return NULL;
		}
		*ppctx = str = head;
		while((c=*str)!='\000'){
			if(c==del){
				*str = '\000';
				*ppctx = str+1;
				return head;
			}
			str++;
		}
		*ppctx = str;
		return head;
	}
	//next calls. head must be NULL
	str = (*ppctx);
	if((c=*str)=='\000'){
		return NULL;
	}
	head = str;
	while((c=*str)!='\000'){
		if(c==del){
			*str = '\000';
			*ppctx = str+1;
			return head;
		}
		str++;
	}
	*ppctx = str;
	return head;
}
