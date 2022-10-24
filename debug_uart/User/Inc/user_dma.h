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
#if !defined USER_DMA_H
#define USER_DMA_H

#define AVAILABLE_CHANNELS              8

#define USING_1_CHANNEL
// #define USING_2_CHANNEL
// #define USING_3TO4_CHANNEL
// #define USING_5TO8_CHANNEL


typedef struct dma_controller
{
    uint8_t cur_ch_num; /* channel C */

}dma_controller_t;

void DMA_PreInit(void);

#endif
