#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EDID_DATA_SIZE	128
#define MAX_LINE_SIZE	128

struct MON_struct {

    unsigned short a_hres      ; /* Horizontal Resolution */
    unsigned short a_vres      ; /* Vertical Resolution */
    unsigned short a_hfp       ; /* Horiz Front Porch */
    unsigned short a_hsync     ; /* Horiz Sync */
    unsigned short a_hbp       ; /* Horiz Back Porch */
    unsigned short a_vfp       ; /* Vert Front Porch */
    unsigned short a_vsync     ; /* Vert Sync */
    unsigned short a_vbp       ; /* Vert Back Porch */
    unsigned char  h_pol       ; /* Hsync Polarity */
    unsigned char  v_pol       ; /* Hsync Polarity */
    double         a_refresh   ; /* Refresh Rate (Hz) */
    double         a_pcr       ; /* Pixel Clock Rate (MHz) */
};

static unsigned char edid_header[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00
};

static char *analog_str[] = {
    "Serrated Vsync"
    "Sync on Green", 
    "Composite Sync", 
    "Separate Syncs", 
    "Blank-to-Black Setup", 
};

static char *std_timing_str[] = {
    "800x600@60Hz", "800x600@56Hz", "640x480@75Hz", "640x480@72Hz", 
    "640x480@67Hz", "640x480@60Hz", "720x400@88Hz", "720x400@70Hz" 
};

static char *std_timing_2_str[] = {
    "1280x1024@75Hz", "1024x768@75Hz", "1024x768@70Hz", "1024x768@60Hz",
    "1024x768@87Hz Interlaced", "832x624@75Hz", "800x600@75Hz", "800x600@72Hz" 
};

static char *power_mngt_str[] = {
    "Active Off/Low Power", "Suspend", "Standby" 
};

static char *display_type_str[] = {
    "Monochrome", "RGB Color", "Non-RGB Color", "Undefined"
};

static char *supported_feature_str[] = {
    "Default GTF Supported", 
    "Prefered Timing Mode",
    "Standard Color Space"
};

static char *get_vendor_sign(unsigned char byte0, unsigned char byte1) {
   static char sign[4];
   unsigned short h = ((unsigned short) byte0 << 8) | (unsigned short) byte1;

   sign[0] = ((h >> 10) & 0x1f) + 'A' - 1; 
   sign[1] = ((h >> 5) & 0x1f) + 'A' - 1; 
   sign[2] = (h & 0x1f) + 'A' - 1; 
   sign[3] = 0;

   return sign;
}

static void display_standard_timing(unsigned char c1, unsigned char c2) {
    unsigned short horiz, vert;
    double asprat;

    if (c1 == 0x01 && c2 == 0x01) return;

    horiz = c1 * 8 + 248;
    asprat = ((c2 & 0xc0) == 0x00) ? ((double) 10 / (double) 16) : 
             (((c2 & 0xc0) == 0x40) ? ((double) 3 / (double) 4) :
    	     (((c2 & 0xc0) == 0x80) ? ((double) 4 / (double) 5) :
    	     (double) 9 / (double) 16));
    vert =  asprat * (double) horiz;
    printf("\t%dx%d@%dHz\n", horiz, vert, (c2 & 0x3f) + 60);
}

static void display_descr_block(unsigned char *buf, struct MON_struct *monp) {
    unsigned short x, y;
    unsigned short hblank, vblank;

    x = (((unsigned short) buf[2] & 0xf0) << 4) | (unsigned short) buf[0];
    y = (((unsigned short) buf[2] & 0x0f) << 8) | (unsigned short) buf[1];
    printf("\tHoriz Active %d\tHoriz Blanking %d\n", x, y);
    monp->a_hres = x; hblank = y;

    x = (((unsigned short) buf[5] & 0xf0) << 4) | (unsigned short) buf[3];
    y = (((unsigned short) buf[5] & 0x0f) << 8) | (unsigned short) buf[4];
    printf("\tVert Active %d\tVert Blanking %d\n", x, y);
    monp->a_vres = x; vblank = y;

    x = (((unsigned short) buf[9] & 0xc0) << 2) | (unsigned short) buf[6]; 
    y = (((unsigned short) buf[9] & 0x30) << 4) | (unsigned short) buf[7]; 
    printf("\tHoriz Sync Offset %d\tHsync Pulse Width %d\n", x, y);
    monp->a_hbp = x; monp->a_hsync = y; monp->a_hfp = hblank - x - y; 

    x = (((unsigned short) buf[9] & 0x0c) << 2) | 
				(((unsigned short) buf[8] & 0xf0) >> 4); 
    y = (((unsigned short) buf[9] & 0x03) << 4) | 
				((unsigned short) buf[8] & 0x0f); 
    printf("\tVert Sync Offset %d\tVsync Pulse Width %d\n", x, y);
    monp->a_vbp = x; monp->a_vsync = y; monp->a_vfp = vblank - x - y; 

    x = (((unsigned short) buf[12] & 0xf0) << 4) |
				((unsigned short) buf[10]);
    y = (((unsigned short) buf[12] & 0x0f) << 8) | 
				((unsigned short) buf[11]);
    printf("\tHoriz Image Size %d\tVert Image Size %d\n", x, y);

    printf("\tHoriz Border %d\t\tVert Border %d\n", buf[13], buf[14]);

    printf("\tHsync Polarity %s\tVsync Polarity %s\n", 
	(buf[15] & 0x04) ? "Pos" : "Neg",  (buf[15] & 0x02) ? "Pos" : "Neg");
    monp->h_pol = (buf[15] & 0x04) ? 1 : 0;
    monp->v_pol = (buf[15] & 0x02) ? 1 : 0;

    if (buf[15] & 0x80) printf("\tInterlaced");
    if (buf[15] & 0x20) printf("\tStereo");
    if (buf[15] & 0x18) printf("\tSeparate Sync");
    if (buf[15] & 0xb8) printf("\n");
}

static void display_mon_range(unsigned char *buf) {
    if (buf[5] == 0x00 && buf[6] == 0x0a) {
        printf("\tHoriz Freq %d - %d KHz\n", buf[2], buf[3]);
        printf("\tVert Freq %d - %d\n", buf[0], buf[1]);
        printf("\tPixel Clock %d MHz\n", (unsigned short) buf[4] * 10);
    }
    else if (buf[5] == 0x02 && buf[6] == 0x00) {
        printf("\tStart Horiz Freq %d\n", buf[7] / 2);
        printf("\t%02x %02x %02x %02x %02x\n", 
		buf[8], buf[9], buf[10], buf[11], buf[12]); 
    }
    else {
        int i;
        printf("Invalid monitor range:\n");
        for (i = 0; i < 8; i++)
        printf("0x%02x ", buf[i]);
    }
}

static void display_color_data(unsigned char *buf) {
    unsigned short wx = 0, wy = 0;
    double gamma;
    
    if (buf[0] == 0x01) {
        wx = ((unsigned short) buf[2] << 2) | ((buf[1] & 0x0c) >> 2); 
        wy = ((unsigned short) buf[3] << 2) | (buf[1] & 0x03); 
        gamma = (double) buf[4] / 100.0 + 1.0;   
        printf("White Point Index #1: X %4.3f\tY %4.3f\tGamma %4.2f\n",
		(double) wx / 1024.0, (double) wy / 1024.0, gamma);
    }
    if (buf[5] == 0x02) {
        wx = ((unsigned short) buf[7] << 2) | ((buf[6] & 0x0c) >> 2); 
        wy = ((unsigned short) buf[8] << 2) | (buf[6] & 0x03); 
        gamma = (double) buf[9] / 100.0 + 1.0;   
        printf("White Point Index #2: X %4.3f\tY %4.3f\tGamma %4.2f\n",
		(double) wx / 1024.0, (double) wy / 1024.0, gamma);
    }
}

static void display_edid(unsigned char *buf, int len) {
    int i, j;
    unsigned char checksum = 0;
    unsigned short x, y, pix, aspect_ratio;
    char *sign;
    struct MON_struct mon;

    if (len != EDID_DATA_SIZE) {
        printf("Wrong EDID length %d, should be %d.\n", len, EDID_DATA_SIZE);
        return;
    }

    for (i = 0; i < sizeof(edid_header); i++) {
        if (buf[i] != edid_header[i]) {
            printf("Wrong EDID header. Byte %d = 0x%02x, should be 0x%02x.\n",
		i, buf[i], edid_header[i]);
            return;
        }
    }

    for (i = 0; i < EDID_DATA_SIZE; i++) {
        checksum += buf[i];
    }

    if (checksum != 0) 
        printf("EDID checksum failed (= 0x%02x). Continuing anyway.\n",checksum);
    else
        printf("EDID checksum passed.\n");

    printf("Product Information\n");
    sign = get_vendor_sign(buf[8], buf[9]);
    printf("\tManufacturer: %s\n", sign);
    printf("\tProduct ID: %s%02x%02x\n", sign, buf[11], buf[10]); 
    printf("\tSerial Number: %02x%02x%02x%02x\n", 
			buf[15], buf[14], buf[13], buf[12]); 
    printf("\tWeek of Manufacture: %d\n", buf[16]);
    printf("\tYear of Manufacture: %d\n", buf[17] + 1990);

    printf("EDID Version %d\tRevison %d\n", buf[18], buf[19]);
    
    printf("Basic Display Parameters\n");
    printf("\tVideo Input Definition\n");
    if (buf[20] & 0x80) {
        printf("\t\tDigital %s DFP 1.x compatible\n", 
					(buf[20] & 0x01) ? "" : "Not");
    }
    else {
        printf("\t\tAnalog Video Level %s\n", 
		((buf[20] & 0x60) == 0x00) ? "0.7, 0.3" :
		(((buf[20] & 0x60) == 0x20) ? "0.714, 0.286" :
		(((buf[20] & 0x60) == 0x40) ? "1, 0.4" : "0.7, 0")));
        for (i = 0; i < 5; i++)
            if (buf[20] & (1 << i)) printf("\t\t%s\n", analog_str[i]);
    }  

    printf("\tMaximum Horiz Size %d cm\n", buf[21] * 10);
    printf("\tMaximum Vert Size %d cm\n", buf[22] * 10);
    printf("\tDisplay Gamma %4.2f\n", (double) buf[23] / (double) 100.0 + 1.0);

    printf("\tPower Management Features\n");
    for (i = 0; i < 3; i++) {
        if (buf[24] & (1 << (5 + i))) 
            printf("\t\t%s\n", power_mngt_str[i]);
    }

    printf("\tSupported Features\n");
    printf("\t\t%s\n", display_type_str[(buf[24] & 0x18) >> 3]);
    for (i = 0; i < 3; i++) {
        if (buf[24] & (1 << i)) 
            printf("\t\t%s\n", supported_feature_str[i]);
    }

    printf("Chroma Information\n");
    x = (unsigned short) buf[27] << 2 | ((buf[25] & 0xc0) >> 6);
    y = (unsigned short) buf[28] << 2 | ((buf[25] & 0x30) >> 4);
    printf("\tRed X: %4.3f\tRed Y: %4.3f\n", 
			(double) x / 1024.0, (double) y / 1024.0);
    x = (unsigned short) buf[29] << 2 | ((buf[25] & 0x0c) >> 2);
    y = (unsigned short) buf[30] << 2 | ((buf[25] & 0x03));
    printf("\tGreen X: %4.3f\tGreen Y: %4.3f\n", 
			(double) x / 1024.0, (double) y / 1024.0);
    x = (unsigned short) buf[31] << 2 | ((buf[26] & 0xc0) >> 6);
    y = (unsigned short) buf[32] << 2 | ((buf[26] & 0x30) >> 4);
    printf("\tBlue X: %4.3f\tBlue Y: %4.3f\n", 
			(double) x / 1024.0, (double) y / 1024.0);
    x = (unsigned short) buf[33] << 2 | ((buf[26] & 0x0c) >> 2);
    y = (unsigned short) buf[34] << 2 | ((buf[26] & 0x03));
    printf("\tWhite X: %4.3f\tWhite Y: %4.3f\n", 
			(double) x / 1024.0, (double) y / 1024.0);

    printf("Established Timing I\n");
    for (i = 0; i < 8; i++) {
        if (buf[35] & (1 << i)) 
            printf("\t%s\n", std_timing_str[i]);
    }
    printf("Established Timing II\n");
    for (i = 0; i < 8; i++) {
        if (buf[36] & (1 << i)) 
            printf("\t%s\n", std_timing_2_str[i]);
    }

    printf("Manufacturer's Reserved Timing: 0x%02x\n", buf[37]);

    printf("Standard Timing Identification:\n");
    for (i = 38; i < 54; i += 2) {
        display_standard_timing(buf[i], buf[i + 1]);
    }
    for (i = 54; i < 126; i += 18) {
        double pixclk, refresh;

        printf("Descriptor Block #%d: ", (i - 54) / 18 + 1);

        pix = ((unsigned short) buf[i + 1] << 8) + (unsigned short) buf[i];

        if (pix) {
            printf("\n");
            display_descr_block(&buf[i + 2], &mon);
            pixclk = (double) pix / 100.0;
            refresh = (double) pix * (double) 10 * (double) 1000 / 
		(double) (mon.a_hres + mon.a_hfp + mon.a_hsync + mon.a_hbp) /
                (double) (mon.a_vres + mon.a_vfp + mon.a_vsync + mon.a_vbp);
            printf("\n\tPixel Clock %6.3f MHz, Refresh Rate %6.3f Hz\n", 
			pixclk, refresh);
            printf("\tHoriz Resolution %d, FP %d, Sync %d, BP %d\n", 
			mon.a_hres, mon.a_hfp, mon.a_hsync, mon.a_hbp);
            printf("\tVert Resolution %d, FP %d, Sync %d, BP %d\n", 
			mon.a_vres, mon.a_vfp, mon.a_vsync, mon.a_vbp);
            printf("\tHsync Polarity %c, Vsync Polarity %c\n\n",
			mon.h_pol ? '+' : '-', mon.v_pol ? '+' : '-');
        }
        else {
            switch (buf[i + 3]) {
            case 0xff:
                printf("Monitor Serial Number\n");
                printf("\t%s\n", &buf[i + 5]);
                break;
            case 0xfe:
                {
                    char tmp[15];
                    strncpy(tmp, &buf[i + 5], 14);
                    tmp[14] = '\0';
                    printf("Ascii String\n");
                    printf("\t%s\n", tmp);
                }
                break;
            case 0xfd: 
                printf("Monitor Range Limits\n");
                display_mon_range(&buf[i + 5]);
                break;
            case 0xfc: 
                printf("Monitor Name\n");
                printf("\t%s\n", &buf[i + 5]);
                break;
            case 0xfb: 
                printf("Color Point Data\n");
                display_color_data(&buf[i + 5]);
                break;
            case 0xfa: 
                printf("Standard Timing Data\n");
                for (j = i + 5; j < i + 16; j += 2) 
                    display_standard_timing(buf[j], buf[j + 1]);
                break;
            case 0xf9: 
                printf("Defined Block Type\n");
                for (j = 0; j < 18; j++) {
                    if (j % 8 == 0) printf("\t");
                    printf("%02x ", buf[i + j]);
                    if (j % 8 == 7) printf("\n");
                }
                printf("\n");
                break;
            case 0xf8: 
                printf("Vendor Defined\n");
                for (j = 0; j < 18; j++) {
                    if (j % 8 == 0) printf("\t");
                    printf("%02x ", buf[i + j]);
                    if (j % 8 == 7) printf("\n");
                }
                printf("\n");
                break;
            default: 
                printf("Invalid Block Type (0x%02x)\n", buf[i + 3]);
                for (j = 0; j < 18; j++) {
                    if (j % 8 == 0) printf("\t");
                    printf("%02x ", buf[i + j]);
                    if (j % 8 == 7) printf("\n");
                }
                printf("\n");
                break;
            }
        }
    }
}

static unsigned long readhex(char **bufptr) {
    char *cptr = *bufptr;
    char ch = *cptr;
    unsigned long val = 0;
    
    if (isxdigit(ch)) {
        do {
            val <<= 4;
            val += (isdigit(ch)) ? (ch - '0') : (toupper(ch) - 'A' + 10);
            cptr++;
            ch = *cptr;
        } while (isxdigit(ch));
        *bufptr = cptr;
    }    
    else {
        printf("Bad character '0x%02x/%c' in hex data.\n", ch, ch);
        *bufptr = NULL;
    }

    return val;
}

static int read_hex_from_file(FILE *file, unsigned char result[], int maxlen) {
    char buf[MAX_LINE_SIZE], *bufptr, ch;
    int n = 0;
   
    while (fgets(buf, MAX_LINE_SIZE, file) != NULL && n <= maxlen) {
        buf[MAX_LINE_SIZE - 1] = '\0';
        if (strlen(buf) == MAX_LINE_SIZE-1 && buf[MAX_LINE_SIZE-2] != '\n') {
             printf("Line too long\n");
             return -1;
        }
        if (buf[0] == '\n') continue;

        bufptr = buf; 
        while ((ch = *bufptr) && isspace(ch)) bufptr++;
        while ((ch = *bufptr) != 0 && ch != '\n') {
            result[n++] = readhex(&bufptr); 
            if (bufptr == NULL) 
                return 0;
            while ((ch = *bufptr) && isspace(ch)) bufptr++;
        } 
    }    
    
    return n;	
}

static void print_help(void) {
    printf("Usage: printedid [-h] -f filename\n");
    printf("\tDisplays the content of 'filename' as EDID data\n");
    printf("\tThe file 'filename' should be the Ascii hexdump of\n");
    printf("\tthe EDID data from a monitor using e.g. 'moninfo'.\n");
};

int main(int argc, char **argv) {
    FILE *file = NULL;
    char *name = NULL;
    int opt, i, data_size;
    unsigned char edid_data[EDID_DATA_SIZE + 1];

    while ((opt = getopt(argc, argv, "hf:")) > 0) {
        switch (opt) {
        case 'f':
            name = optarg;
            break;
        case '?':
        case 'h':
            print_help();
            return -1;
        }
    }

    if (name == NULL) {
        print_help();
        return -1;
    }
    if ((file = fopen(name, "r")) < 0 || file == NULL) {
        printf("Can't open input file %s. %m.\n", name);
        return -1;
    }
    data_size = read_hex_from_file(file, edid_data, EDID_DATA_SIZE);
    fclose(file); 

    printf("\nRead %d bytes data from file %s:\n\n", data_size, name);
    for (i = 0; i < data_size; i++) {
        printf("%02x ", edid_data[i]);
        if (i % 16 == 15) printf("\n");
    }
    printf("\n");
    
    display_edid(edid_data, data_size);

    return 0;
}
