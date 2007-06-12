#include "libexif/_stdint.h"

int main()
{
  /* libexif assumes that in very many places */
  if (sizeof(unsigned int) < sizeof(uint32_t)) {
    return 1;
  }
  return 0;
}
