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
#include "cmsis_os2.h"
#include "ohos_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "E53_IS1.h"
#include "wifi_connect.h"
#include <dtls_al.h>
#include <mqtt_al.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>

#define CONFIG_WIFI_SSID "BearPi" //修改为自己的WiFi 热点账号

#define CONFIG_WIFI_PWD "BearPi" //修改为自己的WiFi 热点密码

#define CONFIG_APP_SERVERIP "121.36.42.100"

#define CONFIG_APP_SERVERPORT "1883"

#define CONFIG_APP_DEVICEID "60154da604feea02d7f9ae8d_2354566786" //替换为注册设备后生成的deviceid

#define CONFIG_APP_DEVICEPWD "123456789" //替换为注册设备后生成的密钥

#define CONFIG_APP_LIFETIME 60 ///< seconds

#define CONFIG_QUEUE_TIMEOUT (5 * 1000)

#define MSGQUEUE_OBJECTS 16 // number of Message Queue Objects

typedef enum {
    en_msg_cmd = 0,
    en_msg_report,
} en_msg_type_t;

typedef struct {
    char* request_id;
    char* payload;
} cmd_t;

typedef struct {
    osMessageQueueId_t app_msg;
    int connected;
} app_cb_t;
static app_cb_t g_app_cb;

int g_infraredStatus = 0;

static void deal_report_msg(void)
{
    oc_mqtt_profile_service_t service;
    oc_mqtt_profile_kv_t status;

    if (g_app_cb.connected != 1) {
        return;
    }
    service.event_time = NULL;
    service.service_id = "Infrared";
    service.service_property = &status;
    service.nxt = NULL;

    status.key = "Infrared_Status";
    status.value = g_infraredStatus ? "Intrude" : "Safe";
    status.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    status.nxt = NULL;

    oc_mqtt_profile_propertyreport(NULL, &service);
    return;
}

#define FLAGS_MSK1 0x00000001U

osEventFlagsId_t g_eventFlagsId;

static void BeepAlarm(char* arg)
{
    (void)arg;
    osEventFlagsSet(g_eventFlagsId, FLAGS_MSK1);
}

static int CloudMainTaskEntry(void)
{
    uint32_t ret;
    WifiConnect(CONFIG_WIFI_SSID, CONFIG_WIFI_PWD);
    dtls_al_init();
    mqtt_al_init();
    oc_mqtt_init();

    g_app_cb.app_msg = osMessageQueueNew(MSGQUEUE_OBJECTS, 10, NULL);
    if (NULL == g_app_cb.app_msg) {
        printf("Create receive msg queue failed");
    }
    oc_mqtt_profile_connect_t connect_para;
    (void)memset(&connect_para, 0, sizeof(connect_para));

    connect_para.boostrap = 0;
    connect_para.device_id = CONFIG_APP_DEVICEID;
    connect_para.device_passwd = CONFIG_APP_DEVICEPWD;
    connect_para.server_addr = CONFIG_APP_SERVERIP;
    connect_para.server_port = CONFIG_APP_SERVERPORT;
    connect_para.life_time = CONFIG_APP_LIFETIME;
    connect_para.rcvfunc = NULL;
    connect_para.security.type = EN_DTLS_AL_SECURITY_TYPE_NONE;
    ret = oc_mqtt_profile_connect(&connect_para);
    if ((ret == (int)en_oc_mqtt_err_ok)) {
        g_app_cb.connected = 1;
        printf("oc_mqtt_profile_connect succed!\r\n");
    } else {
        printf("oc_mqtt_profile_connect faild!\r\n");
    }

    E53IS1Init();
    ret = E53IS1ReadData(BeepAlarm);
    if (ret != 0) {
        printf("E53_IS1 Read Data failed!\r\n");
        return;
    }
    deal_report_msg();
    while (1) {
        osEventFlagsWait(g_eventFlagsId, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
        BeepStatusSet(ON);
        g_infraredStatus = 1;
        deal_report_msg();

        osDelay(500);

        BeepStatusSet(OFF);
        g_infraredStatus = 0;
        deal_report_msg();
    }
    return 0;
}

static void IotMainTaskEntry(void)
{
    g_eventFlagsId = osEventFlagsNew(NULL);
    if (g_eventFlagsId == NULL) {
        printf("Failed to create EventFlags!\n");
    }
    osThreadAttr_t attr;

    attr.name = "CloudMainTaskEntry";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = 24;

    if (osThreadNew((osThreadFunc_t)CloudMainTaskEntry, NULL, &attr) == NULL) {
        printf("Failed to create CloudMainTaskEntry!\n");
    }
}

APP_FEATURE_INIT(IotMainTaskEntry);