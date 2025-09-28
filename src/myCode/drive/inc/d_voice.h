#ifndef __D_VOICE_H__
#define __D_VOICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"
#include "udcheck/src/core/udc_core.h"


void d_voice_init(void);

extern udc_pack_t udc_voice_pack;

#ifdef __cplusplus
}
#endif

#endif /* __D_VOICE_H__ */
