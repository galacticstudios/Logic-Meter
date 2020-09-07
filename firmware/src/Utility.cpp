

#include <cstdio>
#include <math.h>
extern "C"
{
#include "definitions.h"
}
#include "Utility.h"

laString laString_CreateFromUTF8Buffer(const char *chr, GFXU_FontAsset* fnt)
{
	laString str = { 0, 0, 0, 0, LA_STRING_NULLIDX };
    uint32_t len = 0;
    uint32_t src, dest;
    
    if (laContext_GetActive() == NULL || chr == NULL)
    {
        laString_Initialize(&str);
        return str;
    }
    
    len = strlen(chr);
    
    if (laString_Allocate(&str, len ? len : 1) == LA_FAILURE)
    {
        __builtin_software_breakpoint();
    }

    if(str.capacity == 0)
        return str;
    
    for (src = 0, dest = 0; chr[src]; ++src, ++dest)
    {
        unsigned char ch = (unsigned char) chr[src];
        if (ch < 128)
            str.data[dest] = (GFXU_CHAR) ch;
        else if (ch < 0xe0)
        {
            str.data[dest] = ((ch & 0x1f) << 6) | (chr[++src] & 0x3f);
        }
        else if (ch < 0xf0)
        {
            str.data[dest] = ((ch & 0x0f) << 12) | ((chr[src + 1] & 0x3f) << 6) | (chr[src + 2] & 0x3f);
            src += 2;
        }
        else if (ch < 0xf8)
        {
            str.data[dest] = ((ch & 0x0f) << 18) | ((chr[src + 1] & 0x3f) << 12) | 
                    ((chr[src + 2] & 0x3f) << 6) | (chr[src + 3] & 0x3f);
            src += 3;
        }
    }

    str.data[dest] = '\0';
 
    str.font = fnt;
    str.table_index = LA_STRING_NULLIDX;
    
    return str;
}

char engResult[100];

std::string eng(double value, int digits, const char *units)
{
    static const char *prefix[] = {
        "y", "z", "a", "f", "p", "n", UTF8_MU, "m", "",
        "k", "M", "G", "T", "P", "E", "Z", "Y"
        }; 
    /* Smallest power of ten for which there is a prefix defined.
       If the set of prefixes will be extended, change this constant
       and update the table "prefix". */
    static const int PREFIX_START = (-24);
    static const int PREFIX_END = (PREFIX_START + (int) ((sizeof(prefix) / sizeof(char *) - 1 ) * 3));

    int expof10;
    char *res = engResult;

    if (value < 0.)
    {
        *res++ = '-';
        value = -value;
    }
    else if (value == 0.)
    {
        return "0.0";
    }

    expof10 = (int) log10(value);
    if(expof10 > 0)
        expof10 = (expof10 / 3)*3;
    else
        expof10 = (-expof10 + 3) / 3 * (-3); 

    value *= pow(10, -expof10);

    if (value >= 1000.)
    {
        value /= 1000.0; 
        expof10 += 3; 
    }
    else if(value >= 100.0)
        digits -= 2;
    else if(value >= 10.0)
        digits -= 1;

    int decimalDigits = digits - 1;
    const char *unitPrefix = prefix[(expof10 - PREFIX_START) / 3];
    sprintf(res, "%.*f %s%s", decimalDigits, value, unitPrefix, units);
    return engResult;
}

extern "C"
int open(const char *buf, int flags, int mode)
{
    __builtin_software_breakpoint();
}

bool inSprintf;

// We're getting crashes in sprintf. This wrapper sets a flag when we're
// in sprintf so that when a mysterious crash occurs, I can quickly check the 
// flag and see if it's an sprintf crash or something else.
//
// The crash might be memory corruption. In one instance, the buffer sprintf
// was writing to was on the stack and we crashed. I moved the buffer to static
// memory and the crash stopped happening

static char sprintfOut[200];
extern "C"
int __wrap_sprintf(char *str, const char *format, ...)
{
    // Do the sprintf into a static buffer so we can look at it after the crash.
    // Also, initialize the buffer so we can see if sprintf is writing outside
    // of where it should
    memset(sprintfOut, '\xff', sizeof(sprintfOut));
    
    inSprintf = true;
    
    va_list aptr;
    va_start(aptr, format);
    
    int ret = vsprintf(sprintfOut + 10, format, aptr);
    va_end(aptr);
    inSprintf = false;
    
    strcpy(str, sprintfOut + 10);
    
    return ret;
}
