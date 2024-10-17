# hello，world
利用llvm ir实现hello，world

## 构建llvm

### 实验环境：
wsl-ubuntu 22.04 ,llvm-17.06
1. 安装wsl或者使用虚拟机配置ubuntu 22.04
2. sudo apt -y install gcc g++ git cmake ninja-build zliblg-dev vim curl 安装开发工具 运行之前先更新sudo apt update
3. 下载llvm-17.06
4. tar -xvf 解压文件
5. 构建llvm 完整的llvm文件目录 
llvm_project_1706 ls
build llvm_install_dir llvm-project
build 用来存放cmake构建的缓存文件
llvm_install_dir 用于存放构建完成，llvm生成的动态库、静态库、头文件
llvm-project 文件夹下的llvm包含llvm源码和clang
6. 进入build文件夹下执行
cmake ../llvm-project/llvm  -GNinja -DLLVM_OPTIMIZED_TABLEGEN=ON -DLLVM_TARGETS_TO_BUILD=X86 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=ON -DLLVM_USE_LINKER=gold -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_INSTALL_PREFIX=../llvm_install_dir 
7. cmake生成ninjia后执行
ninja -j4  #j4 表示指定4个线程，可以根据机器的能力设置 时间会很长，耐心等待吧
8. ninja install #前面的命令，务必设置-DCMAKE_INSTALL_PREFIX=xxxdir，不建议安装在默认路径

9. 构建成功

## 编写代码
详情直接看main.cc
具体内容可以利用ai阅读了解一下

### 注意
.vscode中有配置文件需要修改
主要是task中"-DLLVM_DIR=/home/xxx/llvm_project_1706/llvm_install_dir/lib/cmake/llvm",
换成自己的路径其他应该没有要修改的
f5运行调试
