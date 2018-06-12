#### 简介

本项目是从[riscvemu](https://bellard.org/riscvemu/)修改而来，旨在基于RISC-V进行一些简单的OS的实验。

RISCVEMU是一个RISC-V架构的模拟器。它的目的是在保持完整的基础上实现一个小的简单的模拟器。

docs：存放了所有的学习笔记和文档。

diskimg：存放了在虚拟机上运行所需的镜像。

#### 编译

```shell
wget https://bellard.org/riscvemu/riscvemu-2017-08-06.tar.gz
tar zxvf riscvemu-XXX.tar.gz
cd riscvemu-XXX
vim Makefile	# 删除-Werror
make
```

##### 问题解决

- `fatal error: curl/multi.h: No such file or directory`

  缺少库，对于ubuntu18.04，应安装`sudo apt install libcurl4-gnutls-dev`

#### 运行

```
wget https://bellard.org/riscvemu/diskimage-linux-riscv-2017-08-06.2.tar.gz
tar zxvf diskimg-linux-riscv-XXX.tar.gz
./riscvemu diskimg-linux-riscv-XXX/root-riscv64.cfg
```

