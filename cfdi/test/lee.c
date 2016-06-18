/*
 * Copyright (c) 2014-2016, L3a,
 *                          Enrique Gamez Flores <egamez@edisson.com.mx>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef INERE_CFDI_LEE_CFDI_H_
#include "inere/cfdi/lee_cfdi.h"
#endif

#include <stdio.h>

int
main(int argc, const char *argv[])
{
  int verbose = 0;
  Comprobante_t *cfdi = NULL;
  Concepto_list_t *conceptos = NULL;
  Traslado_list_t *traslado = NULL;
  Retencion_list_t *retencion = NULL;
  InformacionAduanera_list_t *info = NULL;
  MetodoDePago_list_t *metodos = NULL;

  if ( argc < 2 ) {
    fprintf(stderr, "Uso: %s  CFDI\n", argv[0]);
    return 1;
  }

  cfdi = lee_cfdi(argv[1], verbose);
  if ( cfdi == NULL ) {
    fprintf(stderr, "No pudo completarse la operacion de creacion de la representacion del CFDi.\n");
    return 2;
  }

  /* Imprime primero la información del emisor */
  printf("Datos del emisor:\n");
  printf("          R.F.C.: %s\n", cfdi->Emisor->rfc);
  if ( cfdi->Emisor->nombre != NULL ) {
    printf("          Nombre: %s\n", cfdi->Emisor->nombre);
  }

  /* Imprime el domicilio fiscal, si es que existe */
  if ( cfdi->Emisor->DomicilioFiscal != NULL ) {
    printf("Domicilio Fiscal:\n");

    if ( cfdi->Emisor->DomicilioFiscal->calle != NULL ) {
      printf("                           Calle: %s\n", cfdi->Emisor->DomicilioFiscal->calle);
    }

    if ( cfdi->Emisor->DomicilioFiscal->noExterior != NULL ) {
      printf("                 Número exterior: %s\n", cfdi->Emisor->DomicilioFiscal->noExterior);
    }

    if ( cfdi->Emisor->DomicilioFiscal->noInterior != NULL ) {
      printf("                 Número interior: %s\n", cfdi->Emisor->DomicilioFiscal->noInterior);
    }

    if ( cfdi->Emisor->DomicilioFiscal->colonia != NULL ) {
      printf("                         Colonia: %s\n", cfdi->Emisor->DomicilioFiscal->colonia);
    }

    if ( cfdi->Emisor->DomicilioFiscal->localidad != NULL ) {
      printf("                       Localidad: %s\n", cfdi->Emisor->DomicilioFiscal->localidad);
    }

    if ( cfdi->Emisor->DomicilioFiscal->municipio != NULL ) {
      printf("                       Municipio: %s\n", cfdi->Emisor->DomicilioFiscal->municipio);
    }

    if ( cfdi->Emisor->DomicilioFiscal->estado != NULL ) {
      printf("                          Estado: %s\n", cfdi->Emisor->DomicilioFiscal->estado);
    }

    if ( cfdi->Emisor->DomicilioFiscal->pais != NULL ) {
      printf("                            País: %s\n", cfdi->Emisor->DomicilioFiscal->pais);
    }

    if ( cfdi->Emisor->DomicilioFiscal->codigoPostal != NULL ) {
      printf("                   Codigo postal: %s\n", cfdi->Emisor->DomicilioFiscal->codigoPostal);
    }

    if ( cfdi->Emisor->DomicilioFiscal->referencia != NULL ) {
      printf("                 Referencias: %s\n", cfdi->Emisor->DomicilioFiscal->referencia);
    }

  }

  /* Imprime el domicilio de expedicion, en caso de que exista */
  if ( cfdi->Emisor->ExpedidoEn != NULL ) {
    printf("         Expedido en:\n");

    if ( cfdi->Emisor->ExpedidoEn->calle != NULL ) {
      printf("                          Calle: %s\n", cfdi->Emisor->ExpedidoEn->calle);
    }

    if ( cfdi->Emisor->ExpedidoEn->noExterior != NULL ) {
      printf("                 Número exterior: %s\n", cfdi->Emisor->ExpedidoEn->noExterior);
    }

    if ( cfdi->Emisor->ExpedidoEn->noInterior != NULL ) {
      printf("                 Número interior: %s\n", cfdi->Emisor->ExpedidoEn->noInterior);
    }

    if ( cfdi->Emisor->ExpedidoEn->colonia != NULL ) {
      printf("                         Colonia: %s\n", cfdi->Emisor->ExpedidoEn->colonia);
    }

    if ( cfdi->Emisor->ExpedidoEn->localidad != NULL ) {
      printf("                       Localidad: %s\n", cfdi->Emisor->ExpedidoEn->localidad);
    }

    if ( cfdi->Emisor->ExpedidoEn->municipio != NULL ) {
      printf("                       Municipio: %s\n", cfdi->Emisor->ExpedidoEn->municipio);
    }

    if ( cfdi->Emisor->ExpedidoEn->estado != NULL ) {
      printf("                          Estado: %s\n", cfdi->Emisor->ExpedidoEn->estado);
    }

    if ( cfdi->Emisor->ExpedidoEn->pais != NULL ) {
      printf("                            País: %s\n", cfdi->Emisor->ExpedidoEn->pais);
    }

    if ( cfdi->Emisor->ExpedidoEn->codigoPostal != NULL ) {
      printf("                   Codigo postal: %s\n", cfdi->Emisor->ExpedidoEn->codigoPostal);
    }

    if ( cfdi->Emisor->ExpedidoEn->referencia != NULL ) {
      printf("                     Referencias: %s\n", cfdi->Emisor->ExpedidoEn->referencia);
    }

  }

  /* Ahora imprime los datos del receptor */
  printf("Datos del receptor/cliente:\n");
  printf("R.F.C.: %s\n", cfdi->Receptor->rfc);
  if ( cfdi->Receptor->nombre != NULL ) {
    printf("Nombre: %s\n", cfdi->Receptor->nombre);
  }

  /* Imprime el domicilio fiscal, si es que existe */
  if ( cfdi->Receptor->Domicilio != NULL ) {
    printf("Domicilio Fiscal:\n");

    if ( cfdi->Receptor->Domicilio->calle != NULL ) {
      printf("                           Calle: %s\n", cfdi->Receptor->Domicilio->calle);
    }

    if ( cfdi->Receptor->Domicilio->noExterior != NULL ) {
      printf("                 Número exterior: %s\n", cfdi->Receptor->Domicilio->noExterior);
    }

    if ( cfdi->Receptor->Domicilio->noInterior != NULL ) {
      printf("                 Número interior: %s\n", cfdi->Receptor->Domicilio->noInterior);
    }

    if ( cfdi->Receptor->Domicilio->colonia != NULL ) {
      printf("                         Colonia: %s\n", cfdi->Receptor->Domicilio->colonia);
    }

    if ( cfdi->Receptor->Domicilio->localidad != NULL ) {
      printf("                       Localidad: %s\n", cfdi->Receptor->Domicilio->localidad);
    }

    if ( cfdi->Receptor->Domicilio->municipio != NULL ) {
      printf("                       Municipio: %s\n", cfdi->Receptor->Domicilio->municipio);
    }

    if ( cfdi->Receptor->Domicilio->estado != NULL ) {
      printf("                          Estado: %s\n", cfdi->Receptor->Domicilio->estado);
    }

    if ( cfdi->Receptor->Domicilio->pais != NULL ) {
      printf("                            País: %s\n", cfdi->Receptor->Domicilio->pais);
    }

    if ( cfdi->Receptor->Domicilio->codigoPostal != NULL ) {
      printf("                   Codigo postal: %s\n", cfdi->Receptor->Domicilio->codigoPostal);
    }

    if ( cfdi->Receptor->Domicilio->referencia != NULL ) {
      printf("                     Referencias: %s\n", cfdi->Receptor->Domicilio->referencia);
    }

  }

  /* Imprime los productos */
  printf("\n\n\nConceptos:\n\n\n");
  conceptos = cfdi->Conceptos;
  while ( conceptos != NULL ) {
    printf("%4s %-40s %12s %15s\n", conceptos->cantidad, conceptos->descripcion, conceptos->valorUnitario, conceptos->importe);
    /* Verifica que si hay otros nodos suplementarios */
    if ( conceptos->InformacionAduanera != NULL ) {
      info = conceptos->InformacionAduanera;
      while ( info != NULL ) {
	printf("\tAduana: %s, numero: %s, fecha: %s\n", info->aduana, info->numero, info->fecha);
	info = info->next;
      }
    }
    if ( conceptos->CuentaPredial != NULL ) {
      printf("\t\t**CUENTA predial numero: %s\n", conceptos->CuentaPredial->numero);
    }
    conceptos = conceptos->next;
  }


  printf("\n\n\n");
  printf("Impuestos:\n");
  printf("         Desglose de impuestos:\n");

  if ( cfdi->Impuestos->Traslados != NULL ) {
    printf("                     Traslados:\n");

    traslado = cfdi->Impuestos->Traslados;
    while ( traslado != NULL ) {
      printf("                               %4s (%5s): %10s\n", traslado->impuesto, traslado->tasa, traslado->importe);
      traslado = traslado->next;
    }
  }

  if ( cfdi->Impuestos->Retenciones != NULL ) {
    retencion = cfdi->Impuestos->Retenciones;
    printf("                   Retenciones:\n");

    while ( retencion != NULL ) {
      printf("                                       %4s: %10s\n", retencion->impuesto, retencion->importe);
      retencion = retencion->next;
    }
  }

  /* Ahora imprime algunos datos reelevantes */
  printf("\n\n\nTotal: %s\n", cfdi->total);
  printf("\n\n\n");
  printf("Fecha: %s\n", cfdi->fecha);
  if ( cfdi->Complemento != NULL ) {
    if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
      printf("Folio fiscal: %s\n", cfdi->Complemento->TimbreFiscalDigital->UUID);
    } else {
      printf("No hay folio fiscal!\n");
    }
  }

  /* Imprime ahora los metodos de pago */
  metodos = cfdi->MetodosDePago;
  while ( metodos != NULL ) {
    printf("Metodo de pago: %s\n", metodos->metodoDePago);
    metodos = metodos->next;
  }

  termina_cfdi(cfdi);

  return 0;
}
