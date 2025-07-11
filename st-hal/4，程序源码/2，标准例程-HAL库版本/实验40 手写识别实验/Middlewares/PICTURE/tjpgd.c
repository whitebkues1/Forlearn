/*----------------------------------------------------------------------------/
/ TJpgDec - Tiny JPEG Decompressor R0.01c                     (C)ChaN, 2019
/-----------------------------------------------------------------------------/
/ The TJpgDec is a generic JPEG decompressor module for tiny embedded systems.
/ This is a free software that opened for education, research and commercial
/  developments under license policy of following terms.
/
/  Copyright (C) 2019, ChaN, all right reserved.
/
/ * The TJpgDec module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-----------------------------------------------------------------------------/
/ Oct 04, 2011 R0.01  First release.
/ Feb 19, 2012 R0.01a Fixed decompression fails when scan starts with an escape seq.
/ Sep 03, 2012 R0.01b Added JD_TBLCLIP option.
/ Mar 16, 2019 R0.01c Supprted stdint.h.
/----------------------------------------------------------------------------*/

#include "./PICTURE/tjpgd.h"
#include "./PICTURE/piclib.h"


/*-----------------------------------------------*/
/* Zigzag-order to raster-order conversion table */
/*-----------------------------------------------*/

#define ZIG(n)	Zig[n]

static const uint8_t Zig[64] =  	/* Zigzag-order to raster-order conversion table */
{
    0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};



/*-------------------------------------------------*/
/* Input scale factor of Arai algorithm            */
/* (scaled up 16 bits for fixed point operations)  */
/*-------------------------------------------------*/

#define IPSF(n)	Ipsf[n]

static const uint16_t Ipsf[64] =  	/* See also aa_idct.png */
{
    (uint16_t)(1.00000 * 8192), (uint16_t)(1.38704 * 8192), (uint16_t)(1.30656 * 8192), (uint16_t)(1.17588 * 8192), (uint16_t)(1.00000 * 8192), (uint16_t)(0.78570 * 8192), (uint16_t)(0.54120 * 8192), (uint16_t)(0.27590 * 8192),
    (uint16_t)(1.38704 * 8192), (uint16_t)(1.92388 * 8192), (uint16_t)(1.81226 * 8192), (uint16_t)(1.63099 * 8192), (uint16_t)(1.38704 * 8192), (uint16_t)(1.08979 * 8192), (uint16_t)(0.75066 * 8192), (uint16_t)(0.38268 * 8192),
    (uint16_t)(1.30656 * 8192), (uint16_t)(1.81226 * 8192), (uint16_t)(1.70711 * 8192), (uint16_t)(1.53636 * 8192), (uint16_t)(1.30656 * 8192), (uint16_t)(1.02656 * 8192), (uint16_t)(0.70711 * 8192), (uint16_t)(0.36048 * 8192),
    (uint16_t)(1.17588 * 8192), (uint16_t)(1.63099 * 8192), (uint16_t)(1.53636 * 8192), (uint16_t)(1.38268 * 8192), (uint16_t)(1.17588 * 8192), (uint16_t)(0.92388 * 8192), (uint16_t)(0.63638 * 8192), (uint16_t)(0.32442 * 8192),
    (uint16_t)(1.00000 * 8192), (uint16_t)(1.38704 * 8192), (uint16_t)(1.30656 * 8192), (uint16_t)(1.17588 * 8192), (uint16_t)(1.00000 * 8192), (uint16_t)(0.78570 * 8192), (uint16_t)(0.54120 * 8192), (uint16_t)(0.27590 * 8192),
    (uint16_t)(0.78570 * 8192), (uint16_t)(1.08979 * 8192), (uint16_t)(1.02656 * 8192), (uint16_t)(0.92388 * 8192), (uint16_t)(0.78570 * 8192), (uint16_t)(0.61732 * 8192), (uint16_t)(0.42522 * 8192), (uint16_t)(0.21677 * 8192),
    (uint16_t)(0.54120 * 8192), (uint16_t)(0.75066 * 8192), (uint16_t)(0.70711 * 8192), (uint16_t)(0.63638 * 8192), (uint16_t)(0.54120 * 8192), (uint16_t)(0.42522 * 8192), (uint16_t)(0.29290 * 8192), (uint16_t)(0.14932 * 8192),
    (uint16_t)(0.27590 * 8192), (uint16_t)(0.38268 * 8192), (uint16_t)(0.36048 * 8192), (uint16_t)(0.32442 * 8192), (uint16_t)(0.27590 * 8192), (uint16_t)(0.21678 * 8192), (uint16_t)(0.14932 * 8192), (uint16_t)(0.07612 * 8192)
};



/*---------------------------------------------*/
/* Conversion table for fast clipping process  */
/*---------------------------------------------*/

#if JD_TBLCLIP

#define BYTECLIP(v) Clip8[(uint16_t)(v) & 0x3FF]

static const uint8_t Clip8[1024] =
{
    /* 0..255 */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    /* 256..511 */
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    /* -512..-257 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* -256..-1 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#else	/* JD_TBLCLIP */

inline uint8_t BYTECLIP (
    int16_t val
)
{
    if (val < 0) val = 0;

    if (val > 255) val = 255;

    return (uint8_t)val;
}

#endif



/*-----------------------------------------------------------------------*/
/* Allocate a memory block from memory pool                              */
/*-----------------------------------------------------------------------*/

static void *alloc_pool (	/* Pointer to allocated memory block (NULL:no memory available) */
    JDEC *jd,		/* Pointer to the decompressor object */
    uint16_t nd		/* Number of bytes to allocate */
)
{
    char *rp = 0;


    nd = (nd + 3) & ~3;			/* Align block size to the word boundary */

    if (jd->sz_pool >= nd)
    {
        jd->sz_pool -= nd;
        rp = (char *)jd->pool;			/* Get start of available memory pool */
        jd->pool = (void *)(rp + nd);	/* Allocate requierd bytes */
    }

    return (void *)rp;	/* Return allocated memory block (NULL:no memory to allocate) */
}




/*-----------------------------------------------------------------------*/
/* Create de-quantization and prescaling tables with a DQT segment       */
/*-----------------------------------------------------------------------*/

static int create_qt_tbl (	/* 0:OK, !0:Failed */
    JDEC *jd,				/* Pointer to the decompressor object */
    const uint8_t *data,	/* Pointer to the quantizer tables */
    uint16_t ndata			/* Size of input data */
)
{
    uint16_t i;
    uint8_t d, z;
    int32_t *pb;


    while (ndata)  	/* Process all tables in the segment */
    {
        if (ndata < 65) return JDR_FMT1;	/* Err: table size is unaligned */

        ndata -= 65;
        d = *data++;							/* Get table property */

        if (d & 0xF0) return JDR_FMT1;			/* Err: not 8-bit resolution */

        i = d & 3;								/* Get table ID */
        pb = alloc_pool(jd, 64 * sizeof (int32_t));/* Allocate a memory block for the table */

        if (!pb) return JDR_MEM1;				/* Err: not enough memory */

        jd->qttbl[i] = pb;						/* Register the table */

        for (i = 0; i < 64; i++)  				/* Load the table */
        {
            z = ZIG(i);							/* Zigzag-order to raster-order conversion */
            pb[z] = (int32_t)((uint32_t) * data++ * IPSF(z));	/* Apply scale factor of Arai algorithm to the de-quantizers */
        }
    }

    return JDR_OK;
}




/*-----------------------------------------------------------------------*/
/* Create huffman code tables with a DHT segment                         */
/*-----------------------------------------------------------------------*/

static int create_huffman_tbl (	/* 0:OK, !0:Failed */
    JDEC *jd,					/* Pointer to the decompressor object */
    const uint8_t *data,		/* Pointer to the packed huffman tables */
    uint16_t ndata				/* Size of input data */
)
{
    uint16_t i, j, b, np, cls, num;
    uint8_t d, *pb, *pd;
    uint16_t hc, *ph;


    while (ndata)  	/* Process all tables in the segment */
    {
        if (ndata < 17) return JDR_FMT1;	/* Err: wrong data size */

        ndata -= 17;
        d = *data++;						/* Get table number and class */

        if (d & 0xEE) return JDR_FMT1;		/* Err: invalid class/number */

        cls = d >> 4;
        num = d & 0x0F;		/* class = dc(0)/ac(1), table number = 0/1 */
        pb = alloc_pool(jd, 16);			/* Allocate a memory block for the bit distribution table */

        if (!pb) return JDR_MEM1;			/* Err: not enough memory */

        jd->huffbits[num][cls] = pb;

        for (np = i = 0; i < 16; i++)  		/* Load number of patterns for 1 to 16-bit code */
        {
            np += (pb[i] = *data++);		/* Get sum of code words for each code */
        }

        ph = alloc_pool(jd, (uint16_t)(np * sizeof (uint16_t)));/* Allocate a memory block for the code word table */

        if (!ph) return JDR_MEM1;			/* Err: not enough memory */

        jd->huffcode[num][cls] = ph;
        hc = 0;

        for (j = i = 0; i < 16; i++)  		/* Re-build huffman code word table */
        {
            b = pb[i];

            while (b--) ph[j++] = hc++;

            hc <<= 1;
        }

        if (ndata < np) return JDR_FMT1;	/* Err: wrong data size */

        ndata -= np;
        pd = alloc_pool(jd, np);			/* Allocate a memory block for the decoded data */

        if (!pd) return JDR_MEM1;			/* Err: not enough memory */

        jd->huffdata[num][cls] = pd;

        for (i = 0; i < np; i++)  			/* Load decoded data corresponds to each code ward */
        {
            d = *data++;

            if (!cls && d > 11) return JDR_FMT1;

            *pd++ = d;
        }
    }

    return JDR_OK;
}




/*-----------------------------------------------------------------------*/
/* Extract N bits from input stream                                      */
/*-----------------------------------------------------------------------*/

__attribute__((always_inline)) static int bitext (	/* >=0: extracted data, <0: error code */
    JDEC *jd,		/* Pointer to the decompressor object */
    int nbit		/* Number of bits to extract (1 to 11) */
)
{
    uint8_t msk, s, *dp;
    uint16_t dc, v, f;


    msk = jd->dmsk;
    dc = jd->dctr;
    dp = jd->dptr;	/* Bit mask, number of data available, read ptr */
    s = *dp;
    v = f = 0;

    do
    {
        if (!msk)  				/* Next byte? */
        {
            if (!dc)  			/* No input data is available, re-fill input buffer */
            {
                dp = jd->inbuf;	/* Top of input buffer */
                dc = jd->infunc(jd, dp, JD_SZBUF);

                if (!dc) return 0 - (int16_t)JDR_INP;	/* Err: read error or wrong stream termination */
            }
            else
            {
                dp++;			/* Next data ptr */
            }

            dc--;				/* Decrement number of available bytes */

            if (f)  			/* In flag sequence? */
            {
                f = 0;			/* Exit flag sequence */

                if (*dp != 0) return 0 - (int16_t)JDR_FMT1;	/* Err: unexpected flag is detected (may be collapted data) */

                *dp = s = 0xFF;			/* The flag is a data 0xFF */
            }
            else
            {
                s = *dp;				/* Get next data byte */

                if (s == 0xFF)  		/* Is start of flag sequence? */
                {
                    f = 1;
                    continue;	/* Enter flag sequence */
                }
            }

            msk = 0x80;		/* Read from MSB */
        }

        v <<= 1;	/* Get a bit */

        if (s & msk) v++;

        msk >>= 1;
        nbit--;
    } while (nbit);

    jd->dmsk = msk;
    jd->dctr = dc;
    jd->dptr = dp;

    return (int)v;
}




/*-----------------------------------------------------------------------*/
/* Extract a huffman decoded data from input stream                      */
/*-----------------------------------------------------------------------*/

__attribute__((always_inline)) static int16_t huffext (	/* >=0: decoded data, <0: error code */
    JDEC *jd,				/* Pointer to the decompressor object */
    const uint8_t *hbits,	/* Pointer to the bit distribution table */
    const uint16_t *hcode,	/* Pointer to the code word table */
    const uint8_t *hdata	/* Pointer to the data table */
)
{
    uint8_t msk, s, *dp;
    uint16_t dc, v, f, bl, nd;


    msk = jd->dmsk;
    dc = jd->dctr;
    dp = jd->dptr;	/* Bit mask, number of data available, read ptr */
    s = *dp;
    v = f = 0;
    bl = 16;	/* Max code length */

    do
    {
        if (!msk)  		/* Next byte? */
        {
            if (!dc)  	/* No input data is available, re-fill input buffer */
            {
                dp = jd->inbuf;	/* Top of input buffer */
                dc = jd->infunc(jd, dp, JD_SZBUF);

                if (!dc) return 0 - (int16_t)JDR_INP;	/* Err: read error or wrong stream termination */
            }
            else
            {
                dp++;	/* Next data ptr */
            }

            dc--;		/* Decrement number of available bytes */

            if (f)  		/* In flag sequence? */
            {
                f = 0;		/* Exit flag sequence */

                if (*dp != 0) return 0 - (int16_t)JDR_FMT1;	/* Err: unexpected flag is detected (may be collapted data) */

                *dp = s = 0xFF;			/* The flag is a data 0xFF */
            }
            else
            {
                s = *dp;				/* Get next data byte */

                if (s == 0xFF)  		/* Is start of flag sequence? */
                {
                    f = 1;
                    continue;	/* Enter flag sequence, get trailing byte */
                }
            }

            msk = 0x80;		/* Read from MSB */
        }

        v <<= 1;	/* Get a bit */

        if (s & msk) v++;

        msk >>= 1;

        for (nd = *hbits++; nd; nd--)  	/* Search the code word in this bit length */
        {
            if (v == *hcode++)  		/* Matched? */
            {
                jd->dmsk = msk;
                jd->dctr = dc;
                jd->dptr = dp;
                return *hdata;			/* Return the decoded data */
            }

            hdata++;
        }

        bl--;
    } while (bl);

    return 0 - (int16_t)JDR_FMT1;	/* Err: code not found (may be collapted data) */
}




/*-----------------------------------------------------------------------*/
/* Apply Inverse-DCT in Arai Algorithm (see also aa_idct.png)            */
/*-----------------------------------------------------------------------*/

static void block_idct (
    int32_t *src,	/* Input block data (de-quantized and pre-scaled for Arai Algorithm) */
    uint8_t *dst	/* Pointer to the destination to store the block as byte array */
)
{
    const int32_t M13 = (int32_t)(1.41421 * 4096), M2 = (int32_t)(1.08239 * 4096), M4 = (int32_t)(2.61313 * 4096), M5 = (int32_t)(1.84776 * 4096);
    int32_t v0, v1, v2, v3, v4, v5, v6, v7;
    int32_t t10, t11, t12, t13;
    uint16_t i;

    /* Process columns */
    for (i = 0; i < 8; i++)
    {
        v0 = src[8 * 0];	/* Get even elements */
        v1 = src[8 * 2];
        v2 = src[8 * 4];
        v3 = src[8 * 6];

        t10 = v0 + v2;		/* Process the even elements */
        t12 = v0 - v2;
        t11 = (v1 - v3) * M13 >> 12;
        v3 += v1;
        t11 -= v3;
        v0 = t10 + v3;
        v3 = t10 - v3;
        v1 = t11 + t12;
        v2 = t12 - t11;

        v4 = src[8 * 7];	/* Get odd elements */
        v5 = src[8 * 1];
        v6 = src[8 * 5];
        v7 = src[8 * 3];

        t10 = v5 - v4;		/* Process the odd elements */
        t11 = v5 + v4;
        t12 = v6 - v7;
        v7 += v6;
        v5 = (t11 - v7) * M13 >> 12;
        v7 += t11;
        t13 = (t10 + t12) * M5 >> 12;
        v4 = t13 - (t10 * M2 >> 12);
        v6 = t13 - (t12 * M4 >> 12) - v7;
        v5 -= v6;
        v4 -= v5;

        src[8 * 0] = v0 + v7;	/* Write-back transformed values */
        src[8 * 7] = v0 - v7;
        src[8 * 1] = v1 + v6;
        src[8 * 6] = v1 - v6;
        src[8 * 2] = v2 + v5;
        src[8 * 5] = v2 - v5;
        src[8 * 3] = v3 + v4;
        src[8 * 4] = v3 - v4;

        src++;	/* Next column */
    }

    /* Process rows */
    src -= 8;

    for (i = 0; i < 8; i++)
    {
        v0 = src[0] + (128L << 8);	/* Get even elements (remove DC offset (-128) here) */
        v1 = src[2];
        v2 = src[4];
        v3 = src[6];

        t10 = v0 + v2;				/* Process the even elements */
        t12 = v0 - v2;
        t11 = (v1 - v3) * M13 >> 12;
        v3 += v1;
        t11 -= v3;
        v0 = t10 + v3;
        v3 = t10 - v3;
        v1 = t11 + t12;
        v2 = t12 - t11;

        v4 = src[7];				/* Get odd elements */
        v5 = src[1];
        v6 = src[5];
        v7 = src[3];

        t10 = v5 - v4;				/* Process the odd elements */
        t11 = v5 + v4;
        t12 = v6 - v7;
        v7 += v6;
        v5 = (t11 - v7) * M13 >> 12;
        v7 += t11;
        t13 = (t10 + t12) * M5 >> 12;
        v4 = t13 - (t10 * M2 >> 12);
        v6 = t13 - (t12 * M4 >> 12) - v7;
        v5 -= v6;
        v4 -= v5;

        dst[0] = BYTECLIP((v0 + v7) >> 8);	/* Descale the transformed values 8 bits and output */
        dst[7] = BYTECLIP((v0 - v7) >> 8);
        dst[1] = BYTECLIP((v1 + v6) >> 8);
        dst[6] = BYTECLIP((v1 - v6) >> 8);
        dst[2] = BYTECLIP((v2 + v5) >> 8);
        dst[5] = BYTECLIP((v2 - v5) >> 8);
        dst[3] = BYTECLIP((v3 + v4) >> 8);
        dst[4] = BYTECLIP((v3 - v4) >> 8);
        dst += 8;

        src += 8;	/* Next row */
    }
}




/*-----------------------------------------------------------------------*/
/* Load all blocks in the MCU into working buffer                        */
/*-----------------------------------------------------------------------*/

static JRESULT mcu_load (
    JDEC *jd		/* Pointer to the decompressor object */
)
{
    int32_t *tmp = (int32_t *)jd->workbuf;	/* Block working buffer for de-quantize and IDCT */
    int b, d, e;
    uint16_t blk, nby, nbc, i, z, id, cmp;
    uint8_t *bp;
    const uint8_t *hb, *hd;
    const uint16_t *hc;
    const int32_t *dqf;


    nby = jd->msx * jd->msy;	/* Number of Y blocks (1, 2 or 4) */
    nbc = 2;					/* Number of C blocks (2) */
    bp = jd->mcubuf;			/* Pointer to the first block */

    for (blk = 0; blk < nby + nbc; blk++)
    {
        cmp = (blk < nby) ? 0 : blk - nby + 1;	/* Component number 0:Y, 1:Cb, 2:Cr */
        id = cmp ? 1 : 0;						/* Huffman table ID of the component */

        /* Extract a DC element from input stream */
        hb = jd->huffbits[id][0];				/* Huffman table for the DC element */
        hc = jd->huffcode[id][0];
        hd = jd->huffdata[id][0];
        b = huffext(jd, hb, hc, hd);			/* Extract a huffman coded data (bit length) */

        if (b < 0) return (JRESULT)(0 - b);		/* Err: invalid code or input */

        d = jd->dcv[cmp];						/* DC value of previous block */

        if (b)  								/* If there is any difference from previous block */
        {
            e = bitext(jd, b);					/* Extract data bits */

            if (e < 0) return (JRESULT)(0 - e);	/* Err: input */

            b = 1 << (b - 1);					/* MSB position */

            if (!(e & b)) e -= (b << 1) - 1;	/* Restore sign if needed */

            d += e;								/* Get current value */
            jd->dcv[cmp] = (int16_t)d;			/* Save current DC value for next block */
        }

        dqf = jd->qttbl[jd->qtid[cmp]];			/* De-quantizer table ID for this component */
        tmp[0] = d * dqf[0] >> 8;				/* De-quantize, apply scale factor of Arai algorithm and descale 8 bits */

        /* Extract following 63 AC elements from input stream */
        for (i = 1; i < 64; tmp[i++] = 0) ;		/* Clear rest of elements */

        hb = jd->huffbits[id][1];				/* Huffman table for the AC elements */
        hc = jd->huffcode[id][1];
        hd = jd->huffdata[id][1];
        i = 1;					/* Top of the AC elements */

        do
        {
            b = huffext(jd, hb, hc, hd);		/* Extract a huffman coded value (zero runs and bit length) */

            if (b == 0) break;					/* EOB? */

            if (b < 0) return (JRESULT)(0 - b);	/* Err: invalid code or input error */

            z = (uint16_t)b >> 4;				/* Number of leading zero elements */

            if (z)
            {
                i += z;							/* Skip zero elements */

                if (i >= 64) return JDR_FMT1;	/* Too long zero run */
            }

            if (b &= 0x0F)  					/* Bit length */
            {
                d = bitext(jd, b);				/* Extract data bits */

                if (d < 0) return (JRESULT)(0 - d);/* Err: input device */

                b = 1 << (b - 1);				/* MSB position */

                if (!(d & b)) d -= (b << 1) - 1;/* Restore negative value if needed */

                z = ZIG(i);						/* Zigzag-order to raster-order converted index */
                tmp[z] = d * dqf[z] >> 8;		/* De-quantize, apply scale factor of Arai algorithm and descale 8 bits */
            }
        } while (++i < 64);		/* Next AC element */

        if (JD_USE_SCALE && jd->scale == 3)
        {
            *bp = (uint8_t)((*tmp / 256) + 128);	/* If scale ratio is 1/8, IDCT can be ommited and only DC element is used */
        }
        else
        {
            block_idct(tmp, bp);		/* Apply IDCT and store the block to the MCU buffer */
        }

        bp += 64;				/* Next block */
    }

    return JDR_OK;	/* All blocks have been loaded successfully */
}




/*-----------------------------------------------------------------------*/
/* Output an MCU: Convert YCrCb to RGB and output it in RGB form         */
/*-----------------------------------------------------------------------*/

static JRESULT mcu_output (
    JDEC *jd,		/* Pointer to the decompressor object */
    uint16_t (*outfunc)(JDEC *, void *, JRECT *),	/* RGB output function */
    uint16_t x,		/* MCU position in the image (left of the MCU) */
    uint16_t y		/* MCU position in the image (top of the MCU) */
)
{
    const int16_t CVACC = (sizeof (int16_t) > 2) ? 1024 : 128;
    uint16_t ix, iy, mx, my, rx, ry;
    int16_t yy, cb, cr;
    uint8_t *py, *pc, *rgb24;
    JRECT rect;


    mx = jd->msx * 8;
    my = jd->msy * 8;					/* MCU size (pixel) */
    rx = (x + mx <= jd->width) ? mx : jd->width - x;	/* Output rectangular size (it may be clipped at right/bottom end) */
    ry = (y + my <= jd->height) ? my : jd->height - y;

    if (JD_USE_SCALE)
    {
        rx >>= jd->scale;
        ry >>= jd->scale;

        if (!rx || !ry) return JDR_OK;					/* Skip this MCU if all pixel is to be rounded off */

        x >>= jd->scale;
        y >>= jd->scale;
    }

    rect.left = x;
    rect.right = x + rx - 1;				/* Rectangular area in the frame buffer */
    rect.top = y;
    rect.bottom = y + ry - 1;


    if (!JD_USE_SCALE || jd->scale != 3)  	/* Not for 1/8 scaling */
    {

        /* Build an RGB MCU from discrete comopnents */
        rgb24 = (uint8_t *)jd->workbuf;

        for (iy = 0; iy < my; iy++)
        {
            pc = jd->mcubuf;
            py = pc + iy * 8;

            if (my == 16)  		/* Double block height? */
            {
                pc += 64 * 4 + (iy >> 1) * 8;

                if (iy >= 8) py += 64;
            }
            else  			/* Single block height */
            {
                pc += mx * 8 + iy * 8;
            }

            for (ix = 0; ix < mx; ix++)
            {
                cb = pc[0] - 128; 	/* Get Cb/Cr component and restore right level */
                cr = pc[64] - 128;

                if (mx == 16)  					/* Double block width? */
                {
                    if (ix == 8) py += 64 - 8;	/* Jump to next block if double block heigt */

                    pc += ix & 1;				/* Increase chroma pointer every two pixels */
                }
                else  						/* Single block width */
                {
                    pc++;						/* Increase chroma pointer every pixel */
                }

                yy = *py++;			/* Get Y component */

                /* Convert YCbCr to RGB */
                *rgb24++ = /* R */ BYTECLIP(yy + ((int16_t)(1.402 * CVACC) * cr) / CVACC);
                *rgb24++ = /* G */ BYTECLIP(yy - ((int16_t)(0.344 * CVACC) * cb + (int16_t)(0.714 * CVACC) * cr) / CVACC);
                *rgb24++ = /* B */ BYTECLIP(yy + ((int16_t)(1.772 * CVACC) * cb) / CVACC);
            }
        }

        /* Descale the MCU rectangular if needed */
        if (JD_USE_SCALE && jd->scale)
        {
            uint16_t x, y, r, g, b, s, w, a;
            uint8_t *op;

            /* Get averaged RGB value of each square correcponds to a pixel */
            s = jd->scale * 2;	/* Bumber of shifts for averaging */
            w = 1 << jd->scale;	/* Width of square */
            a = (mx - w) * 3;	/* Bytes to skip for next line in the square */
            op = (uint8_t *)jd->workbuf;

            for (iy = 0; iy < my; iy += w)
            {
                for (ix = 0; ix < mx; ix += w)
                {
                    rgb24 = (uint8_t *)jd->workbuf + (iy * mx + ix) * 3;
                    r = g = b = 0;

                    for (y = 0; y < w; y++)  	/* Accumulate RGB value in the square */
                    {
                        for (x = 0; x < w; x++)
                        {
                            r += *rgb24++;
                            g += *rgb24++;
                            b += *rgb24++;
                        }

                        rgb24 += a;
                    }							/* Put the averaged RGB value as a pixel */

                    *op++ = (uint8_t)(r >> s);
                    *op++ = (uint8_t)(g >> s);
                    *op++ = (uint8_t)(b >> s);
                }
            }
        }

    }
    else  	/* For only 1/8 scaling (left-top pixel in each block are the DC value of the block) */
    {

        /* Build a 1/8 descaled RGB MCU from discrete comopnents */
        rgb24 = (uint8_t *)jd->workbuf;
        pc = jd->mcubuf + mx * my;
        cb = pc[0] - 128;		/* Get Cb/Cr component and restore right level */
        cr = pc[64] - 128;

        for (iy = 0; iy < my; iy += 8)
        {
            py = jd->mcubuf;

            if (iy == 8) py += 64 * 2;

            for (ix = 0; ix < mx; ix += 8)
            {
                yy = *py;	/* Get Y component */
                py += 64;

                /* Convert YCbCr to RGB */
                *rgb24++ = /* R */ BYTECLIP(yy + ((int16_t)(1.402 * CVACC) * cr / CVACC));
                *rgb24++ = /* G */ BYTECLIP(yy - ((int16_t)(0.344 * CVACC) * cb + (int16_t)(0.714 * CVACC) * cr) / CVACC);
                *rgb24++ = /* B */ BYTECLIP(yy + ((int16_t)(1.772 * CVACC) * cb / CVACC));
            }
        }
    }

    /* Squeeze up pixel table if a part of MCU is to be truncated */
    mx >>= jd->scale;

    if (rx < mx)
    {
        uint8_t *s, *d;
        uint16_t x, y;

        s = d = (uint8_t *)jd->workbuf;

        for (y = 0; y < ry; y++)
        {
            for (x = 0; x < rx; x++)  	/* Copy effective pixels */
            {
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
            }

            s += (mx - rx) * 3;	/* Skip truncated pixels */
        }
    }

    /* Convert RGB888 to RGB565 if needed */
    if (JD_FORMAT == 1)
    {
        uint8_t *s = (uint8_t *)jd->workbuf;
        uint16_t w, *d = (uint16_t *)s;
        uint16_t n = rx * ry;

        do
        {
            w = (*s++ & 0xF8) << 8;		/* RRRRR----------- */
            w |= (*s++ & 0xFC) << 3;	/* -----GGGGGG----- */
            w |= *s++ >> 3;				/* -----------BBBBB */
            *d++ = w;
        } while (--n);
    }

    /* Output the RGB rectangular */
    return (JRESULT)outfunc(jd, jd->workbuf, &rect); /* 去掉三目运算,节省时间 */
}




/*-----------------------------------------------------------------------*/
/* Process restart interval                                              */
/*-----------------------------------------------------------------------*/

static JRESULT restart (
    JDEC *jd,		/* Pointer to the decompressor object */
    uint16_t rstn	/* Expected restert sequense number */
)
{
    uint16_t i, dc;
    uint16_t d;
    uint8_t *dp;


    /* Discard padding bits and get two bytes from the input stream */
    dp = jd->dptr;
    dc = jd->dctr;
    d = 0;

    for (i = 0; i < 2; i++)
    {
        if (!dc)  	/* No input data is available, re-fill input buffer */
        {
            dp = jd->inbuf;
            dc = jd->infunc(jd, dp, JD_SZBUF);

            if (!dc) return JDR_INP;
        }
        else
        {
            dp++;
        }

        dc--;
        d = (d << 8) | *dp;	/* Get a byte */
    }

    jd->dptr = dp;
    jd->dctr = dc;
    jd->dmsk = 0;

    /* Check the marker */
    if ((d & 0xFFD8) != 0xFFD0 || (d & 7) != (rstn & 7))
    {
        return JDR_FMT1;	/* Err: expected RSTn marker is not detected (may be collapted data) */
    }

    /* Reset DC offset */
    jd->dcv[2] = jd->dcv[1] = jd->dcv[0] = 0;

    return JDR_OK;
}




/*-----------------------------------------------------------------------*/
/* Analyze the JPEG image and Initialize decompressor object             */
/*-----------------------------------------------------------------------*/

#define	LDB_WORD(ptr)		(uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))


__attribute__((always_inline)) JRESULT jd_prepare (
    JDEC *jd,			/* Blank decompressor object */
    uint16_t (*infunc)(JDEC *, uint8_t *, uint16_t),	/* JPEG strem input function */
    void *pool,			/* Working buffer for the decompression session */
    uint16_t sz_pool,	/* Size of working buffer */
    void *dev			/* I/O device identifier for the session */
)
{
    uint8_t *seg, b;
    uint16_t marker;
    uint32_t ofs;
    uint16_t n, i, j, len;
    JRESULT rc;


    if (!pool) return JDR_PAR;

    jd->pool = pool;		/* Work memroy */
    jd->sz_pool = sz_pool;	/* Size of given work memory */
    jd->infunc = infunc;	/* Stream input function */
    jd->device = dev;		/* I/O device identifier */
    jd->nrst = 0;			/* No restart interval (default) */

    for (i = 0; i < 2; i++)  	/* Nulls pointers */
    {
        for (j = 0; j < 2; j++)
        {
            jd->huffbits[i][j] = 0;
            jd->huffcode[i][j] = 0;
            jd->huffdata[i][j] = 0;
        }
    }

    for (i = 0; i < 4; jd->qttbl[i++] = 0) ;

    jd->inbuf = seg = alloc_pool(jd, JD_SZBUF);		/* Allocate stream input buffer */

    if (!seg) return JDR_MEM1;

    if (jd->infunc(jd, seg, 2) != 2) return JDR_INP;/* Check SOI marker */

    if (LDB_WORD(seg) != 0xFFD8) return JDR_FMT1;	/* Err: SOI is not detected */

    ofs = 2;

    for (;;)
    {
        /* Get a JPEG marker */
        if (jd->infunc(jd, seg, 4) != 4) return JDR_INP;

        marker = LDB_WORD(seg);		/* Marker */
        len = LDB_WORD(seg + 2);	/* Length field */

        if (len <= 2 || (marker >> 8) != 0xFF) return JDR_FMT1;

        len -= 2;		/* Content size excluding length field */
        ofs += 4 + len;	/* Number of bytes loaded */

        switch (marker & 0xFF)
        {
            case 0xC0:	/* SOF0 (baseline JPEG) */

                /* Load segment data */
                if (len > JD_SZBUF) return JDR_MEM2;

                if (jd->infunc(jd, seg, len) != len) return JDR_INP;

                jd->width = LDB_WORD(seg + 3);		/* Image width in unit of pixel */
                jd->height = LDB_WORD(seg + 1);		/* Image height in unit of pixel */

                if (seg[5] != 3) return JDR_FMT3;	/* Err: Supports only Y/Cb/Cr format */

                /* Check three image components */
                for (i = 0; i < 3; i++)
                {
                    b = seg[7 + 3 * i];							/* Get sampling factor */

                    if (!i)  	/* Y component */
                    {
                        if (b != 0x11 && b != 0x22 && b != 0x21)  	/* Check sampling factor */
                        {
                            return JDR_FMT3;					/* Err: Supports only 4:4:4, 4:2:0 or 4:2:2 */
                        }

                        jd->msx = b >> 4;
                        jd->msy = b & 15;		/* Size of MCU [blocks] */
                    }
                    else  	/* Cb/Cr component */
                    {
                        if (b != 0x11) return JDR_FMT3;			/* Err: Sampling factor of Cr/Cb must be 1 */
                    }

                    b = seg[8 + 3 * i];							/* Get dequantizer table ID for this component */

                    if (b > 3) return JDR_FMT3;					/* Err: Invalid ID */

                    jd->qtid[i] = b;
                }

                break;

            case 0xDD:	/* DRI */

                /* Load segment data */
                if (len > JD_SZBUF) return JDR_MEM2;

                if (jd->infunc(jd, seg, len) != len) return JDR_INP;

                /* Get restart interval (MCUs) */
                jd->nrst = LDB_WORD(seg);
                break;

            case 0xC4:	/* DHT */

                /* Load segment data */
                if (len > JD_SZBUF) return JDR_MEM2;

                if (jd->infunc(jd, seg, len) != len) return JDR_INP;

                /* Create huffman tables */
                rc = (JRESULT)create_huffman_tbl(jd, seg, len);

                if (rc) return rc;

                break;

            case 0xDB:	/* DQT */

                /* Load segment data */
                if (len > JD_SZBUF) return JDR_MEM2;

                if (jd->infunc(jd, seg, len) != len) return JDR_INP;

                /* Create de-quantizer tables */
                rc = (JRESULT)create_qt_tbl(jd, seg, len);

                if (rc) return rc;

                break;

            case 0xDA:	/* SOS */

                /* Load segment data */
                if (len > JD_SZBUF) return JDR_MEM2;

                if (jd->infunc(jd, seg, len) != len) return JDR_INP;

                if (!jd->width || !jd->height) return JDR_FMT1;	/* Err: Invalid image size */

                if (seg[0] != 3) return JDR_FMT3;				/* Err: Supports only three color components format */

                /* Check if all tables corresponding to each components have been loaded */
                for (i = 0; i < 3; i++)
                {
                    b = seg[2 + 2 * i];	/* Get huffman table ID */

                    if (b != 0x00 && b != 0x11)	return JDR_FMT3;	/* Err: Different table number for DC/AC element */

                    b = i ? 1 : 0;

                    if (!jd->huffbits[b][0] || !jd->huffbits[b][1])  	/* Check dc/ac huffman table for this component */
                    {
                        return JDR_FMT1;					/* Err: Nnot loaded */
                    }

                    if (!jd->qttbl[jd->qtid[i]])  			/* Check dequantizer table for this component */
                    {
                        return JDR_FMT1;					/* Err: Not loaded */
                    }
                }

                /* Allocate working buffer for MCU and RGB */
                n = jd->msy * jd->msx;						/* Number of Y blocks in the MCU */

                if (!n) return JDR_FMT1;					/* Err: SOF0 has not been loaded */

                len = n * 64 * 2 + 64;						/* Allocate buffer for IDCT and RGB output */

                if (len < 256) len = 256;					/* but at least 256 byte is required for IDCT */

                jd->workbuf = alloc_pool(jd, len);			/* and it may occupy a part of following MCU working buffer for RGB output */

                if (!jd->workbuf) return JDR_MEM1;			/* Err: not enough memory */

                jd->mcubuf = (uint8_t *)alloc_pool(jd, (uint16_t)((n + 2) * 64));	/* Allocate MCU working buffer */

                if (!jd->mcubuf) return JDR_MEM1;			/* Err: not enough memory */

                /* Pre-load the JPEG data to extract it from the bit stream */
                jd->dptr = seg;
                jd->dctr = 0;
                jd->dmsk = 0;	/* Prepare to read bit stream */

                if (ofs %= JD_SZBUF)  						/* Align read offset to JD_SZBUF */
                {
                    jd->dctr = jd->infunc(jd, seg + ofs, (uint16_t)(JD_SZBUF - ofs));
                    jd->dptr = seg + ofs - 1;
                }

                return JDR_OK;		/* Initialization succeeded. Ready to decompress the JPEG image. */

            case 0xC1:	/* SOF1 */
            case 0xC2:	/* SOF2 */
            case 0xC3:	/* SOF3 */
            case 0xC5:	/* SOF5 */
            case 0xC6:	/* SOF6 */
            case 0xC7:	/* SOF7 */
            case 0xC9:	/* SOF9 */
            case 0xCA:	/* SOF10 */
            case 0xCB:	/* SOF11 */
            case 0xCD:	/* SOF13 */
            case 0xCE:	/* SOF14 */
            case 0xCF:	/* SOF15 */
            case 0xD9:	/* EOI */
                return JDR_FMT3;	/* Unsuppoted JPEG standard (may be progressive JPEG) */

            default:	/* Unknown segment (comment, exif or etc..) */

                /* Skip segment data */
                if (jd->infunc(jd, 0, len) != len)  	/* Null pointer specifies to skip bytes of stream */
                {
                    return JDR_INP;
                }
        }
    }
}




/*-----------------------------------------------------------------------*/
/* Start to decompress the JPEG picture                                  */
/*-----------------------------------------------------------------------*/

JRESULT jd_decomp (
    JDEC *jd,								/* Initialized decompression object */
    uint16_t (*outfunc)(JDEC *, void *, JRECT *),	/* RGB output function */
    uint8_t scale							/* Output de-scaling factor (0 to 3) */
)
{
    uint16_t x, y, mx, my;
    uint16_t rst, rsc;
    JRESULT rc;


    if (scale > (JD_USE_SCALE ? 3 : 0)) return JDR_PAR;

    jd->scale = scale;

    mx = jd->msx * 8;
    my = jd->msy * 8;			/* Size of the MCU (pixel) */

    jd->dcv[2] = jd->dcv[1] = jd->dcv[0] = 0;	/* Initialize DC values */
    rst = rsc = 0;

    rc = JDR_OK;

    for (y = 0; y < jd->height; y += my)  		/* Vertical loop of MCUs */
    {
        for (x = 0; x < jd->width; x += mx)  	/* Horizontal loop of MCUs */
        {
            if (jd->nrst && rst++ == jd->nrst)  	/* Process restart interval if enabled */
            {
                rc = restart(jd, rsc++);

                if (rc != JDR_OK) return rc;

                rst = 1;
            }

            rc = mcu_load(jd);					/* Load an MCU (decompress huffman coded stream and apply IDCT) */

            if (rc != JDR_OK) return rc;

            rc = mcu_output(jd, outfunc, x, y);	/* Output the MCU (color space conversion, scaling and output) */

            if (rc != JDR_OK) return rc;
        }
    }

    return rc;
}

/******************************************************************************************/
/* 以下代码为正点原子团队添加, 方便使用TJPGD */

/**
 ****************************************************************************************************
 * @file        tjpgd.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-04
 * @brief       图片解码-jpeg解码 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200404
 * 第一次发布
 *
 ****************************************************************************************************
 */

/* 下面根据是否使用malloc来决定变量的分配方法 */
#if JPEG_USE_MALLOC == 1    /* 使用malloc */

FIL *f_jpeg;                /* JPEG文件指针 */
JDEC *jpeg_dev;             /* 待解码对象结构体指针 */
uint8_t  *jpg_buffer;       /* 定义jpeg解码工作区大小(最少需要3092字节)，作为解压缓冲区，必须4字节对齐 */

/**
 * @brief       给占内存大的数组/结构体申请内存
 * @param       无
 * @retval      操作结果
 *   @arg       0   , 成功
 *   @arg       其他, 错误码
 */
static uint8_t jpeg_mallocall(void)
{
    f_jpeg = (FIL *)piclib_mem_malloc(sizeof(FIL));

    if (f_jpeg == NULL)return PIC_MEM_ERR;      /* 申请内存失败 */

    jpeg_dev = (JDEC *)piclib_mem_malloc(sizeof(JDEC));

    if (jpeg_dev == NULL)return PIC_MEM_ERR;    /* 申请内存失败 */

    jpg_buffer = (uint8_t *)piclib_mem_malloc(JPEG_WBUF_SIZE);

    if (jpg_buffer == NULL)return PIC_MEM_ERR;  /* 申请内存失败 */

    return 0;
}

/**
 * @brief       释放内存
 * @param       无
 * @retval      操作结果
 *   @arg       0   , 成功
 *   @arg       其他, 错误码
 */
static void jpeg_freeall(void)
{
    piclib_mem_free(f_jpeg);        /* 释放f_jpeg申请到的内存 */
    piclib_mem_free(jpeg_dev);      /* 释放jpeg_dev申请到的内存 */
    piclib_mem_free(jpg_buffer);    /* 释放jpg_buffer申请到的内存 */
}

#else       /* 不使用malloc */

FIL  tf_jpeg;
JDEC tjpeg_dev;
FIL  *f_jpeg = &tf_jpeg;                /* JPEG文件指针 */
JDEC *jpeg_dev = &tjpeg_dev;            /* 待解码对象结构体指针 */

/* 定义jpeg解码工作区大小(最少需要3092字节)，作为解压缓冲区，必须4字节对齐 */
__ALIGNED(4) uint8_t jpg_buffer[JPEG_WBUF_SIZE];

#endif

/**
 * @brief       jpeg数据输入回调函数
 * @param       jd       : 储存待解码的对象信息的结构体
 * @param       buf      : 输入数据缓冲区 (NULL:执行地址偏移)
 * @param       num      : 需要从输入数据流读出的数据量/地址偏移量
 * @retval      读取到的字节数/地址偏移量
 */
static uint16_t jpeg_in_func(JDEC *jd, uint8_t *buf, uint16_t num)
{
    uint16_t  rb;   /* 读取到的字节数 */
    FIL *dev = (FIL *)jd->device;           /* 待解码的文件的信息，使用FATFS中的FIL结构类型进行定义 */

    if (buf)        /* 读取数据有效，开始读取数据 */
    {
        f_read(dev, buf, num, (UINT *)&rb); /* 调用FATFS的f_read函数，用于把jpeg文件的数据读取出来 */
        return rb;  /* 返回读取到的字节数目 */
    }
    else
    {
        return (f_lseek(dev, f_tell(dev) + num) == FR_OK) ? num : 0;    /* 重新定位数据点，相当于删除之前的n字节数据 */
    }
}

/**
 * @brief       采用填充的方式进行图片解码显示(速度快)
 * @param       jd       : 储存待解码的对象信息的结构体
 * @param       rgbbuf   : 指向等待输出的RGB位图数据的指针
 * @param       rect     : 等待输出的矩形图像的参数
 * @retval      操作结果
 *   @arg       0   , 输出成功
 *   @arg       其他, 输出失败/结束输出
 */
static uint16_t jpeg_out_func_fill(JDEC *jd, void *rgbbuf, JRECT *rect)
{
    uint16_t *pencolor = (uint16_t *)rgbbuf;
    uint16_t width = rect->right - rect->left + 1;  /* 填充的宽度 */
    uint16_t height = rect->bottom - rect->top + 1; /* 填充的高度 */
    pic_phy.fillcolor(rect->left + picinfo.S_XOFF, rect->top + picinfo.S_YOFF, width, height, pencolor);    /* 颜色填充 */
    return 0;   /* 返回0,使得解码工作继续执行 */
}

/**
 * @brief       采用画点的方式进行图片解码显示(速度慢)
 * @param       jd       : 储存待解码的对象信息的结构体
 * @param       rgbbuf   : 指向等待输出的RGB位图数据的指针
 * @param       rect     : 等待输出的矩形图像的参数
 * @retval      操作结果
 *   @arg       0   , 输出成功
 *   @arg       其他, 输出失败/结束输出
 */
static uint16_t jpeg_out_func_point(JDEC *jd, void *rgbbuf, JRECT *rect)
{
    uint16_t i, j;
    uint16_t realx = rect->left, realy = 0;
    uint16_t *pencolor = rgbbuf;
    uint16_t width = rect->right - rect->left + 1;  /* 图片的宽度 */
    uint16_t height = rect->bottom - rect->top + 1; /* 图片的高度 */

    for (i = 0; i < height; i++)    /* y坐标 */
    {
        realy = (picinfo.Div_Fac * (rect->top + i)) >> 13;  /* 实际Y坐标 */

        /* 在这里不改变picinfo.staticx和picinfo.staticy的值 ,如果在这里改变,则会造成每块的第一个点不显示!!! */
        if (!piclib_is_element_ok(realx, realy, 0))   /* 行值是否满足条件? 寻找满足条件的行 */
        {
            pencolor += width;
            continue;
        }

        for (j = 0; j < width; j++)   /* x坐标 */
        {
            realx = (picinfo.Div_Fac * (rect->left + j)) >> 13; /* 实际X坐标 */

            /* 在这里改变picinfo.staticx和picinfo.staticy的值 */
            if (!piclib_is_element_ok(realx, realy, 1))   /* 列值是否满足条件? 寻找满足条件的列 */
            {
                pencolor++;
                continue;
            }

            pic_phy.draw_point(realx + picinfo.S_XOFF, realy + picinfo.S_YOFF, *pencolor);  /* 显示图片 */
            pencolor++;
        }
    }

    return 0;   /* 返回0,使得解码工作继续执行 */
}

/**
 * @brief       获取JPEG/JPG图片的宽度和高度
 * @param       filename : 包含路径的文件名(.jpeg/.jpg)
 * @param       width    : 图片宽度
 * @param       height   : 图片高度
 * @retval      操作结果
 *   @arg       0   , 成功
 *   @arg       其他, 失败
 */
uint8_t jpg_get_size(const uint8_t *filename, uint32_t *width, uint32_t *height)
{
    uint8_t res = 0;        /* 返回值 */
#if JPEG_USE_MALLOC == 1    /* 使用malloc */
    res = jpeg_mallocall();
#endif

    if (res == 0)
    {
        /* 得到JPEG/JPG图片的开始信息 */
        res = f_open(f_jpeg, (const TCHAR *)filename, FA_READ); /* 打开文件 */

        if (res == FR_OK)   /* 打开文件成功 */
        {
            res = jd_prepare(jpeg_dev, jpeg_in_func, jpg_buffer, JPEG_WBUF_SIZE, f_jpeg);   /* 执行解码的准备工作，调用TjpgDec模块的jd_prepare函数 */

            if (res == JDR_OK)  /* 准备解码成功,即获取到了图片的宽度和高度.返回正确的宽度和高度 */
            {
                *width = jpeg_dev->width;
                *height = jpeg_dev->height;
            }
        }

        f_close(f_jpeg);    /* 解码工作执行成功，返回0 */
    }

#if JPEG_USE_MALLOC == 1    /* 使用malloc */
    jpeg_freeall();         /* 释放内存 */
#endif
    return res;
}

/**
 * @brief       采用画点的方式进行图片解码显示(速度慢)
 * @param       filename : 包含路径的文件名(.jpeg/.jpg)
 * @param       fast     : 使能快速解码
 *   @arg                  0, 不使能
 *   @arg                  1, 使能
 *   @note                 图片尺寸小于等于液晶分辨率,才支持快速解码
 *
 * @param       rect     : 等待输出的矩形图像的参数
 * @retval      操作结果
 *   @arg       0   , 成功
 *   @arg       其他, 错误码
 */
uint8_t jpg_decode(const char *filename, uint8_t fast)
{
    uint8_t res = 0;    /* 返回值 */
    uint8_t scale;      /* 图像输出比例 0,1/2,1/4,1/8 */
    uint16_t (*outfun)(JDEC *, void *, JRECT *);

#if JPEG_USE_MALLOC == 1    /* 使用malloc */
    res = jpeg_mallocall();
#endif

    if (res == 0)
    {
        /* 得到JPEG/JPG图片的开始信息 */
        res = f_open(f_jpeg, (const TCHAR *)filename, FA_READ); /* 打开文件 */

        if (res == FR_OK)       /* 打开文件成功 */
        {
            res = jd_prepare(jpeg_dev, jpeg_in_func, jpg_buffer, JPEG_WBUF_SIZE, f_jpeg);   /* 执行解码的准备工作，调用TjpgDec模块的jd_prepare函数 */
            outfun = jpeg_out_func_point;   /* 默认采用画点的方式显示 */

            if (res == JDR_OK)   /* 准备解码成功 */
            {
                for (scale = 0; scale < 4; scale++)   /* 确定输出图像的比例因子 */
                {
                    if ((jpeg_dev->width >> scale) <= picinfo.S_Width && (jpeg_dev->height >> scale) <= picinfo.S_Height)   /* 在目标区域内 */
                    {
                        if (((jpeg_dev->width >> scale) != picinfo.S_Width) && ((jpeg_dev->height >> scale) != picinfo.S_Height && scale))
                        {
                            scale = 0;  /* 不能贴边,则不缩放 */
                        }
                        else
                        {
                            outfun = jpeg_out_func_fill;    /* 在显示尺寸以内,可以采用填充的方式显示 */
                        }

                        break;
                    }
                }

                if (scale == 4)scale = 0;   /* 错误 */

                if (fast == 0)   /* 不需要快速解码 */
                {
                    outfun = jpeg_out_func_point;   /* 默认采用画点的方式显示 */
                }

                picinfo.ImgHeight = jpeg_dev->height >> scale;  /* 缩放后的图片尺寸 */
                picinfo.ImgWidth = jpeg_dev->width >> scale;    /* 缩放后的图片尺寸 */
                piclib_ai_draw_init();  /* 初始化智能画图 */

                /* 执行解码工作，调用TjpgDec模块的jd_decomp函数 */
                res = jd_decomp(jpeg_dev, outfun, scale);
            }
        }

        f_close(f_jpeg);    /* 解码工作执行成功，返回0 */
    }

#if JPEG_USE_MALLOC == 1    /* 使用malloc */
    jpeg_freeall();         /* 释放内存 */
#endif
    return res;
}



























