#include <strings.h>


unsigned int compare(const char * str1, const uint8_t * str2) {

    while (*str1 && *str2) {
        if (*str1 != *str2) return 0;
        str1++;
        str2++;
    }
    return 1;
}

