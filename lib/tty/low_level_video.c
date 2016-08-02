/* Ultra-low level video functions specific to x86 */

#include <low_level_video.h>
#include <stdarg.h>

static char *_cursor=(char *)VIDEO_MEM;
static int _xpos=0,_ypos=0;

void low_level_setup(void){

}

int low_level_getxpos(void){
	return _xpos;
}

int low_level_getypos(void){
	return _ypos;
}

void low_level_setxpos(int x){	/*Needs Some Basic Error Checking */
	_xpos=x;
	_cursor=(char *)(VIDEO_MEM+((x * 80)*2)+(_ypos * 2));
}

void low_level_setypos(int y){	/*Needs Some Basic Error Checking */
	_ypos=y;
	_cursor=(char *)(VIDEO_MEM+((_xpos * 80)*2)+(y * 2));
}


void low_level_putchar(char c){
	if(c=='\n'){
		_xpos++; _ypos=0;
		low_level_setxpos(_xpos);
		return;
	}
	
	*_cursor=c;
	_cursor++;
	*_cursor=7;
	_cursor++;
	_ypos++;
}
