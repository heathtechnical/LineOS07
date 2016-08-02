/* string.c - Basic platform indpendant string functions */
#include<low_level_video.h>

	/* strlen - Return length of string */
int strlen(const unsigned char *string){
	int size=0;
	for(size=0; *string!='\0';string++)
		size++;
	
	return size;		
}

//TODO: This is from ominos - NEEDS REWRITING!!!
int strncpy(char *dest, char *src, int count)
{
        char *d, *s;
        int ret = 0;

        d = dest;
        s = src;

        while(*s != '\0' && count != 0)
        {
                *d++ = *s++;
                count--;
                ret++;
        }
        return ret;
}

