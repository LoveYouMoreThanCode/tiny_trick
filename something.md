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
# ceonos上使用jemalloc定位内存泄漏
**生成pdf依赖软件** 
```
yum install ghostscript graphviz
```
**编译和安装jemalloc** 
注意：脚本较多（可能会有permission denied错误），执行chmod 777 -R ./*
```
./configure --enable-prof
make 
make install
```
**加载jemalloc运行程序** 
```
MALLOC_CONF=prof:true,lg_prof_interval:20 LD_PRELOAD=/usr/local/lib/libjemalloc.so.2 程序名以及运行参数
```
**生成pdf分析**
```
jeprof --pdf 二进制名 --base=./proffile1 proffile2 > my.pdf
```

# vim配置clang-format
**准备clang-format.py**
拷贝到vim所在机器的目录，比如/home/fanwei
```
https://github.com/llvm-mirror/clang/blob/master/tools/clang-format/clang-format.py
```
**修改~/.vimrc**
```
if has('python')
  map <C-K> :pyf /home/fanwei/clang-format.py<cr>
  imap <C-K> <c-o>:pyf /home/fanwei/clang-format.py<cr>
 elseif has('python3')
  map <C-K>:pyf /home/fanwei/clang-format.py<cr>
  imap <C-K>:pyf /home/fanwei/clang-format.py<c-o><cr>
endif
```
**格式化**
- shift + v，然后j或k选择需要格式化的代码
- ctrl + k进行格式化


