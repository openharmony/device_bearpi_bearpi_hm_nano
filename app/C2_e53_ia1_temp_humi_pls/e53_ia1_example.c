/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "E53_IA1.h"

#define TASK_STACK_SIZE 1024 * 8
#define TASK_PRIO 25

static void ExampleTask(void)
{
    int ret;
    E53IA1Data data;

    ret = E53IA1Init();
    if (ret != 0) {
       printf("E53_IA1 Init failed!\r\n");
       return;
    }

    while (1) {
        printf("\r\n=======================================\r\n");
        printf("\r\n*************E53_IA1_example***********\r\n");
        printf("\r\n=======================================\r\n");

        ret = E53IA1ReadData(&data);
        if (ret != 0) {
            printf("E53_IA1 Read Data failed!\r\n");
            return;
        }
        printf("\r\n******************************Lux Value is  %.2f\r\n", data.Lux);
        printf("\r\n******************************Humidity is  %.2f\r\n", data.Humidity);
        printf("\r\n******************************Temperature is  %.2f\r\n", data.Temperature);

        if ((int)data.Lux < 20) {
            LightStatusSet(ON);
        } else {
            LightStatusSet(OFF);
        }

        if (((int)data.Humidity > 70) | ((int)data.Temperature > 35)) {
            MotorStatusSet(ON);
        } else {
            MotorStatusSet(OFF);
        }
        usleep(1000000);
    }
}

static void ExampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "ExampleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)ExampleTask, NULL, &attr) == NULL) {
        printf("Failed to create ExampleTask!\n");
    }
}

APP_FEATURE_INIT(ExampleEntry);