#include "gtest/gtest.h"
#include "string.h"
#include "frame_counter_utils.h"
#include "custmenu.h"

TEST(FrameCounterUtilsTest, PrintFrmcnt) 
{

    unsigned char array[256] = {16,0,0,0,32,0,0,0,48,0,0,0,64,0,0,0,96,0,0,0,112,0,0,0,144,0,0,0,176,0,0,0,208,0,0,0,240,0,0,0,28,0,0,0,16,0,0,0,32,0,0,0,16,0,0,0};

    char txt[1024];
    char * pager;
    int level = 14;
    int tm = 5;
    int type, pages; 

  
    pages = print_framecnt_to_buffer(array, txt, 
            (char *) "-- Random Test for TX --", level, tm , BOARDTYPE_TX);
    EXPECT_EQ(pages,-1);

/*
    Frame Processed   # Skipped
    ----------------------------
     00001
     00002          0
     00003          0
     00004          0
     00006          1               // This is how the loop looks when we give the boardtype as TX and with the above array.
     00007          0            
     00009          1
     00011          1
     00013          1
     00015          1

    --Random Test for TX--
*/

    unsigned char array1[256] = {32,0,0,0,48,0,0,0,64,0,0,0,96,0,0,0,112,0,0,0,144,0,0,0,176,0,0,0,208,0,0,0,240,0,0,0,28,0,0,0,16,0,0,0,32,0,0,0,16,0,0,48,0,0,0,56,0,0,0,16,0,0,0,128,0,0,0};

    char text[1024];
    char * pager1;
    int level1 = 10;
    int time = 10;
    int type1, page;


    pages = print_framecnt_to_buffer(array1, text, 
            (char *) "--Test Random for RX--", level1, time , BOARDTYPE_RX);
    EXPECT_EQ(pages,-1);
/*
    Frame Processed   Displayed
    ----------------------------
     00002          1
     00003          1
     00004          1
     00005          0
     00006          1
     00007          1           //This is how the loop looks when we give the boardtype as RX and with the above array values.
     00008          0
     00009          1
     00010          0
     00011          1

--Test Random for RX--
*/

}
