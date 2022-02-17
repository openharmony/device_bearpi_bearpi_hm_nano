Reason for abandonment of this repository: the repository is abandoned due to the separation and adjustment of board and SOC
New repository:[device_board_bearpi](https://gitee.com/openharmony/device_board_bearpi)，[device_soc_hisilicon](https://gitee.com/openharmony/device_soc_hisilicon)
# BearPi-HM Nano<a name="ZH-CN_TOPIC_0000001130176841"></a>

-   [Get source code](#section11660541590)
-   [Construction of development environment](#section11660541591)
-   [Source code compilation](#section11660541592)
-   [Firmware burning](#section11660541593)
-   [Introduction to development board](#section11660541593)
-   [Development board details](#section12212842173518)
-   [Application](#section1464106163819)
-   [Directory Structure](#section1464106163817)

## Get source code<a name="section11660541590"></a>
```
repo init -u git@gitee.com:openharmony/manifest.git -b master --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'
```
## Construction of development environment<a name="section11660541591"></a>
Please refer to [Hi3861 development environment construction](https://gitee.com/openharmony/docs/blob/master/en/device-dev/quick-start/quickstart-lite-steps-hi3861-setting.md)

## Source code compilation<a name="section11660541592"></a>
```
hb set

bearpi
    >bearpi_hm_nano

选择bearpi_hm_nano

hb build -f
```
## Firmware burning<a name="section11660541593"></a>
1. Open hiburn tool in windows, click 'Refresh', and select com number in 'com', as shown in the following figure。

    ![](docs/quick-start/figures/HiBurn_Main_interface.png)

    Then click 'Setting' and select 'Com settings'.

2. Set 'Baud' to '921600' in com settings, and click OK, as shown in the figure below.   

    ![](docs/quick-start/figures/HiBurn_Comsettings.png)

3. Click the 'select file' button in hiburn tool, and in the pop-up file box, select the './out/bearpi_hm_nano/bearpi_hm_nano/' file under the 'Hi3861_wifiiot_app_allinone.bin' path of the project file, as shown in the following figure.

    ![](docs/quick-start/figures/HiBurn_Open_file.png)

4. Click the 'Auto burn' check box, and then click 'Connect', as shown in the following figure.

    ![](docs/quick-start/figures/HiBurn_Ready_to_download.png)

    The 'Connect' button changes to 'Disconnect' and waits for the download.

5. Reset the 'RESET' button on the development board to start downloading the program, as shown in the figure below.

    ![](docs/quick-start/figures/Reset_development_board.png)

    ![](docs/quick-start/figures/Hiburn_Downloading.png)

    
6. Until the word "Execution Successful" appears, the program download is completed.

7. After downloading, click the 'Disconnect' button to facilitate later debugging.
## Introduction to development board<a name="section11660541593"></a>

### Development board overview

BearPi[（BearPi-HM Nano）](https://item.taobao.com/item.htm?id=633296694816)is a development board specially built by BearPi school for openharmony system. It carries a highly integrated 2.4GHz WiFi SoC chip hi3861, and carries NFC circuit and standard E53 interface. The standard E53 interface can expand cases such as intelligent humidifier, intelligent desk lamp, intelligent security, intelligent smoke detector, etc.

### Development board function
BearPi-HM Nano development board, for developers, is used for openharmony development and learning. At the same time, it provides rich cases and tutorials to realize full scene application design.

1. E53 interface: it is a standard interface with rich resources and easy to expand. It realizes multi application case expansion, making case development more flexible and convenient.

2. NFC: the on-board NFC RF circuit, combined with the key capabilities of openharmony, can perfectly realize the "touch and touch" networking mechanism and service pull-up function of openharmony system, reduce the complexity of user operation, and improve the user experience.

3. User keys: open function keys. The key functions are fully defined by the developer, which increases user operability.

## Development board details<a name="section12212842173518"></a>
The detailed functions of the development board are shown in the figure below:

![](figures/BearPi-HM_NanoBoardDetail.png)

## Application<a name="section1464106163819"></a>

### Development board application scenario

Bearpi_HM_Nano can be used in intelligent humidifier, intelligent desk lamp, intelligent security, intelligent smoke detector and other cases, such as the following cases.

1. Intelligent humidifier: it can monitor the current indoor temperature and humidity in real time, and can open and close the humidifier remotely and in real time.

2. Intelligent desk lamp: it can monitor the current indoor light intensity in real time. When it is lower than the predetermined threshold, the desk lamp will be turned on automatically, and the independent control of the desk lamp can be realized.

3. Intelligent security: it can monitor the movement of human body within the range in real time and report to the cloud display to realize the monitoring and alarm of intelligent security.

4. Intelligent smoke sensing: it can monitor the smoke concentration in the current room in real time and report to the cloud display. When the smoke concentration exceeds the set threshold, the alarm will trigger the alarm immediately.

## Directory Structure<a name="section1464106163817"></a>

The root directory of the Hi3861 SDK is  **device\\bearpi\\bearpi\_hm\_nano\\sdk\_liteos**. Its directory structure is as follows:

```
device/bearpi/bearpi_hm_nano/sdk_liteos
├── app                 # Application layer code, including the demo program.
├── boot                # Flash bootloader code.
├── build               # Library, link, and configuration files required for SDK construction.
├── BUILD.gn            # GN building script.
├── build_patch.sh      # File used for decompressing the U-Boot open-source code package and applying patches.
├── build.sh            # Building script for startup, which can be customized using the sh build.sh menuconfig command.
├── components          # Files related to the SDK platform.
├── config              # SDK system configuration files.
├── config.gni          # Configuration files supporting OpenHarmony.
├── factory.mk          # Building script of the factory-test version.
├── hm_build.sh         # Building script for adapting to OpenHarmony.
├── include             # Header files declaring APIs.
├── license             # License of the open-source SDK.
├── Makefile            # File for building using the make or make all command.
├── non_factory.mk      # Building script of the non-factory-test version.
├── platform            # Files related to the SDK platform, including the kernel image and driver modules.
├── SConstruct          # SCons build script.
├── third_party         # Open-source third-party software.
└── tools               # Tools provided by the SDK for Linux and Windows systems, including the NV making tool, signature tool, and Menuconfig.
```



## Repositories Involved<a name="section1371113476307"></a>

**device/bearpi/bearpi_hm_nano**

vendor/bearpi

