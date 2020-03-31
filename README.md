# 8-ways-smart-switch
 基于esphome的多控开关，最高支持八路

## 使用方法
### 硬件选择部分
下位机尽量选择 arduino nano/mini/pro mini，引脚多，体积小。arduino pro micro有坑，它的串口是usb模拟的，貌似无法正常使用
上位机普通的esp8266 nodemcu就行，想自己焊也ok，只要引出两个gpio口做软串口就行
另外需要一个5v高电平触发的继电器模块最高支持8路。
### 下位机
lower_machine_firmware文件夹内的就是下位机固件，按照代码注释做好配置，直接刷入即可。
按照代码中配置的输出端口连接继电器模块的控制端，输入端连接开关。
### 上位机
安装好esphome环境（可以去github下载源码部署或用pip安装，详见官网 www.esphome.io ）

按照你的芯片版本，新建一个esphome节点（命令行：esphome [配置文件名.yaml] wizard），按照步骤一步步来即可。创建完配置文件之后将esphome config file文件夹中customSwitches.h复制到与你的yaml配置文件相同的文件夹中，参照示例example.yaml中的注释填写配置即可。

之后下载到板子里即可。（命令行执行esphome [配置文件名.yaml] run）按照提示步骤一步一步来就行了。

按照配置文件中定义的tx，rx端口，连接下位机的tx，rx端口（注意对调）就可以正常使用了,注意所有模块必须共地，否则会出问题。

有问题可以联系作者邮箱cloudicen@gmail.com，如果他能看到的话（
