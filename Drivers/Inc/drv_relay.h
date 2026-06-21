#ifndef __DRV_RELAY_H
#define __DRV_RELAY_H
#include <stdbool.h>

void relay_init(void);
void relay_unlock(void);
void relay_lock(void);
bool relay_is_unlocked(void);

#endif /* __DRV_RELAY_H */
