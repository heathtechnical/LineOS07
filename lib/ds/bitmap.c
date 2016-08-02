#include<types.h>

typedef struct bitmap{
	u8 *base;
	u32 limit;
} bitmap_t;

#define SET_BIT(x, b) ((x) |= (1 << (b)))
#define GET_BIT(x,b) (((x) >> (b)) & 0x01)
#define CLEAR_BIT(x, b) ((x) &= ~(1 << (b)))
#define TOGGLE_BIT(x, b) ((TEST_BIT(x,b))?(CLEAR_BIT(x,b)):(SET_BIT(x,b)))

void bitmap_init(bitmap_t *bitmap)
{
	u32 i,bytes;

	bytes=(bitmap->limit / 8);
	if(bitmap->limit % 8) bytes++;

	/* This should use memset */
	for(i=0;i<bytes;i++) bitmap->base[i]=0x0;
}

s8 bitmap_bitset(bitmap_t *bitmap, u32 bit)
{
	u32 by,bi;

	if(bit>=bitmap->limit) return -1;

	/* Set bit in bitmap */
	by=(bit / 8);
	bi=(bit%8);
	
	SET_BIT(bitmap->base[by],bi);

	return 0;
}

s8 bitmap_bitclear(bitmap_t *bitmap, u32 bit)
{
        u32 by,bi;

        if(bit>=bitmap->limit) return -1;

        /* Clear bit in bitmap */
        by=(bit / 8);
        bi=(bit%8);

        CLEAR_BIT(bitmap->base[by],bi);

        return 0;
}

s8 bitmap_bitget(bitmap_t *bitmap, u32 bit)
{
        u32 by,bi;

        if(bit>=bitmap->limit) return -1;

        /* Set bit in bitmap */
        by=(bit / 8);
        bi=(bit%8);

        return GET_BIT(bitmap->base[by],bi);
}
