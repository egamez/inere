#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <inere/extrae_fecha.h>

int
main(int argc, char *argv[])
{
  int res = 0;
  char day[3];
  char month[3];
  char year[3];

  if ( argc < 2 ) {
    fprintf(stderr, "Error. Sintax:\n\t%s PRECLAVE\n", argv[0]);
    return 1;
  }

  res = extrae_fecha(argv[1], day, month, year, 1);
  if ( res == -1 ) {
    printf("La longitud de la preclave no es la apropiada, no se realizo la extraccion de la fecha.\n");
    return 1;

  } else if ( res == -2 ) {
    printf("No se encontraron digitos suficientes para extraer la fecha de la clave proporcionada. No se realizará ninguna extracción.\n");
    return 2;

  } else if ( res == -3 ) {
    printf("La cantidad de letras no es suficiente para evaluar la fecha que debería extraerse de la clave. No se realizará extracción alguna.\n");
    return 3;

  } else if ( res == -4 ) {
    printf("La conversion no pudo ser realizada por un suceso inesperado. No hay más información.\n");
    return 4;

  } else if ( res == -5 ) {
    printf("No fue posible completar la conversión. La cantidad de campos que se requieren para la extracción no fueron completados.\n");
    return 5;
  }

  printf("Resultado:\nextrae = %d\ndia = [%s]\nmes = [%s]\naño = [%s]\n", res, day, month, year);

  return 0;
}
