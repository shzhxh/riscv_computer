解读riscvemu的实现原理。

#### 编译过程

编译目标是四个模拟器和两个应用程序。四个模拟器是riscvemu32，riscvemu64，riscvemu128，x86emu。两个应用程序是build_filelist和splitimg。

##### 模拟器

1. 把\*.c编译为\*.o。

   对于文件riscv_cpu.c，-DMAX_XLEN参数定义了宏MAX_XLEN的大小，进而决定生成目标文件的名称。(例如参数为64则生成riscv_cpu64.o)

   对于文件riscvemu.c，给定-DCONFIG_CPU_RISCV参数则生成riscvemu.o，给定-DCONFIG_CPU_X86则生成x86emu.o

2. 把riscv_cpuXX.o, riscvemu.o和其它*.o文件编译为riscvemuXX。生成了3个文件：riscvemu32，riscvemu64，riscvemu128。**riscv模拟器**

3. 把x86emu.o和*.o文件编译为x86emu。其中x86_cpu.c, x86_machine.c, ide.c, ps2.c, vmmouse.c, pckbd.c, vga.c是仅由x86emu使用的。**x86模拟器**

4. riscvemu是对riscvemu64的链接。

##### build_filelist

- build_filelist是从build_filelist.c、fs_utils.o、cutils.o文件编译而成。**对目录构建一个文件列表**

##### splitimg

- splitimg是编译自splitimg.c。**为HTTP块设备创建多文件磁盘镜像**

#### 模拟器的原理

##### 入口函数

- main(riscvemu.c)函数是四个模拟器共用的入口函数。如果定义了宏`CONFIG_CPU_RISCV`，则是RISCV模拟器。如果定义了宏`CONFIG_CPU_X86`，则是X86模拟器。

- 参数：若干可选的参数，加一个必选的参数(配置文件)。可选参数如下：

  ```
  -m		# 指定内存大小，以MB为单位
  -rw		# 允许写镜像，默认为快照。
  -ctrlc	# C-c组合键的作用是停止模拟器，而不是发送给模拟器
  -append cmdline	# 向内核命令行追加cmdline
  -b		# 仅用于RISCV，设置寄存器宽度，有效的值是32, 64, 128
  -no-accel	# 仅用于x86，禁用VM加速
  ```

- 变量：

  + 结构体p，存储了虚拟机的参数。

- 运行流程：

  1. 用一个无限的for循环来遍历所有可选的参数，短选项h,b,m，长选项0为help，1～5如注释所标记。
  2. 用变量path保存配置文件。
  3. 函数virt_machine_load_config_file来载入配置文件
  4. 打开文件与设备
  5. 函数virt_machine_run运行虚拟机
  6. 函数virt_machine_end释放资源后退出。

#### build_filelist的原理

#### splitimg的原理

#### 文件结构

- riscvemu.c：抽象的最顶层，主控函数。

- machine.c，machine.h：虚拟机的相关功能与定义。
- virtio.c, virtio.h：虚拟机I/O驱动。
- riscv_cpu.c, x86_cpu.c
- riscv_machine.c, x86_machine.c
- ide.c, ps2.c, vmmouse.c, pckbd.c, vga.c
- slirp/*



