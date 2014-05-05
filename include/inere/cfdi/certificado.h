#ifndef EDISSON_CERTIFICADO_H_INCLUDED
#define EDISSON_CERTIFICADO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

char *
load_certificate(const char *filename, unsigned char **serial_number, const int verbose);

#ifdef __cplusplus
}
#endif

#endif /* EDISSON_CERTIFICADO_H_INCLUDED */
