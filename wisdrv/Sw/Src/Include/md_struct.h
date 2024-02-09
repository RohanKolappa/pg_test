


typedef struct 
{
	char ulRow, ulCol;
	char brRow, brCol;
	unsigned char cellSensitivity;	// cell (motion block) sensitivity, 1 to 100
	unsigned char almThreshold;		// 1 to 100 alarm threshold
/*	unsigned char mask[169];		// Motion region setting Bit Mask for each cell in this region,from upper left
									// conner to lower right conner, 1 means MD enabled in the cell,
									// the effective mask length is roundup ((brRow-ulRow)*(brCol-ulCol) / 8)
*/
	unsigned char mask[203];

	unsigned char direction;
	unsigned char mode; 
	int timeout;		
} MotionROI_t;
