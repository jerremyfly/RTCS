# CSDKExample 

### 介绍
该工程是一个简单的测试demo，它基于Qt开发实现，全面覆盖了视频会议SDK中所有功能接口。<br>
该demo实现了多人音视频通话、发送消息和屏幕共享等功能，代码结构清晰简单，清楚的示范了SDK库中所有接口的调用方法，用户可以根据自己的需求调用SDK接口对demo做出修改或重写。<br>

### 使用说明
1. 下载代码，使用配置好qt5.6.1的vs（13或17）或qt creator打开工程，把下载RTCSSDKForC文件夹
   放置sample同一目录（或重新配置头文件包含目录），编译；<br>
2. 把depends提供的第三方库和SDK相关动态库、静态库拷贝至自己工程生成目录bin\release下；<br>
3. 运行CSDKExample。<br>

#### 注：
[RTCSSDKForC](../../RTCSSDKForC/include) 包含CSDKExample需要引用的头文件，其中[RTCSSDKForC.h](../../RTCSSDKForC/include/RTCSSDKForC.h) 即SDK包含的所有接口。
