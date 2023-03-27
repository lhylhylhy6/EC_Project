# 多传感器设计

---

## 说明

多传感器实时数据采集：通过搭载多传感器，为城市环境或辅助巡防车提供数据支持。

## 传感器清单

-  mpu9250九轴传感器
-  ATGM332D GPS模块[参考链接](https://doc.embedfire.com/module/module_tutorial/zh/latest/Module_Manual/port_class/gps.html)
-  LM386声音传感器（噪声检测）
-  XFS5152CE语音合成模块[参考链接](https://zhuanlan.zhihu.com/p/139751645)
-  ESP8266联网模块
-  红外传感器
-  温湿度传感器
-  BMP280气压传感器
-  BH1750光照传感器
-  GP2Y10 PM2.5颗粒传感器（用于城市环境污染监测）
-  LPRIDAR A1：思岚激光雷达[RTT软件包rplidar](https://github.com/wuhanstudio/rplidar)
-  SIM7600CE 4G模块

## 系统设计

- 环境监测：PM2.5监测、噪声监测
-  气象数据采集：光照监测、气压监测、温湿度监测
-  麦克风交流
-  雷达避障

