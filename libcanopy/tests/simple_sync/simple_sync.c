#include <canopy.h>

int main(void)
{
    CanopyResultEnum result;
    result = canopy_sync();
    return (result == CANOPY_SUCCESS) ? 0 : -1;
}
