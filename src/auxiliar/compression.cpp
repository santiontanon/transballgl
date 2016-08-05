#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

#include"compression.h"

#define CHUNK 262144

bool compress(char *source,int len,FILE *dest)
{
	int orig_position=0;
    int ret, flush;
    unsigned have;
    z_stream strm;
    char in[CHUNK];
    char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        return false;

    /* compress until end of file */
    do {

		{
			int i;
			for(i=0;i+orig_position<len && i<CHUNK;i++) in[i]=source[i+orig_position];
			strm.avail_in=i;
			orig_position+=i;
		}

        flush = (orig_position>=len) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = (unsigned char *)in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = (unsigned char *)out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                return false;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */
        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */
    /* clean up and return */
    (void)deflateEnd(&strm);
    return true;
} /* compress */ 

	
char *decompress(FILE *source,unsigned *len)
{
    int ret;
    unsigned have;
    z_stream strm;
    char in[CHUNK];
    char out[CHUNK];

	char *result=0;
	unsigned result_length=0;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return 0;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = (unsigned int)fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
			if (result!=0) delete []result;
            return 0;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = (unsigned char *)in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = (unsigned char *)out;

            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
				if (result!=0) delete []result;
                return 0;
            }

            have = CHUNK - strm.avail_out;

			if (have>0) {
				unsigned int i;
				char *new_result = new char[result_length+have];

				for(i=0;i<result_length;i++) new_result[i]=result[i];

				if (result!=0) delete []result;
				result=new_result;

				for(i=0;i<have;i++) new_result[i+result_length]=out[i];
				result_length+=have;
			} // if 
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
	*len=result_length;
	return result;
} /* decompress */ 


