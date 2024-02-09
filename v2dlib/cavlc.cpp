/*
 * cavlc.cpp
 *
 *  Created on: Oct 26, 2010
 *      Author: rkale
 */

#include <stdlib.h>

#include "cavlc.hh"
#include "svc.h"
#include "bitstreamwriter.hh"

//#define DEBUG_CAVLC

#ifdef DEBUG_CAVLC
#define PRINTF printf
#else
#define PRINTF(...) do {} while (0)
#endif

//#define COLLECT_STATS

#define bs_write(count, bits) pBitStream->PutBits((bits), (count))
#define bs_write_vlc(v) pBitStream->PutBits((v).i_bits, (v).i_size)

void CCAVLC::Encode(CBitStreamWriter * pBitStream, int i_idx,
        int *l, int i_count, int nonZeroPredict)
{

    int level[16], run[16];
    int i_total, i_trailing;
    int i_total_zero;
    int i_last;
    unsigned int i_sign;

    int i;
    int i_zero_left;
    int i_suffix_length, i_suffix_mask;


    /* first find i_last */
    i_last = i_count - 1;
    while( i_last >= 0 && l[i_last] == 0 )
    {
        i_last--;
    }

    i_sign = 0;
    i_total = 0;
    i_trailing = 0;
    i_total_zero = 0;

    if( i_last >= 0 )
    {
        int b_trailing = 1;
        int idx = 0;

        /* level and run and total */
        while( i_last >= 0 )
        {
            level[idx] = l[i_last--];

            run[idx] = 0;
            while( i_last >= 0 && l[i_last] == 0 )
            {
                run[idx]++;
                i_last--;
            }

            i_total++;
            i_total_zero += run[idx];

            if( b_trailing && abs( level[idx] ) == 1 && i_trailing < 3 )
            {
                i_sign <<= 1;
                if( level[idx] < 0 )
                {
                    i_sign |= 0x01;
                }

                i_trailing++;
            }
            else
            {
                b_trailing = 0;
            }

            idx++;
        }
    }

    PRINTF("Putting coeff tokens total = %d trailing = %d\n", i_total, i_trailing);
    /* total/trailing */
    if( i_idx == BLOCK_INDEX_CHROMA_DC )
    {
        bs_write_vlc(x264_coeff_token[4][i_total*4+i_trailing]);
    }
    else
    {
        /* x264_mb_predict_non_zero_code return 0 <-> (16+16+1)>>1 = 16 */
        static const int ct_index[17] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3 };

        bs_write_vlc(x264_coeff_token[ct_index[nonZeroPredict]][i_total*4+i_trailing] );
    }

#ifdef COLLECT_STATS
    //FillBucket(5);
#endif

    if( i_total <= 0 )
    {
        //printf("Encoding zeros!!\n");
        return;
    }

    PRINTF("Trailing ones\n");

    i_suffix_length = i_total > 10 && i_trailing < 3 ? 1 : 0;
    i_suffix_mask = i_suffix_length == 1 ? 0x1 : 0x00;

    if( i_trailing > 0 )
    {
        bs_write(i_trailing, i_sign );
    }
#ifdef COLLECT_STATS
    //FillBucket(6);
#endif

    PRINTF("Coefficients\n");

    for( i = i_trailing; i < i_total; i++ )
    {
        int i_level_code;


        /* calculate level code */
        if( level[i] < 0 )
        {
            i_level_code = -2*level[i] - 1;
        }
        else /* if( level[i] > 0 ) */
        {
            i_level_code = 2 * level[i] - 2;
        }
        if( i == i_trailing && i_trailing < 3 )
        {
            i_level_code -=2; /* as level[i] can't be 1 for the first one if i_trailing < 3 */
        }

        PRINTF("Coefficient level[%d] = %d, levelcode = %d\n", i , level[i], i_level_code);

        if( ( i_level_code >> i_suffix_length ) < 14 )
        {
            PRINTF("Case 1 suffix_length = %d\n", i_suffix_length);
            bs_write_vlc(x264_level_prefix[i_level_code >> i_suffix_length] );
            if( i_suffix_length > 0 )
            {
                bs_write(i_suffix_length, i_level_code & i_suffix_mask);
            }
        }
        else if( i_suffix_length == 0 && i_level_code < 30 )
        {
            PRINTF("Case 2 suffix_length = %d\n", i_suffix_length);
            bs_write_vlc(x264_level_prefix[14] );
            bs_write(4, i_level_code - 14 );
        }
        else if( i_suffix_length > 0 && ( i_level_code >> i_suffix_length ) == 14 )
        {
            PRINTF("Case 3 suffix_length = %d\n", i_suffix_length);
            bs_write_vlc(x264_level_prefix[14] );
            bs_write(i_suffix_length, i_level_code & i_suffix_mask);
        }
        else
        {
            PRINTF("Case 4 suffix_length = %d\n", i_suffix_length);
            bs_write_vlc(x264_level_prefix[15] );
            i_level_code -= 15 << i_suffix_length;
            if( i_suffix_length == 0 )
            {
                i_level_code -= 15;
            }

            if( i_level_code >= ( 1 << 12 ) || i_level_code < 0 )
            {
                PRINTF("OVERFLOW levelcode=%d\n", i_level_code);
                //x264_log(h, X264_LOG_ERROR, "OVERFLOW levelcode=%d\n", i_level_code );
            }

            bs_write(12, i_level_code );    /* check overflow ?? */
        }

        if( i_suffix_length == 0 )
        {
            i_suffix_length++;
            i_suffix_mask = 0x1;
        }
        PRINTF("Abs level %d %d\n", level[i], abs(level[i]));
        if( abs( level[i] ) > ( 3 << ( i_suffix_length - 1 ) ) && i_suffix_length < 6 )
        {
            i_suffix_length++;
            i_suffix_mask <<= 1;
            i_suffix_mask |= 0x1;
        }
    }
#ifdef COLLECT_STATS
    //FillBucket(7);
#endif

    PRINTF("Total zeros %d\n", i_total_zero);

    if( i_total < i_count )
    {
        if( i_idx == BLOCK_INDEX_CHROMA_DC )
        {
            bs_write_vlc(x264_total_zeros_dc[i_total-1][i_total_zero] );
        }
        else
        {
            bs_write_vlc(x264_total_zeros[i_total-1][i_total_zero] );
        }
    }
#ifdef COLLECT_STATS
    //FillBucket(8);
#endif

    PRINTF("Runs before\n");

    for( i = 0, i_zero_left = i_total_zero; i < i_total - 1; i++ )
    {
        int i_zl;

        if( i_zero_left <= 0 )
        {
            break;
        }

        i_zl = SVC_MIN( i_zero_left - 1, 6 );

        PRINTF("Runs before zl = %d, run = %d\n", i_zl, run[i]);
        bs_write_vlc(x264_run_before[i_zl][run[i]] );

        i_zero_left -= run[i];
    }
#ifdef COLLECT_STATS
    //FillBucket(9);
#endif


}
