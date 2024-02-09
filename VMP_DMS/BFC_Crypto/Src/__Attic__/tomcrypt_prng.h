#ifdef FORTUNA
struct fortuna_prng {
	 hash_state pool[FORTUNA_POOLS];	  /* the  pools */

	 symmetric_key skey;

	 unsigned char K[32],		/* the current key */
						IV[16];	  /* IV for CTR mode */
	 
	 unsigned long pool_idx,	/* current pool we will add to */
						pool0_len,  /* length of 0'th pool */
						wd;				

	 ulong64		 reset_cnt;  /* number of times we have reset */
	 LTC_MUTEX_TYPE(prng_lock)
};
#endif

#ifdef SOBER128
struct sober128_prng {
	 ulong32		R[17],			 /* Working storage for the shift register */
					  initR[17],		/* saved register contents */ 
					  konst,			 /* key dependent constant */
					  sbuf;			  /* partial word encryption buffer */

	 int			 nbuf,			  /* number of part-word stream bits buffered */
					  flag,			  /* first add_entropy call or not? */
					  set;				/* did we call add_entropy to set key? */
	 
};
#endif

int find_prng(const char *name);
int register_prng(const struct ltc_prng_descriptor *prng);
int unregister_prng(const struct ltc_prng_descriptor *prng);
int prng_is_valid(int idx);
LTC_MUTEX_PROTO(ltc_prng_mutex)

