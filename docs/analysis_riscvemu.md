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

##### 预定义的宏

- CONFIG_FS_NET

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

  + 整型ram_size，表示内存大小。

  + 枚举型drive_mode，表示镜像属性(BF_MODE_RO, BF_MODE_RW, BF_MODE_SNAPSHOT)

  + 布尔型allow_ctrlc，表示C-C组合键的作用(TRUE停止模拟器，FAUSE发给模拟器)

  + 字符串cmdline，向内核命令行追加的cmdline。

  + 整型accel_enable，x86使用VM加速(非0则使用加速，FALSE则禁用加速)

  + 字符串path，必选的参数：配置文件。

  + 结构体p(`type def struct{} VirtMachineParams;`)，存储了虚拟机的参数。

    p->ram_size对应ram_size;

    p->accel_enable对应acel_enable;

    p->cmdline对应cmdline;

    p->drive_count设备数量；

    p->tab_drive[]设备列表；

    p->fs_count 设备数量；

    p->tab_fs[]设备列表；

    p->cfg_filename配置文件；

    p->eth_count以太网设备数量；

    p->tab_eth以太网设备列表；

    p->console控制台；

    p->rtc_real_time实时时钟；

    p->rtc_local_time本地时钟；

  + 结构体s(`type def struct{}VirtMachine`)，代表了虚拟机。

- 运行流程：

  1. 用一个无限的for循环来遍历所有可选的参数，短选项h,b,m，长选项0为help，1～5如注释所标记。
  2. 用变量path保存配置文件。
  3. 函数virt_machine_load_config_file来载入配置文件
  4. 打开文件与设备
  5. 函数virt_machine_init初始化虚拟机
  6. 函数virt_machine_run运行虚拟机
  7. 函数virt_machine_end释放资源后退出。

#### build_filelist的原理

#### splitimg的原理

#### 文件结构

- riscvemu.c：抽象的最顶层

  main：主控函数

  void launch_alternate_executable(char **argv, int xlen)：修改argv[0]为riscvemu+xlen

- machine.c，machine.h：虚拟机的相关功能与定义。

- virtio.c, virtio.h：虚拟机I/O驱动。

- riscv_cpu.c, x86_cpu.c

- riscv_machine.c, x86_machine.c

- ide.c, ps2.c, vmmouse.c, pckbd.c, vga.c

- slirp/*

#### 一些问题

##### 关于配置文件

问题描述：riscvemu里最重要的参数就是配置文件了，期望理解配置文件的详细作用，将其改成类似qemu的参数形式。

在riscvemu.c的main函数里，可见代表配置文件的字符串是赋值给`path`的。对`path`的处理是放在`virt_machine_load_config_file`函数里。

`virt_machine_load_config_file`的实现是在machine.c文件里，配置文件是结构体`VirtMachineParams`里的一项`cfg_filename`，而`VirtMachineParams`是结构体`VMConfigLoadState`里的一项`vm_params`，接下来又调用了`config_load_file`函数，由此看来`virt_machine_load_config_file`只是对`config_load_file`的封装。

`config_load_file`的实现是在machine.c文件里，它把配置文件载入到内存里，然后执行第三个参数代表的函数`config_file_loaded`，由此看来`config_load_file`只是对它第三个参数的封装。

`config_file_loaded`的实现是在machine.c文件里，首先用virt_machine_parse_config函数对配置文件和数据结构VirtMachineParams进行分析，然后通过config_additional_file_load函数来把所有二进制文件载入内存。

virt_machine_parse_config的实现是在machine.c文件里，其执行过程如下：

1. 解析配置文件的内容，以json格式的形式存放在变量cfg中。
2. 获取配置文件里version的值，它要与模拟器版本匹配，目前模拟器版本为1。
3. 获取配置文件里machine的值，它要与模拟器代表的机器名一致，可能的机器名有riscv32, riscv64, riscv128。
4. 获取配置文件里memory_size的值，并将其以M为单位存储在VirtMachineParams的ram_size里。
5. 获取配置文件里的bios的值，并将其存储在VirtMachineParams的files[VM_FILE_BIOS].filename里。那么这里的bios究竟在VM里起什么作用呢？
6. 获取配置文件里kernel的值，并将其存储在VirtMachineParams的files[VM_FILE_KERNEL].filename里。
7. 获取配置文件里cmdline的值，使用comline_subst函数将其保存在VirtMachineParams的cmdline里。
8. 

