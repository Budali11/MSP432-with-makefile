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
//----------------------------------------------------------------------
/**
 * @name    user_nvic.cpp
 * @brief   This file contents nvic functions 
 * 
 */

#include "user_core.h"
#include "gpio.h"
#include "cs.h"
#include "user_uart.h"

extern User_Uart_T yuki;

User_Nvic_T user_nvic_ctrler;


void User_Nvic_T::grouping(uint32_t PriorityGroup)
{
    if(PriorityGroup > 0x07)
    {
        NVIC_SetPriorityGrouping(PriorityGroup >> 8);
        user_nvic_ctrler.priority_group = PriorityGroup >> 8;
    }
    else
    {
        NVIC_SetPriorityGrouping(PriorityGroup);
        user_nvic_ctrler.priority_group = PriorityGroup;
    }
}

void User_Nvic_T::set_up_IRQn(IRQn_Type IRQn, uint16_t priority, bool status)
{
    NVIC_SetPriority(IRQn, priority);
    if(status == true)
    {
        //attach one node to the irq_list
        list_node * pIRQn = user_nvic_ctrler.enable_irq_list;
        list_node * new_node = (list_node *)malloc(sizeof(list_node));
        memset(new_node, 0, sizeof(list_node));
        if(pIRQn == NULL)
        {
            user_nvic_ctrler.enable_irq_list = new_node;
            new_node->IRQn_num = IRQn;
            NVIC_EnableIRQ(IRQn);
            return;
        }
        else if(pIRQn->IRQn_num > IRQn)
        {
            //reflash list
            user_nvic_ctrler.enable_irq_list = new_node;
            new_node->IRQn_num = IRQn;
            new_node->next = pIRQn;
            NVIC_EnableIRQ(IRQn);
            return;
        }
        while(1)
        {
            if ((pIRQn->next == NULL) || (pIRQn->next->IRQn_num > IRQn))
            {
                new_node->next = pIRQn->next;
                pIRQn->next = new_node;
                new_node->IRQn_num = IRQn;
                NVIC_EnableIRQ(IRQn);
                return;
            }
            pIRQn = pIRQn->next;
        }    
    }
    else //disable a irq
    {
        //detach one node from the irq_list
        list_node * pIRQn = user_nvic_ctrler.enable_irq_list;
        for (;(pIRQn->next->IRQn_num != IRQn) && (pIRQn->next != NULL) && (pIRQn != NULL); pIRQn = pIRQn->next);
        //end loop, now pIRQn points to the correct positon
        if(pIRQn->next->IRQn_num == IRQn)
        {
            //disable
            NVIC_DisableIRQ(IRQn);
            list_node * del_node = pIRQn->next;
            pIRQn->next = pIRQn->next->next;
            free(del_node);            
        }
        else
            yuki.send_string("irq not found.\r\n");
        
    }
    
}

uint8_t User_Nvic_T::show_enable_irqs(void)
{
    list_node * pIRQn = user_nvic_ctrler.enable_irq_list;
    char name[16] = {0};
    uint8_t i = 0;
    for (; pIRQn != NULL; pIRQn = pIRQn->next)
    {
        sprintf(name, "The %d", pIRQn->IRQn_num);
        yuki.send_string(name);
        yuki.send_string(" IRQ is enabled.\r\n");
        i++;
    }
    return i;
    
}

