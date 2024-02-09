#ifdef LTC_LRW_MODE
/** A LRW structure */
typedef struct {
	/** The index of the cipher chosen (must be a 128-bit block cipher) */
	int					cipher;

	/** The current IV */
	Uchar	IV[16],

	/** the tweak key */
							tweak[16],

	/** The current pad, it's the product of the first 15 bytes against the tweak key */
							pad[16];

	/** The scheduled symmetric key */
	symmetric_key	key;

#ifdef LRW_TABLES
	/** The pre-computed multiplication table */
	Uchar	PC[16][256][16];
#endif
} symmetric_LRW;
#endif

#ifdef LTC_F8_MODE
/** A block cipher F8 structure */
typedef struct {
	/** The index of the cipher chosen */
	int					cipher,
	/** The block size of the given cipher */
							blocklen,
	/** The padding offset */
							padlen;
	/** The current IV */
	Uchar		IV[MAXBLOCKSIZE],
							MIV[MAXBLOCKSIZE];
	/** Current block count */
	Uint32				blockcnt;
	/** The scheduled key */
	symmetric_key		key;
} symmetric_F8;
#endif

#ifdef LTC_LRW_MODE

#define LRW_ENCRYPT 0
#define LRW_DECRYPT 1

int lrw_start(					int	cipher,
				const Uchar *IV,
				const Uchar *key,		int keylen,
				const Uchar *tweak,
									int  num_rounds,
						symmetric_LRW *lrw);
int lrw_encrypt(const Uchar *pt, Uchar *ct, unsigned long len, symmetric_LRW *lrw);
int lrw_decrypt(const Uchar *ct, Uchar *pt, unsigned long len, symmetric_LRW *lrw);
int lrw_getiv(Uchar *IV, unsigned long *len, symmetric_LRW *lrw);
int lrw_setiv(const Uchar *IV, unsigned long len, symmetric_LRW *lrw);
int lrw_done(symmetric_LRW *lrw);
int lrw_test(void);

/* don't call */
int lrw_process(const Uchar *pt, Uchar *ct, unsigned long len, int mode, symmetric_LRW *lrw);
#endif

#ifdef LTC_F8_MODE
int f8_start(					int  cipher, const Uchar *IV,
				const Uchar *key,						int  keylen,
				const Uchar *salt_key,					int  skeylen,
									int  num_rounds,	symmetric_F8  *f8);
int f8_encrypt(const Uchar *pt, Uchar *ct, unsigned long len, symmetric_F8 *f8);
int f8_decrypt(const Uchar *ct, Uchar *pt, unsigned long len, symmetric_F8 *f8);
int f8_getiv(Uchar *IV, unsigned long *len, symmetric_F8 *f8);
int f8_setiv(const Uchar *IV, unsigned long len, symmetric_F8 *f8);
int f8_done(symmetric_F8 *f8);
int f8_test_mode(void);
#endif


int find_cipher(const char *name);
int find_cipher_any(const char *name, int blocklen, int keylen);
int find_cipher_id(Uchar ID);
int register_cipher(const struct ltc_cipher_descriptor *cipher);
int unregister_cipher(const struct ltc_cipher_descriptor *cipher);
int cipher_is_valid(int idx);

LTC_MUTEX_PROTO(ltc_cipher_mutex)

