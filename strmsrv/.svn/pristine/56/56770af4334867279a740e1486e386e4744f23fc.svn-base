/*
 * atomic.hh
 *
 *  Created on: May 12, 2011
 *      Author: rkale
 */

#ifndef ATOMIC_HH_
#define ATOMIC_HH_

#define SMP_SAFE
//#define USE_ATOMIC

#ifdef SMP_SAFE
#define LOCK "lock ; "
#else
#define LOCK ""
#endif

//typedef struct { volatile int value; } atomic_t;
typedef struct { int value; } atomic_t;

#define atomic_read(atom)              ((atom)->value)
#define atomic_set(atom, arg)          (((atom)->value) = (arg))


#ifdef USE_ATOMIC

static __inline__ void atomic_incr(atomic_t *atom)
{
        __asm__ __volatile__(
                LOCK "incl %0"
                :"=m" (atom->value)
                :"m" (atom->value));
}

static __inline__ void atomic_decr(atomic_t *atom)
{
        __asm__ __volatile__(
                LOCK "decl %0"
                :"=m" (atom->value)
                :"m" (atom->value));

}

static __inline__ void atomic_add(int i, atomic_t *atom)
{
        __asm__ __volatile__(
                LOCK "addl %1,%0"
                :"=m" (atom->value)
                :"ir" (i), "m" (atom->value));
}

static __inline__ void atomic_sub(int i, atomic_t *atom)
{
        __asm__ __volatile__(
                LOCK "subl %1,%0"
                :"=m" (atom->value)
                :"ir" (i), "m" (atom->value));
}


#else
#define atomic_incr(atom)   \
        do { \
                (++((atom)->value)); \
        } while (0)

#define atomic_decr(atom)   \
        do { \
                (--((atom)->value)); \
        } while (0)

#define atomic_add(i, atom)   \
        do { \
                (((atom)->value) += i); \
        } while (0)

#define atomic_sub(i, atom)   \
        do { \
                (((atom)->value) -= i); \
        } while (0)

#endif


#endif /* ATOMIC_HH_ */
