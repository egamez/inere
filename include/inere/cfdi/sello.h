#ifndef EDISSON_SELLO_H_INCLUDED
#define EDISSON_SELLO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

char *
sello(const char *keyfile, const char *digest, const unsigned char *cadena, const int verbose);

#ifdef __cplusplus
}
#endif

#endif /* EDISSON_SELLO_H_INCLUDED */
