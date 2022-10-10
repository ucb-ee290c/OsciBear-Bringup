
#ifndef __OSCIBEAR_HAL_CLINT_H
#define __OSCIBEAR_HAL_CLINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oscibear.h"

void HAL_CLINT_triggerSoftwareInterrupt(uint32_t hartid);

uint64_t HAL_CLINT_getTime();

void HAL_CLINT_setTimerInterrupt(uint64_t time);

#ifdef __cplusplus
}
#endif

#endif /* __OSCIBEAR_HAL_CLINT_H */
