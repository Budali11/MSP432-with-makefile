// Copyright 2022 Budali11
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if !defined USER_PERIPH_H
#define USER_PERTPH_H

#include "msp.h"
#include <stddef.h>
#include "user_defs.h"
#include <stdlib.h>
#include <string.h>

typedef int (*preFunc_t)(void);

#define preinit(fn) \
    static preFunc_t __preinit_call_##fn __used __attribute__((__section__(".preinitcall"))) = fn

int Peripheral_PreInit(void);


#endif
