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
