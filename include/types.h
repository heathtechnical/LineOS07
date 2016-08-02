typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef unsigned long default_type;

/* 
	Bitwise Macros
*/
#define  TEST_BIT(x, b)   (((x) & (1<<(b))) != 0 ) 
#define  SET_BIT(x, b)   ((x) |= (1 << (b))) 
#define  CLEAR_BIT(x, b)   ((x) &= ~(1 << (b))) 
#define  TOGGLE_BIT(x, b)   ((TEST_BIT(x,b))?(CLEAR_BIT(x,b)):(SET_BIT(x,b))) 
