# vim中粘贴格式乱
```
set paste
```
# clang-format配置文件生成
```
clang-format -style=google -dump-config > .clang-format
```
# git配置代理
```
git config --global https.proxy http://127.0.0.1:7890
git config --global https.proxy https://127.0.0.1:7890

git config --global --unset http.proxy
git config --global --unset https.proxy
```
# centos配置容器开发
**拉取centos7镜像**
```
docker pull centos:7
```
**创建并启动centos7容器，运行init程序（必须打开privileged选项，否则将出现“Failed to get D-Bus connection: Operation not permitted”）**
```
docker run -d -name centos7 -p10086:22 --privileged=true centos:7 /usr/sbin/init
重要参数：
- --privileged=true
- /usr/sbin/init
- -p10086:22
```
**进入容器终端，运行bash程序**
```
docker exec -it centos7 /bin/bash
```
**安装所需软件**
```
yum install -y initscripts openssh-server openssl openssl-devel
```
**启动ssh服务**
```
service sshd restart
```
 
