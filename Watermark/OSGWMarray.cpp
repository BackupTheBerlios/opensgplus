#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#include <assert.h>

#include "OSGWMmylimits.h"
#include "OSGWMarray.h"

#ifndef ARRAY_SINGLE
#include "OSGWMutil.h"
#include "OSGWMtinyvector.h"
#endif

/* base64 coding, needed for hashing */
#ifndef BOOL
#define BOOL    unsigned char
#define FALSE   0
#define TRUE    1
#endif
int                     gl_array_debug1 = 0;

static unsigned char    table_b2a_base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char             table_a2b_base64[] =
{
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    62,
    -1,
    -1,
    -1,
    63,
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    -1,
    -1,
    -1,
    0,
    -1,
    -1, /* Note PAD->0 */
    -1,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    -1,
    -1,
    -1,
    -1,
    -1
};

#define BASE64_PAD  '='

/* */

int GEOMARK_base64_encode(unsigned char *in, char *out, int n)
{
#ifdef USE_SSLEAY_BASE64
    int m;
    m = EVP_EncodeBlock(out, in, n);
    return m;
#else
    char            *ascii_data;
    unsigned char   *bin_data;
    int             leftbits = 0;
    unsigned char   this_ch;
    unsigned int    leftchar = 0;
    int             bin_len;

    ascii_data = out;
    bin_data = in;
    bin_len = n;

    for(; bin_len > 0; bin_len--, bin_data++)
    {
        /* Shift the data into our buffer */
        leftchar = (leftchar << 8) | *bin_data;
        leftbits += 8;

        /* See if there are 6-bit groups ready */
        while(leftbits >= 6)
        {
            this_ch = (leftchar >> (leftbits - 6)) & 0x3f;
            leftbits -= 6;
            *ascii_data++ = table_b2a_base64[this_ch];
        }
    }

    if(leftbits == 2)
    {
        *ascii_data++ = table_b2a_base64[(leftchar & 3) << 4];
        *ascii_data++ = BASE64_PAD;
        *ascii_data++ = BASE64_PAD;
    }
    else if(leftbits == 4)
    {
        *ascii_data++ = table_b2a_base64[(leftchar & 0xf) << 2];
        *ascii_data++ = BASE64_PAD;
    }

    return (int) (((char *) ascii_data) - out);
#endif
}

/* */
int GEOMARK_base64_decode(char *in, unsigned char *out, int n)
{
#ifdef USE_SSLEAY_BASE64
    int m;
    m = EVP_DecodeBlock(out, in, n);
    return m;
#else
    char            *ascii_data;
    unsigned char   *bin_data;
    int             leftbits = 0;
    unsigned char   this_ch;
    unsigned int    leftchar = 0;
    int             npad = 0;
    int             ascii_len, bin_len;

    bin_data = out;
    ascii_data = in;
    ascii_len = n;

    bin_len = 0;

    for(; ascii_len > 0; ascii_len--, ascii_data++)
    {
        /*
		** XXXX I don't do any checks on the chars, ignoring
		** any illegal chars. Hope this is correct...
		*/
        this_ch = (*ascii_data & 0x7f);
        if(this_ch == BASE64_PAD)
            npad++;
        this_ch = table_a2b_base64[(*ascii_data) & 0x7f];
        if(this_ch == -1)
            continue;

        /*
		** Shift it in on the low end, and see if there's
		** a byte ready for output.
		*/
        leftchar = (leftchar << 6) | (this_ch);
        leftbits += 6;
        if(leftbits >= 8)
        {
            leftbits -= 8;
            *bin_data++ = (leftchar >> leftbits) & 0xff;
            leftchar &= ((1 << leftbits) - 1);
            bin_len++;
        }
    }

    /* and remove any padding */
    bin_len -= npad;
    return bin_len;
#endif
}

/* */
int GEOMARK_calcBase64Len(int numOctets)
{
    int r, l;
    l = (int) ceil((numOctets / 3.) * 4.);

    /* find next larger value divideable by 4 */
    r = l % 4;
    l = (l - r) + 4;
    return l;
}

/* instance counters */
//int ListArray::numAllocated=0;
//int ListArray::numFreed=0;
int GEOMARK_BitArray::  numAllocated = 0;
int GEOMARK_BitArray::  numFreed = 0;
int GrowBitArray::      numAllocated = 0;
int GrowBitArray::      numFreed = 0;
int FixedArray::        numAllocated = 0;
int FixedArray::        numFreed = 0;
int GrowArray::         numAllocated = 0;
int GrowArray::         numFreed = 0;

/* */

void printMemoryConsumptionInfo(void)
{
    //printf("ListArray::numAllocated=%d\n",ListArray::numAllocated);
    //printf("ListArray::numFreed=%d\n",ListArray::numFreed);
    printf("GEOMARK_BitArray::numAllocated=%d\n",
           GEOMARK_BitArray::numAllocated);
    printf("GEOMARK_BitArray::numFreed=%d\n", GEOMARK_BitArray::numFreed);
    printf("GrowBitArray::numAllocated=%d\n", GrowBitArray::numAllocated);
    printf("GrowBitArray::numFreed=%d\n", GrowBitArray::numFreed);
    printf("FixedArray::numAllocated=%d\n", FixedArray::numAllocated);
    printf("FixedArray::numFreed=%d\n", FixedArray::numFreed);
    printf("GrowArray::numAllocated=%d\n", GrowArray::numAllocated);
    printf("GrowArray::numFreed=%d\n", GrowArray::numFreed);
}

/* */
void dumpGrowArrayInt(GrowArray *ga)
{
    int i, n, element;

    printf("GrowArray-contents: ");
    n = ga->getNumElements();
    for(i = 0; i < n; i++)
    {
        element = *((int *) ga->getElement(i));
        printf("%d ", element);
    }

    printf("\n");
}

#if 0

/* a list-like array */
void *ListArray::setElement(int index, void *element)
{ }

/* */
void *ListArray::getElement(int index)
{ }

/* */
void *ListArray::append(void *element)
{ }

/* */
int ListArray::getNumElements(void)
{
    return tailIndex_;
}

/* */
int ListArray::empty(void)
{
    return 0;
}

/* */
int ListArray::copy(GrowArray *a, BOOL emptyList = TRUE, BOOL addFragments)
{
    return 0;
}

/* */
void *ListArray::insertElement(void *element)
{ }

/* */
void *ListArray::insertElementSorted(void *element,
                                     int (*cmp) (const void *, const void *))
{ }

/* */
void *ListArray::removeElement(int index)
{ }

/* */
void ListArray::compress(void)
{ }
#endif

/* a simple (fixed) bit array */
int GEOMARK_BitArray::getBit(int index)
{
    assert((index >= 0) && (index <= maxIndex_));

    int charPos = index >> 3;
    int bitPos = index - (charPos << 3);                    /* faster than % ? */
    return (int) ((bits_[charPos] & (0x80 >> bitPos)) > 0);
}

/* */
int GEOMARK_BitArray::setBit(int index, unsigned char value)
{
    int charPos = index >> 3;
    int bitPos = index - (charPos << 3);                    /* faster than % ? */
    if(value == 0)
    {
        bits_[charPos] &= ~(0x80 >> bitPos);
    }
    else
    {
        bits_[charPos] |= (0x80 >> bitPos);
    }

    return 0;
}

/* */
int GEOMARK_BitArray::copyContents(GEOMARK_BitArray *from)
{
    if(maxIndex_ == from->maxIndex_)
    {
        memcpy(bits_, from->bits_, numAllocedBytes_);
        return 0;
    }
    else
        return -1;
}

/* */
int GEOMARK_BitArray::getValue(int startIndex, int numBits, int *value)
{
    int index, i, val = 0;
    int charPos = startIndex >> 3;
    int bitPos = startIndex - (charPos << 3);               /* faster than % ? */

    assert((startIndex >= 0) && (startIndex <= maxIndex_));

    i = 0;
    index = startIndex;
    while((i < numBits) && (index <= maxIndex_))
    {
        val |= ((bits_[charPos] & (0x80 >> bitPos)) > 0);
        if(i < numBits - 1)
            val <<= 1;
        i++;
        if(bitPos < 7)
            bitPos++;
        else
        {
            charPos++;
            bitPos = 0;
        }

        index++;
    }

    *value = val;
    return i;
}

/* */
int GEOMARK_BitArray::getValue(int startIndex, int numBits,
                               GEOMARK_UINT64 *value)
{
    int             index, i;
    int             charPos = startIndex >> 3;
    int             bitPos = startIndex - (charPos << 3);   /* faster than % ? */
    GEOMARK_UINT64  val = 0;

    assert((startIndex >= 0) && (startIndex <= maxIndex_));

    i = 0;
    index = startIndex;
    while((i < numBits) && (index <= maxIndex_))
    {
        val |= ((bits_[charPos] & (0x80 >> bitPos)) > 0);
        if(i < numBits - 1)
            val <<= 1;
        i++;
        if(bitPos < 7)
            bitPos++;
        else
        {
            charPos++;
            bitPos = 0;
        }

        index++;
    }

    *value = val;
    return i;
}

/* */
int GEOMARK_BitArray::setValue(int startIndex, unsigned char *bytes, int numBits)
{
    int val, index, i, n, p;
    int charPos = startIndex >> 3;
    int bitPos = startIndex - (charPos << 3);               /* faster than % ? */
    int bc;

    assert((startIndex >= 0) && (startIndex <= maxIndex_));

    i = 0;
    bc = 0;
    p = 0;
    n = numBits;
    index = startIndex;
    while((i < n) && (index < maxIndex_))
    {
        val = ((bytes[bc] & (0x80 >> p)) > 0);

        if(val == 0)
        {
            bits_[charPos] &= ~(0x80 >> bitPos);
        }
        else
        {
            bits_[charPos] |= (0x80 >> bitPos);
        }

        i++;
        if(bitPos < 7)
            bitPos++;
        else
        {
            charPos++;
            bitPos = 0;
        }

        if(p < 7)
            p++;
        else
        {
            p = 0;
            bc++;
        }

        index++;
    }

    return i;
}

/* */
int GEOMARK_BitArray::setValue(int startIndex, int value, int numBits)
{
    int val, index, i, n;
    int charPos = startIndex >> 3;
    int bitPos = startIndex - (charPos << 3);               /* faster than % ? */
    int maxBit = (1 << (numBits - 1));

    assert((startIndex >= 0) && (startIndex <= maxIndex_));

    i = 0;
    n = numBits;
    index = startIndex;
    while((i < n) && (index <= maxIndex_))
    {
        val = ((value & (maxBit >> i)) > 0);

        if(val == 0)
        {
            bits_[charPos] &= ~(0x80 >> bitPos);
        }
        else
        {
            bits_[charPos] |= (0x80 >> bitPos);
        }

        i++;
        if(bitPos < 7)
            bitPos++;
        else
        {
            charPos++;
            bitPos = 0;
        }

        index++;
    }

    return i;
}

/* */
int GEOMARK_BitArray::setValue(int startIndex, GEOMARK_UINT64 value, int numBits)
{
    int             val, index, i, n;
    int             charPos = startIndex >> 3;
    int             bitPos = startIndex - (charPos << 3);   /* faster than % ? */
    GEOMARK_UINT64  maxBit = (1 << (numBits - 1));

    assert((startIndex >= 0) && (startIndex <= maxIndex_));

    i = 0;
    n = numBits;
    index = startIndex;
    while((i < n) && (index <= maxIndex_))
    {
        val = ((value & (maxBit >> i)) > 0);

        if(val == 0)
        {
            bits_[charPos] &= ~(0x80 >> bitPos);
        }
        else
        {
            bits_[charPos] |= (0x80 >> bitPos);
        }

        i++;
        if(bitPos < 7)
            bitPos++;
        else
        {
            charPos++;
            bitPos = 0;
        }

        index++;
    }

    return i;
}

/* growable bit array */
int GrowBitArray::getBit(int index)
{
    unsigned char   *adr;

    assert((index >= 0) && (index <= maxIndex_));

    int chunkIndex = index >> chunkSize2PowerBits_;
    int tmp = index - (chunkIndex << chunkSize2PowerBits_);
    int chunkByteIndex = tmp >> 3;
    int bitPos = tmp - (chunkByteIndex << 3);

    adr = (unsigned char *) ptrElements_[chunkIndex] + chunkByteIndex;
    return (int) ((*adr & (0x80 >> bitPos)) > 0);
}

/* */
int GrowBitArray::setBit(int index, unsigned char value)
{
    unsigned char   *adr;

    assert(index >= 0);

    if(index > maxIndex_)
    {
        /* allocate new chunk(s) */
        int nc = ((index - maxIndex_) >> chunkSize2PowerBits_) + 1;

        /* BUGFIX: > instead of >= */
        if(numChunks_ + nc > MAX_NUM_CHUNKS)
            return -1;

        for(int i = 0; i < nc; i++)
        {
            ptrElements_[numChunks_ + i] = (unsigned char *) malloc(chunkSize_);
            memset(ptrElements_[numChunks_ + i], 0, chunkSize_);
        }

        numChunks_ += nc;
        maxIndex_ += nc << chunkSize2PowerBits_;
    }

    int chunkIndex = index >> chunkSize2PowerBits_;
    int tmp = index - (chunkIndex << chunkSize2PowerBits_);
    int chunkByteIndex = tmp >> 3;
    int bitPos = tmp - (chunkByteIndex << 3);

    adr = (unsigned char *) ptrElements_[chunkIndex] + chunkByteIndex;
    if(value == 0)
        *adr &= ~(0x80 >> bitPos);
    else
        *adr |= (0x80 >> bitPos);

    return 0;
}

/* */
int GrowBitArray::getValue(int startIndex, int numBits, int *value)
{
    int index, i, val = 0;

    assert((startIndex >= 0) && (startIndex <= maxIndex_));

    i = 0;
    index = startIndex;
    while((i < numBits) && (index <= maxIndex_))
    {
        val |= getBit(index);

        if(i < numBits - 1)
            val <<= 1;
        i++;
        index++;
    }

    *value = val;
    return i;
}

/* */
int GrowBitArray::setValue(int startIndex, unsigned char *bytes, int numBits)
{
    unsigned char   val;
    int             index, i, p, n, bc;
    BOOL            setFailed = FALSE;

    assert((startIndex >= 0));                              //&& (startIndex<=maxIndex_));
    i = 0;
    bc = 0;
    p = 0;
    n = numBits;
    index = startIndex;
    while((i < n) && (!setFailed))
    {
        val = ((bytes[bc] & (0x80 >> p)) > 0);
        if(setBit(index, val) == -1)
            setFailed = TRUE;
        index++;
        i++;
        if(p < 7)
            p++;
        else
        {
            p = 0;
            bc++;
        }
    }

    return i;
}

/* */
int GrowBitArray::setValue(int startIndex, int value, int numBits)
{
    unsigned char   val;
    int             index, i, n;
    BOOL            setFailed = FALSE;
    int             maxBit = (1 << (numBits - 1));

    assert((startIndex >= 0));                              //&& (startIndex<=maxIndex_));
    i = 0;
    n = numBits;
    index = startIndex;
    while((i < n) && (!setFailed))
    {
        val = ((value & (maxBit >> i)) > 0);
        if(setBit(index, val) == -1)
            setFailed = TRUE;
        index++;
        i++;
    }

    return i;
}

/* non resizable array */

/*
  - random access (read & write)
*/
void FixedArray::sort(int from, int to, int (*cmp) (const void *, const void *))
{
    void    *ptr;

    if(from == -1)
        from = 0;
    if(to == -1)
        to = size_ - 1;

    /* BUGFIX */
    assert(((from >= 0) && (from <= to) && (to < size_)));

    if(from < to)
    {
        ptr = (void *) &((unsigned char *) ptrElements_)[from * sizeElement_];

        qsort(ptr, to - from + 1, sizeElement_, cmp);
    }
}

/* */
void *FixedArray::setElement(int index, void *element)
{
    void    *pos;

    if((index < 0) || (index > maxIndex_))
        return NULL;
    pos = (void *) &((unsigned char *) ptrElements_)[index * sizeElement_];
    memcpy(pos, element, sizeElement_);
    return pos;
}

/* */
void *FixedArray::getElement(int index)
{
    if((index < 0) || (index > maxIndex_))
        return NULL;
    return (void *) &((unsigned char *) ptrElements_)[index * sizeElement_];
}

/* */
void *FixedArray::appendElement(void *element)
{
    void    *adr = setElement(tailIndex_, element);
    if(adr != NULL)
        tailIndex_++;
    return adr;
}

/* growable array */
void *GrowArray::setElement(int index, void *element)
{
    void    *adr;

    if(index > maxIndex_)
    {
        /* allocate new chunk(s) */
        int nc = ((index - maxIndex_) >> chunkSize2Power_) + 1;

        /* BUG FIXED: > instead of >= */
        if(numChunks_ + nc > MAX_NUM_CHUNKS)
            return NULL;

        for(int i = 0; i < nc; i++)
        {
            ptrElements_[numChunks_ + i] = (void *) malloc(chunkSize_ * sizeElement_);
            memset(ptrElements_[numChunks_ + i], 0, chunkSize_ * sizeElement_);
        }

        numChunks_ += nc;

        /* find chunk */
        int chunkIndex = index >> chunkSize2Power_;

        //int elementIndex=index % chunkSize_;
        int elementIndex = index - (chunkIndex << chunkSize2Power_);

        adr = (void *) &
            ((unsigned char *) ptrElements_[chunkIndex])
                [elementIndex * sizeElement_];
        memcpy(adr, element, sizeElement_);
        maxIndex_ += chunkSize_;
    }
    else
    {
        /* find chunk */
        int chunkIndex = index >> chunkSize2Power_;

        //int elementIndex=index % chunkSize_;
        int elementIndex = index - (chunkIndex << chunkSize2Power_);
        adr = (void *) &
            ((unsigned char *) ptrElements_[chunkIndex])
                [elementIndex * sizeElement_];

        memcpy(adr, element, sizeElement_);
    }

    return adr;
}

/* */
void *GrowArray::getElement(int index)
{
    if(index > maxIndex_)
        return NULL;

    /* find chunk */
    int chunkIndex = index >> chunkSize2Power_;

    //int elementIndex=index % chunkSize_;
    int elementIndex = index - (chunkIndex << chunkSize2Power_);

    return (void *) &
        ((unsigned char *) ptrElements_[chunkIndex])
            [elementIndex * sizeElement_];
}

/* */
void GrowArray::sort(int from, int to, int (*cmp) (const void *, const void *))
{
    void    *h, *x;

    if(from == -1)
        from = 0;
    if(to == -1)
        to = tailIndex_ - 1;

    if(gl_array_debug1)
    {
        fprintf(stderr, "from,to,tailIndex_=%d,%d,%d\n", from, to, tailIndex_);
    }

    /* BUGFIX */
    assert((from >= 0) && (from <= to) && (to < tailIndex_));

    if(from < to)
    {
        h = (void *) malloc(sizeElement_);
        qsort_(from, to, h, cmp);
        GEOMARK_FREE(h);
        h = NULL;
    }
}

/* */
void *GrowArray::appendElement(void *element)
{
    void    *adr = setElement(tailIndex_, element);
    if(adr != NULL)
        tailIndex_++;
    return adr;
}

/* */
int GrowArray::deleteTailElement(void)
{
    if(tailIndex_ > 0)
    {
        tailIndex_--;
        return 0;
    }
    else
        return -1;
}

/*
procedure quicksort (a : feld; lo, hi : integer);
{ lo ist der unterste Index, hi ist der oberste Index des
  zu sortierenden (Teil-)Feldes a }
var i, j, h, x : integer;
begin
  i:=lo; j:=hi;
  x:=a[(lo+hi) div 2];
  repeat
    while a[i]<x do i:=i+1; 
    while x<a[j] do j:=j-1;
    if i<=j then
    begin
      h:=a[i]; a[i]:=a[j]; a[j]:=h;
      i:=i+1; j:=j-1;
    end
  until i>j;
  if lo<j then quicksort(a, lo, j);
  if i<hi then quicksort(a, i, hi);
end;
*/
void GrowArray::qsort_(int lo, int hi, void *h,
                       int (*cmp) (const void *, const void *))
{
    if(gl_array_debug1)
    {
        fprintf(stderr, "->qsort %d,%d\n", lo, hi);
    }

    int     i, j, rv;
    void    *x, *ai, *aj;

    i = lo;
    j = hi;

    //x=getElement((lo+hi) >> 1);
    /* BUGFIX */
    x = getElement(lo);
    do
    {
        // while a[i]<x do i:=i+1;
        while((*cmp) (ai = getElement(i), x) == -1)
        {
            i++;
        }

        // while x<a[j] do j:=j-1;
        while((*cmp) (x, aj = getElement(j)) == -1)
        {
            j--;
        }

        if(i <= j)
        {
            // h:=a[i]; a[i]:=a[j]; a[j]:=h;
            memcpy(h, ai, sizeElement_);
            memcpy(ai, aj, sizeElement_);
            memcpy(aj, h, sizeElement_);
            i++;
            j--;
        }
    } while(i <= j);

    if(lo < j)
        qsort_(lo, j, h, cmp);
    if(i < hi)
        qsort_(i, hi, h, cmp);
}

/* */
int HashArray::searchElement(void *key, void **value)
{
    BOOL    itemExists;

    itemExists = hfind(hashTable_, (ub1 *) key, (ub4) sizeElement_);
    if(itemExists)
    {
        if(value != NULL)
            *value = hstuff(hashTable_);
        return 0;
    }
    else
        return -1;
}

/* */
int HashArray::insertElement(void *key, void *value = NULL)
{
    BOOL    isNew;
    void    *adr;

    adr = keyValues_->appendElement(key);

    isNew = hadd(hashTable_, (ub1 *) adr, (ub4) sizeElement_, value);

    if(isNew)
        return 0;
    else
    {
        keyValues_->deleteTailElement();                    /* stupid but most simple solution */
        return -1;
    }
}

/* */
void HashArray::dump(void)
{
    hstat(hashTable_);
}

/* */
int HashDelArray::searchElement(void *key, int *value)
{
    BOOL                itemExists;
    HashDelArrayElement *element;

    itemExists = hfind(hashTable_, (ub1 *) key, (ub4) sizeElement_);
    if(itemExists)
    {
        //*value=hstuff(hashTable_);
        element = (HashDelArrayElement *) hstuff(hashTable_);
        *value = element->value;
        return 0;
    }
    else
        return -1;
}

/* */
int HashDelArray::insertElement(void *key, int value)
{
    BOOL                itemExists;
    void                *adr;
    int                 numFreedPositions, index;
    HashDelArrayElement element;

    int                 i;

    //HashDelArrayElement *checkAdr;
    memcpy(element.key, key, sizeElement_);

    /* check if element is not contained */
    itemExists = hfind(hashTable_, (ub1 *) key, (ub4) sizeElement_);

    if(itemExists)
        return -1;

    numFreedPositions = freedPositions_->getNumElements();

    if(numFreedPositions > 0)
    {
        index = *((int *) freedPositions_->getElement(numFreedPositions - 1));
        element.index = index;
        element.value = value;
        adr = keyValues_->setElement(index, &element);
        freedPositions_->deleteTailElement();
    }
    else
    {
        element.index = keyValues_->getNumElements();
        element.value = value;
        adr = keyValues_->appendElement(&element);
    }

    hadd(hashTable_, (ub1 *) adr, (ub4) sizeElement_, adr);

    return 0;
}

/* */
int HashDelArray::deleteElement(void *key)
{
    BOOL                itemExists;
    HashDelArrayElement *element;
    int                 index;

    itemExists = hfind(hashTable_, (ub1 *) key, (ub4) sizeElement_);
    if(itemExists)
    {
        element = (HashDelArrayElement *) hstuff(hashTable_);
        hdel(hashTable_);
        index = element->index;

        freedPositions_->appendElement(&index);

        //dumpGrowArrayInt(freedPositions_);
    }
    else
        return -1;
    return 0;
}

/* */
void HashDelArray::dump(void)
{
    int                 i, n;
    HashDelArrayElement *element;

    hstat(hashTable_);

    n = keyValues_->getNumElements();
    printf("# key values=%d\n", n);
    printf("keyValues_=\n");

    for(i = 0; i < n; i++)
    {
        element = (HashDelArrayElement *) keyValues_->getElement(i);
        printf("element no. %d, key=(%d,%d,%d,%d,%d,%d), index=%d, value=%d\n",
           i, element->key[0], element->key[1], element->key[2],
               element->key[3], element->key[4], element->key[5],
               element->index, element->value);
    }

    printf("# freed positions=%d\n", freedPositions_->getNumElements());
    printf("freedPositions_=\n");
    dumpGrowArrayInt(freedPositions_);
    printf("HashDelArray-dump finished\n");
}

/* */
int compareInts(const void *e1, const void *e2)
{
    if(*((int *) e1) < *((int *) e2))
        return -1;
    else if(*((int *) e1) == *((int *) e2))
        return 0;
    else
        return 1;
}

/* */
int compareDoubles(const void *e1, const void *e2)
{
    if(*((double *) e1) < *((double *) e2))
        return -1;
    else if(*((double *) e1) == *((double *) e2))
        return 0;
    else
        return 1;
}

/* */
int compareFloats(const void *e1, const void *e2)
{
    if(*((float *) e1) < *((float *) e2))
        return -1;
    else if(*((float *) e1) == *((float *) e2))
        return 0;
    else
        return 1;
}

#ifndef ARRAY_SINGLE

/* */
int compareVectorsByLengthAsc(const void *e1, const void *e2)
{
    double  a1, a2;

    //a1=abs(*(TinyVector*) e1);a2=abs(*(TinyVector*) e2);
    a1 = TINYVECTOR_FAST_ABS((*(TinyVector *) e1));
    a2 = TINYVECTOR_FAST_ABS((*(TinyVector *) e2));

    if(a1 < a2)
        return -1;
    else if(a1 == a2)
        return 0;
    else
        return 1;
}

/* */
int compareVectorsByLengthDesc(const void *e1, const void *e2)
{
    double  a1, a2;

    //a1=abs(*(TinyVector*) e1);a2=abs(*(TinyVector*) e2);
    a1 = TINYVECTOR_FAST_ABS((*(TinyVector *) e1));
    a2 = TINYVECTOR_FAST_ABS((*(TinyVector *) e2));

    if(a1 > a2)
        return -1;
    else if(a1 == a2)
        return 0;
    else
        return 1;
    return 0;
}

/* */
int compareVectorsByZAsc(const void *e1, const void *e2)
{
    double  a1, a2;

    //a1=(*(TinyVector*) e1)[2];a2=(*(TinyVector*) e2)[2];
    a1 = ((TinyVector *) e1)->v_[2];
    a2 = ((TinyVector *) e2)->v_[2];

    if(a1 < a2)
        return -1;
    else if(a1 == a2)
        return 0;
    else
        return 1;
}

/* */
int compareVectorsByZDesc(const void *e1, const void *e2)
{
    double  a1, a2;

    //a1=(*(TinyVector*) e1)[2];a2=(*(TinyVector*) e2)[2];
    a1 = ((TinyVector *) e1)->v_[2];
    a2 = ((TinyVector *) e2)->v_[2];

    if(a1 > a2)
        return -1;
    else if(a1 == a2)
        return 0;
    else
        return 1;
}
#endif
#endif
