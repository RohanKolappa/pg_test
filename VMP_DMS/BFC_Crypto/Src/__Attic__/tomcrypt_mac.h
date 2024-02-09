#ifdef EAX_MODE

#if !(defined(LTC_OMAC) && defined(LTC_CTR_MODE))
	#error EAX_MODE requires OMAC and CTR
#endif

typedef struct {
	unsigned char N[MAXBLOCKSIZE];
	symmetric_CTR ctr;
	omac_state	 headeromac, ctomac;
} eax_state;

int eax_init(eax_state *eax, int cipher, const unsigned char *key, unsigned long keylen,
				 const unsigned char *nonce, unsigned long noncelen,
				 const unsigned char *header, unsigned long headerlen);

int eax_encrypt(eax_state *eax, const unsigned char *pt, unsigned char *ct, unsigned long length);
int eax_decrypt(eax_state *eax, const unsigned char *ct, unsigned char *pt, unsigned long length);
int eax_addheader(eax_state *eax, const unsigned char *header, unsigned long length);
int eax_done(eax_state *eax, unsigned char *tag, unsigned long *taglen);

int eax_encrypt_authenticate_memory(int cipher,
	 const unsigned char *key,	 unsigned long keylen,
	 const unsigned char *nonce,  unsigned long noncelen,
	 const unsigned char *header, unsigned long headerlen,
	 const unsigned char *pt,	  unsigned long ptlen,
			 unsigned char *ct,
			 unsigned char *tag,	 unsigned long *taglen);

int eax_decrypt_verify_memory(int cipher,
	 const unsigned char *key,	 unsigned long keylen,
	 const unsigned char *nonce,  unsigned long noncelen,
	 const unsigned char *header, unsigned long headerlen,
	 const unsigned char *ct,	  unsigned long ctlen,
			 unsigned char *pt,
			 unsigned char *tag,	 unsigned long taglen,
			 int			  *stat);

 int eax_test(void);
#endif /* EAX MODE */

#ifdef OCB_MODE
typedef struct {
	unsigned char	  L[MAXBLOCKSIZE],			/* L value */
							Ls[32][MAXBLOCKSIZE],	 /* L shifted by i bits to the left */
							Li[MAXBLOCKSIZE],		  /* value of Li [current value, we calc from previous recall] */
							Lr[MAXBLOCKSIZE],		  /* L * x^-1 */
							R[MAXBLOCKSIZE],			/* R value */
							checksum[MAXBLOCKSIZE];  /* current checksum */

	symmetric_key	  key;							/* scheduled key for cipher */
	unsigned long	  block_index;				 /* index # for current block */
	int					cipher,						/* cipher idx */
							block_len;					/* length of block */
} ocb_state;

int ocb_init(ocb_state *ocb, int cipher, 
				 const unsigned char *key, unsigned long keylen, const unsigned char *nonce);

int ocb_encrypt(ocb_state *ocb, const unsigned char *pt, unsigned char *ct);
int ocb_decrypt(ocb_state *ocb, const unsigned char *ct, unsigned char *pt);

int ocb_done_encrypt(ocb_state *ocb, 
							const unsigned char *pt,  unsigned long ptlen,
									unsigned char *ct, 
									unsigned char *tag, unsigned long *taglen);

int ocb_done_decrypt(ocb_state *ocb, 
							const unsigned char *ct,  unsigned long ctlen,
									unsigned char *pt, 
							const unsigned char *tag, unsigned long taglen, int *stat);

int ocb_encrypt_authenticate_memory(int cipher,
	 const unsigned char *key,	 unsigned long keylen,
	 const unsigned char *nonce,  
	 const unsigned char *pt,	  unsigned long ptlen,
			 unsigned char *ct,
			 unsigned char *tag,	 unsigned long *taglen);

int ocb_decrypt_verify_memory(int cipher,
	 const unsigned char *key,	 unsigned long keylen,
	 const unsigned char *nonce,  
	 const unsigned char *ct,	  unsigned long ctlen,
			 unsigned char *pt,
	 const unsigned char *tag,	 unsigned long taglen,
			 int			  *stat);

int ocb_test(void);

/* internal functions */
void ocb_shift_xor(ocb_state *ocb, unsigned char *Z);
int ocb_ntz(unsigned long x);
int s_ocb_done(ocb_state *ocb, const unsigned char *pt, unsigned long ptlen,
					unsigned char *ct, unsigned char *tag, unsigned long *taglen, int mode);

#endif /* OCB_MODE */

#ifdef CCM_MODE

#define CCM_ENCRYPT 0
#define CCM_DECRYPT 1

int ccm_memory(int cipher,
	 const unsigned char *key,	 unsigned long keylen,
	 symmetric_key		 *uskey,
	 const unsigned char *nonce,  unsigned long noncelen,
	 const unsigned char *header, unsigned long headerlen,
			 unsigned char *pt,	  unsigned long ptlen,
			 unsigned char *ct,
			 unsigned char *tag,	 unsigned long *taglen,
						  int  direction);

int ccm_test(void);

#endif /* CCM_MODE */

#if defined(LRW_MODE) || defined(GCM_MODE)
void gcm_gf_mult(const unsigned char *a, const unsigned char *b, unsigned char *c);
#endif


/* table shared between GCM and LRW */
#if defined(GCM_TABLES) || defined(LRW_TABLES) || ((defined(GCM_MODE) || defined(GCM_MODE)) && defined(LTC_FAST))
extern const unsigned char gcm_shift_table[];
#endif

#ifdef GCM_MODE

#define GCM_ENCRYPT 0
#define GCM_DECRYPT 1

#define GCM_MODE_IV	 0
#define GCM_MODE_AAD	1
#define GCM_MODE_TEXT  2

typedef struct { 
	symmetric_key		 K;
	unsigned char		 H[16],		  /* multiplier */
							  X[16],		  /* accumulator */
							  Y[16],		  /* counter */
							  Y_0[16],		/* initial counter */
							  buf[16];		/* buffer for stuff */

	int					  cipher,		 /* which cipher */
							  ivmode,		 /* Which mode is the IV in? */
							  mode,			/* mode the GCM code is in */
							  buflen;		 /* length of data in buf */

	ulong64				 totlen,		 /* 64-bit counter used for IV and AAD */
							  pttotlen;	  /* 64-bit counter for the PT */

#ifdef GCM_TABLES
	unsigned char		 PC[16][256][16]  /* 16 tables of 8x128 */
#ifdef GCM_TABLES_SSE2
__attribute__ ((aligned (16)))
#endif
;
#endif  
} gcm_state;

void gcm_mult_h(gcm_state *gcm, unsigned char *I);

int gcm_init(gcm_state *gcm, int cipher,
				 const unsigned char *key, int keylen);

int gcm_reset(gcm_state *gcm);

int gcm_add_iv(gcm_state *gcm, 
					const unsigned char *IV,	  unsigned long IVlen);

int gcm_add_aad(gcm_state *gcm,
					const unsigned char *adata,  unsigned long adatalen);

int gcm_process(gcm_state *gcm,
							unsigned char *pt,	  unsigned long ptlen,
							unsigned char *ct,
							int direction);

int gcm_done(gcm_state *gcm, 
							unsigned char *tag,	 unsigned long *taglen);

int gcm_memory(		int			  cipher,
					const unsigned char *key,	 unsigned long keylen,
					const unsigned char *IV,	  unsigned long IVlen,
					const unsigned char *adata,  unsigned long adatalen,
							unsigned char *pt,	  unsigned long ptlen,
							unsigned char *ct, 
							unsigned char *tag,	 unsigned long *taglen,
										 int direction);
int gcm_test(void);

#endif /* GCM_MODE */

