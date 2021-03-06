#### PAHO库修改说明

在PAHO v1.1.0版本上修改，修改的主要点如下：

| 修改级别 | 修改内容                                                     |
| :------- | ------------------------------------------------------------ |
| 修复     | 原库定义的SUCCESS会引起冲突，C50版本重新命                   |
| 修复     | MQTTClientInit原库没有返回值，C50版本修改为有返回值，并增加了MQTTClientDeinit函数 |
| 修复     | MQTTClientInit原库没有检查互斥锁返回值，C50版本修改为检查返回值 |
| 优化     | C50版本修改了TOPIC匹配函数（用于收到消息时投递消息），应该是引用了某个库的API函数，暂时未查阅到 |
| 修复     | C50在CYCLE中增加了UNSUBACK的处理                             |
| 修改     | 原库版本的KEEPALIVE逻辑为上次发送和上次接收为或逻辑，C50版本修改为与逻辑 |
| 修改     | C50版本修改了超时重传逻辑(QOS2)                              |
| 修复     | C50版本的MQTTYield函数使用互斥锁进行互斥，避免数据冲突       |
| 修改     | 原库版本订阅函数的回调函数不支持ARG参数，修改为带参数模式    |

修复：原版本或者库存在显性或者隐性BUG，进行修复

修改：原版本不满足现有需求或者功能，需要修改

优化：对某些实现做了更合适的修改

