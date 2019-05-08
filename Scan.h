#ifndef _SCAN_H_
#define _SCAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int scan(char* in, char* out, int start, int max)
{
    if (start >= strlen(in))
        return -1;
    
    int i = start;
    int count = 0;
    int out_count = 0;

    while (i < strlen(in)) 
    {
        if ( *(in+i) == ' ' || *(in+i) == '\t' || *(in+i) == '\n' || *(in+i) == '\r') 
        {
            break;
        }
        else 
        {
            if (count < (max-1)) 
            {
                *(out+out_count) = *(in+i);
                ++out_count;
                ++count;
            } 
        }
        i++;
    }

    *(out+out_count) = '\0';

    for( i=i+1; i < strlen(in); i++)
    {
        if ( *(in+i) != ' ' || *(in+i) != '\t' || *(in+i) != '\n' || *(in+i) != '\r') 
        {
            break;
        }
    }

    return i;
}

#endif