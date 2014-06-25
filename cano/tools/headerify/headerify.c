#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int c;
    printf("static const char * %s[] = {\n\"", argv[1]);
    while ((c = getchar()) != EOF) 
    {
        switch ((char)c) {
            case '"':
                printf("\\\"");
                break;
            case '\\':
                printf("\\\\");
                break;
            case '\n':
                printf("\",\n\"");
                break;
            default:
                putchar(c);
                break;
        }
    }
    printf("\"};\n");
    return 0;
}
