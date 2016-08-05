#ifndef BRAIN_COMPRESSION
#define BRAIN_COMPRESSION

bool compress(char *orig,int len,FILE *fp_out);
char *decompress(FILE *orig,unsigned *len);

#endif