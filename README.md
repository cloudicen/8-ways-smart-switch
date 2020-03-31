# 8-ways-smart-switch
 基于esphome的多控开关，最高支持八路

## 使用方法
### 硬件部分
### 下位机
lower_machine_firmware文件夹内的就是下位机固件，按照代码注释做好配置，直接刷入即可。
### 上位机
安装好esphome环境（可以去github下载源码部署或用pip安装，详见官网 www.esphome.io ）

按照你的芯片版本，新建一个esphome节点（命令行：esphome [配置文件名.yaml] wizard），按照步骤一步步来即可。创建完配置文件之后将esphome config file文件夹中customSwitches.h复制到与你的yaml配置文件相同的文件夹中，参照示例example.yaml中的注释填写配置即可。

之后下载到板子里即可。（命令行执行esphome [配置文件名.yaml] run）按照提示步骤一步一步来就行了。
