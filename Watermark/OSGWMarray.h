#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <assert.h>
#include "OSGWMcommontypes.h"
#include "OSGWMhashtab.h"        /* hash table implementation */

//#include "./memdebug/memdebug.h"
/******************************************
  6 container classes as substitute for
  STL containers since implementation is
  broken for some compilers/versions.

  ListArray
  GEOMARK_BitArray
  GrowBitArray
  FixedArray
  GrowArray
  HashArray
  HashDelArray

  Oliver Benedens 1999
*******************************************/
#include <string.h>
#include <stdlib.h>

//#include "tinyvector.h"
#define MAX_NUM_CHUNKS  128 // 256

/* defines minimum size for a chunk: 2^MIN_CHUNK_POWER */
#define MIN_CHUNK_POWER 3   //7

/* calculate needed 2-power of chunksize for given number of maximum elements */
/* BUGFIX */
#define POWER_CHUNK_SIZE_MAX_ELEMENTS(X)                                \
        (                                                               \
            MAX2(MIN_CHUNK_POWER,                                       \
                 (int) ceil(log((double) X / MAX_NUM_CHUNKS) / log(2.))) \
        )
#define POWER_CHUNK_SIZE_MAX_BITS(X)                                     \
        (                                                                \
            MAX2(MIN_CHUNK_POWER, (int) MAX2(ceil(                       \
                     log((double) X / MAX_NUM_CHUNKS) / log(2.)) - 3., 0.)) \
        )

/* a list like-array */
/* forward decl */
class   GrowArray;

typedef struct
{
    int prev;
    int next;
} ListLink;

#if 0
class   ListArray
{
private:

    GrowArray   *link_;     /* element type is ListLink */
    GrowArray   *elements_;
    GrowArray   *freeIds_;  /* element type is int */
    int         chunkSize2Power_;
    int         numElements_;
    int         tailIndex_;
    int         firstElementIndex_;
public:

    static int  numAllocated;
    static int  numFreed;

    ListArray(int chunkSize2Power, int sizeElement)
    {
        tailIndex_ = 0;
        numAllocated++;
    }

    ~ListArray(void)
    {
        numFreed--;
    }
    void    *setElement(int index, void *element);
    void    *getElement(int index);
    void    *getFirstElement(void);
    void    *getNextElement(void);
    void    *append(void *element);
};
#endif

/* a simple (fixed) bit array */
class   GEOMARK_BitArray
{
private:

    int             numAllocedBytes_;
    unsigned char   *bits_;
    int             maxIndex_;
    int             initBitValue_;
public:

    static int  numAllocated;
    static int  numFreed;

    GEOMARK_BitArray(int n, int bitValue = 0)
    {
        if(bitValue == 1)
            initBitValue_ = 0xff;
        else
            initBitValue_ = 0;
        numAllocedBytes_ = (n >> 3);
        if((numAllocedBytes_ << 3) < n)
            numAllocedBytes_++;
        bits_ = (unsigned char *) malloc(numAllocedBytes_);
        memset(bits_, initBitValue_, numAllocedBytes_);
        maxIndex_ = n - 1;
        numAllocated++;
    }

    ~GEOMARK_BitArray(void)
    {
        GEOMARK_FREE(bits_);
        bits_ = NULL;
        numFreed++;
    }
    unsigned char *getBitsAddr(void)
    {
        return bits_;
    }

    int getBit(int index);
    int setBit(int index, unsigned char value);
    int getMaxIndex(void)
    {
        return maxIndex_;
    }

    int copyContents(GEOMARK_BitArray *from);
    int empty(void)
    {
        memset(bits_, initBitValue_, numAllocedBytes_);
        return 0;
    }

    /* 32 bit output*/
    int getValue(int startIndex, int numBits, int *value);

    /* 64 bit output */
    int getValue(int startIndex, int numBits, GEOMARK_UINT64 *value);

    int setValue(int startIndex, unsigned char *bytes, int numBits);

    int setValue(int startIndex, int value, int numBits);
    int setValue(int startIndex, GEOMARK_UINT64 value, int numBits);
};

/* a growable bit array */
/*
   - only powers of 2 are allowed for chunk-size 
   - maximum capacity is MAX_NUM_CHUNKS*chunkSize elements
*/
class   GrowBitArray
{
private:

    unsigned char   *ptrElements_[MAX_NUM_CHUNKS];
    int             numChunks_;
    int             chunkSize_;
    int             chunkSize2Power_;
    int             chunkSize2PowerBits_;
    int             maxIndex_;
    int             initBitValue_;
public:

    static int  numAllocated;
    static int  numFreed;

    GrowBitArray(int chunkSize2Power, int bitValue = 0)
    {                       /* space for 8*2^chunkSize2Power bits */
        if(bitValue == 1)
            initBitValue_ = 0xff;
        else
            initBitValue_ = 0;
        chunkSize2Power_ = chunkSize2Power;
        chunkSize2PowerBits_ = chunkSize2Power_ + 3;
        chunkSize_ = 1 << chunkSize2Power_;
        ptrElements_[0] = (unsigned char *) malloc(chunkSize_);
        memset(ptrElements_[0], initBitValue_, chunkSize_);
        numChunks_ = 1;
        maxIndex_ = ((chunkSize_) << 3) - 1;
        numAllocated++;
    }

    ~GrowBitArray(void)
    {
        for(int i = 0; i < numChunks_; i++)
        {
            GEOMARK_FREE(ptrElements_[i]);
            ptrElements_[i] = NULL;
        }
        numFreed++;
    }
    int getBit(int index);
    int setBit(int index, unsigned char value);
    int getMaxIndex(void)
    {
        return maxIndex_;
    }

    int empty(void)
    {
        for(int i = 0; i < numChunks_; i++)
        {
            memset(ptrElements_[i], initBitValue_, chunkSize_);
        }

        return 0;
    }

    int getValue(int startIndex, int numBits, int *value);
    int setValue(int startIndex, unsigned char *bytes, int numBits);
    int setValue(int startIndex, int value, int numBits);
};

/* non resizable array */
/*
  - random access (read & write)
*/
class   FixedArray
{
private:

    void    *ptrElements_;
    int     sizeElement_;
    int     size_;
    int     maxIndex_;
    int     tailIndex_;
public:

    static int  numAllocated;
    static int  numFreed;

    FixedArray(int size, int sizeElement)
    {
        ptrElements_ = (void *) malloc(size * sizeElement);
        size_ = size;
        sizeElement_ = sizeElement;
        memset(ptrElements_, 0, size_ * sizeElement);
        maxIndex_ = size_ - 1;
        tailIndex_ = 0;
        numAllocated++;
    }

    ~FixedArray(void)
    {
        GEOMARK_FREE(ptrElements_);
        ptrElements_ = NULL;
        numFreed++;
    }
    void    *setElement(int index, void *element);
    void    *getElement(int index);
    void    sort(int from, int to, int (*cmp) (const void *, const void *));    /* to==-1 means sort all */
    void    *appendElement(void *element);
    int getMaxIndex(void)
    {
        return maxIndex_;
    }

    int getNumElements(void)
    {
        return tailIndex_;
    }

    int empty(void)
    {
        tailIndex_ = 0;
        return 0;
    }

    int copy(FixedArray *a)
    {
        int i, n;

        empty();
        n = a->getNumElements();
        for(i = 0; i < n; i++)
        {
            appendElement(a->getElement(i));
        }

        return 0;
    }
};

/* growable array */
/*
   - random access (read & write)
   - only powers of 2 are allowed for chunk-size 
   - maximum capacity is MAX_NUM_CHUNKS*chunkSize elements
*/
class   GrowArray
{
    //private:
public:

    void        *ptrElements_[MAX_NUM_CHUNKS];

    //int numChunks_;
    int         maxIndex_;
    int         chunkSize2Power_;
    int         tailIndex_;
    void        qsort_(int from, int to, void *h,
                       int (*cmp) (const void *, const void *));

    //public:
    int         debug_;

    int         numChunks_;
    int         chunkSize_;
    int         sizeElement_;

    static int  numAllocated;
    static int  numFreed;

    GrowArray(int chunkSize2Power, int sizeElement)
    {
        sizeElement_ = sizeElement;
        chunkSize2Power_ = chunkSize2Power;
        chunkSize_ = 1 << chunkSize2Power_;
        numChunks_ = 1;

        /* alloc mem for first chunk */
        ptrElements_[0] = (void *) malloc(chunkSize_ * sizeElement_);
        memset(ptrElements_[0], 0, chunkSize_ * sizeElement_);
        maxIndex_ = chunkSize_ - 1;
        tailIndex_ = 0;
        numAllocated++;

        debug_ = 0;
    }

    ~GrowArray(void)
    {
        for(int i = 0; i < numChunks_; i++)
        {
            GEOMARK_FREE(ptrElements_[i]);
            ptrElements_[i] = NULL;
        }
        numFreed++;
    }
    void    *setElement(int index, void *element);
    void    *getElement(int index);
    void    sort(int from, int to, int (*cmp) (const void *, const void *));    /* to==-1 means sort all */
    void    *appendElement(void *element);
    int     deleteTailElement(void);
    int getMaxIndex(void)
    {
        return maxIndex_;
    }

    int getNumElements(void)
    {
        return tailIndex_;
    }

    int empty(void)
    {
        tailIndex_ = 0;
        return 0;
    }

    int copy(GrowArray *a, BOOL emptyList = TRUE)
    {
        int i, n;

        assert(sizeElement_ == a->sizeElement_);

        if(emptyList)
            empty();
        n = a->getNumElements();
        for(i = 0; i < n; i++)
        {
            appendElement(a->getElement(i));
        }

        return 0;
    }
};

/* associative array */
/*
  - keys must be distinct !
  - array stores only pointers to values
  - uses C-implementation of hash tables
      in hashtab.cpp, lookupa.cpp and recycle.cpp
*/
class   HashArray
{
public:

    htab        *hashTable_;
    GrowArray   *keyValues_;                    /* we need "external" storage because hash
                           implementation stores only references */
    int         sizeElement_;
    int         chunkSize2Power_;
    int         debug_;
public:

    HashArray(int chunkSize2Power, int keySize)
    {
        hashTable_ = hcreate((word) 10);
        keyValues_ = new GrowArray(chunkSize2Power, keySize);
        chunkSize2Power_ = chunkSize2Power;
        sizeElement_ = keySize;
        debug_ = 0;
    }

    ~HashArray(void)
    {
        hdestroy(hashTable_);
        delete(keyValues_);
    }
    int searchElement(void *key, void **value); /* returns 0 if found */
    int insertElement(void *key, void *value);
    GrowArray *getKeyValues(void)
    {
        return keyValues_;
    }

    void    dump(void);
};

/* BUGFIX: not enough space for storing edges and triangles ! */
#define HASH_DEL_ARRAY_MAX_KEY_LENGTH   9

typedef struct
{
    unsigned char   key[HASH_DEL_ARRAY_MAX_KEY_LENGTH];
    int             index;
    int             value;
} HashDelArrayElement;

class   HashDelArray
{
public:

    htab        *hashTable_;
    GrowArray   *keyValues_;                    /* we need "external" storage because hash
                           implementation stores only references */
    GrowArray   *freedPositions_;
    int         sizeElement_;
    int         chunkSize2Power_;
    int         debug_;
public:

    HashDelArray(int chunkSize2Power, int keySize)
    {
        hashTable_ = hcreate((word) 10);
        keyValues_ = new GrowArray(chunkSize2Power, sizeof(HashDelArrayElement));
        freedPositions_ = new GrowArray(chunkSize2Power, sizeof(int));
        chunkSize2Power_ = chunkSize2Power;
        sizeElement_ = keySize;
        debug_ = 0;
    }

    ~HashDelArray(void)
    {
        hdestroy(hashTable_);
        delete(keyValues_);
        delete(freedPositions_);
    }
    int searchElement(void *key, int *value);   /* returns 0 if found */
    int insertElement(void *key, int value);
    int deleteElement(void *key);
    GrowArray *getKeyValues(void)
    {
        return keyValues_;
    }

    int getNumElements(void)
    {
        return keyValues_->getNumElements() - freedPositions_->getNumElements();
    }

    void    dump(void);
};

/* dump grow array consisting of integers */
void    dumpGrowArrayInt(GrowArray *ga);

/* print memory consumption info */
void    printMemoryConsumptionInfo(void);

/* compare functions for sorting integers */
int     compareInts(const void *e1, const void *e2);

/* compare function for sorting doubles */
int     compareDoubles(const void *e1, const void *e2);

/* compare function for sorting floats */
int     compareFloats(const void *e1, const void *e2);

/* compare functions for sorting vectors */
#ifndef ARRAY_SINGLE
int     compareVectorsByLengthAsc(const void *e1, const void *e2);
int     compareVectorsByLengthDesc(const void *e1, const void *e2);
int     compareVectorsByZAsc(const void *e1, const void *e2);
int     compareVectorsByZDesc(const void *e1, const void *e2);
#endif

/* base64 encoding/decoding */
int     GEOMARK_base64_encode(unsigned char *in, char *out, int n);
int     GEOMARK_base64_decode(char *in, unsigned char *out, int n);
int     GEOMARK_calcBase64Len(int numOctets);
#endif
#endif
