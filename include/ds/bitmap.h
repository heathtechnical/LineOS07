/*
	Bitmap Structure
*/
typedef struct bitmap{
        u8 *base;
        u32 limit;
} bitmap_t;

/*
	Bitmap Macros
*/
#define SET_BIT(x, b) ((x) |= (1 << (b)))
#define GET_BIT(x,b) (((x) >> (b)) & 0x01)
#define CLEAR_BIT(x, b) ((x) &= ~(1 << (b)))
#define TOGGLE_BIT(x, b) ((TEST_BIT(x,b))?(CLEAR_BIT(x,b)):(SET_BIT(x,b)))

/*
	Function Declarations
*/
void bitmap_init(bitmap_t *bitmap);
s8 bitmap_bitset(bitmap_t *bitmap, u32 bit);
s8 bitmap_bitclear(bitmap_t *bitmap, u32 bit);
s8 bitmap_bitget(bitmap_t *bitmap, u32 bit);
