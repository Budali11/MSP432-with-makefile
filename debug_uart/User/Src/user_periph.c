/**
 * Copyright 2022 Budali11
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "user_periph.h"

extern void Printf(const char *str, ...);

extern preFunc_t __preinit_start[];
extern preFunc_t __preinit_end[];

static int do_one_initcall(preFunc_t fn)
{
    /* exert the functions */
    int ret = fn();

    if(ret != 0)
    {
        Printf("function %s returns a error %d", getName(fn), ret);
        return -1;
    }
    return 0;
}


int Peripheral_PreInit(void)
{
    int ret = 0;
    preFunc_t inits = NULL;

    /* calculate the counts of initcall functions */
    // unsigned int num_inits = (__preinit_end - __preinit_start)/sizeof(preFunc_t);
    unsigned int num_inits = (unsigned int)(__preinit_end - __preinit_start);
    
    /* exert the functions */
    for(unsigned int i = 0; i < num_inits; i++)
    {
        inits = __preinit_start[i];
        ret += do_one_initcall(inits);
    }

    /* print debug information */
    if(ret != 0)
        Printf("\r\nInitialization occurs some error.\r\n");
    else
        Printf("\r\nInitialization succeed.\r\n");
    
    return ret;
}

