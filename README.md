#### 简介

本项目是从[riscvemu](https://bellard.org/riscvemu/)修改而来，旨在基于RISC-V进行一些简单的OS的实验。

RISCVEMU是一个RISC-V架构的模拟器。它的目的是在保持完整的基础上实现一个小的简单的模拟器。

- docs：存放了所有的学习笔记和文档。
- diskimg：存放了在虚拟机上运行所需的镜像。

#### 编译

```shell
# 对于ubuntu18.04或16.04均适用，安装编译所需的库
sudo apt install libcurl4-gnutls-dev libssl-dev libsdl1.2-dev libc6	
git clone https://github.com/shzhxh/riscv_computer.git
cd riscv_computer
make
sudo make install	# 可选，将程序装到可执行目录
```

#### 运行

```
./riscvemu diskimg/root-riscv64.cfg
```

