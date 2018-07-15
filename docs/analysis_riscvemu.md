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
  4. 依据p->tab_drive[i].filename载入drive,保存在p->tab_drive[i].block_dev。
  5. 依据p->tab_fs[i].filename载入fs，保存在p->tab_ts[i].fs_dev。
  6. 如果是用户态，则为p->tab_eth[i].net赋值，否则打印错误信息。
  7. 如果设置了宏CONFIG_SDL，则初始化p->width,p->height，否则初始化p->console
  8. 函数virt_machine_init初始化虚拟机。即依据p设置好s，随后释放p占用的空间。
  9. 函数virt_machine_run运行虚拟机。
  10. 函数virt_machine_end释放资源后退出。

##### 初始化虚拟机

初始化虚拟机是在`virt_machine_init`函数里完成的，即依据VirtMachineParams的值填充RISCVMachine的值，最后返回是的经过强制类型转换后RISCVMachine的值。

1. 为变量RISCVMachine分配内存空间。
2. 初始化虚拟机的内存(s->ram_size)，指定内存管理结构体(s->mem_map)。
3. 初始化CPU状态(s->cpu_state)。
4. 使用cpu_register_ram函数分配内存资源。
5. 设置虚拟机时间(s->rtc_real_time)。
6. 使用cpu_register_device函数分配设备资源(CLINT, PLIC, HTIF)。
7. 设置virtio console，即s->common.console_dev和s->virtio_count。
8. 设置virtio net device，即s->common.net和s->virtio_count。
9. 设置virtio block device。
10. 设置virtio filesystem。
11. 如果设置了p->display_device，则设置s->common.fb_dev。
12. 如果设置了p->input_device，则设置s->keyboard_dev，s->mouse_dev。
13. 由copy_kernel函数实现将内核复制到虚拟机的内存中。

##### 虚拟机运行

virt_machine_run函数是虚拟机运行的顶层函数，真正干活的是riscv_cpu_interp32函数。

1. 设置delay, rfds, wfds, efds, tv
2. 如果设置了m->console_dev，则设置STDIODevice为m->console->opaque
3. 如果设置了m->net，则执行m->net->select_fill函数
4. 函数virt_machine_interp执行了内核二进制文件，调用关系vit_machine_interp --> riscv_cpu_interp --> riscv_cpu_interp32。

riscv_cpu_inerp32(在riscvemu_template.h文件中)函数执行流程。

1. 传入的参数是结构体RISCVCPUState和最大允许执行的周期数n_cycles。
2. s->insn_counter是指令计数器，insn_counter_addend是指令计数器允许的最大值。
3. 如果发生了中断，则在raise_inerrupt函数里处理中断。
4. 用一个循环体来模拟指令执行。
   1. 如果--n_cycles为0则周期数用完，退出指令执行。
   2. 如果code_ptr>=code_end，则表明发生了跳转、分支或system类指令。
      - 如果(s->mip & s->mie) != 0，则表明有要处理的中断，由raise_interrupt函数来处理此中断。
      - 重新为code_ptr和code_end赋值。
      - 如果依然code_ptr>=code_end，则可能是指令跨页了。
   3. 否则code_ptr<code_end，则表明这是正常的指令流，通过code_ptr值得到当前指令，保存在变量insn中。
   4. 从insn中分解出各个操作符和操作数。(opcode, rd, rs1, rs2)
   5. 通过opcode来判断执行什么操作。
   6. 如果设置了CONFIG_EXT_C，则表明使用的是压缩指令，则会从insn中分解出funct3，通过funct3来判断执行什么操作。
   7. opcode依次是lui, auipc, jal, jalr, branch, load, store, op-imm, op-imm-32,  op, op-32, system, misc-mem, amo, load-fp, store-fp, madd, msub, nmsub, nmadd,  op-fp

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

`config_file_loaded`的实现是在machine.c文件里，首先用`virt_machine_parse_config`函数对配置文件和数据结构VirtMachineParams进行分析，然后通过`config_additional_file_load`函数来把附加的二进制文件载入内存。

`virt_machine_parse_config`的实现是在machine.c文件里，其执行过程如下：

1. 解析配置文件的内容，以json格式的形式存放在变量cfg中。
2. 获取配置文件里version的值，它要与模拟器版本匹配，目前模拟器版本为1。
3. 获取配置文件里machine的值，它要与模拟器代表的机器名一致，可能的机器名有riscv32, riscv64, riscv128。
4. 获取配置文件里memory_size的值，并将其以M为单位存储在VirtMachineParams的ram_size里。
5. 获取配置文件里的bios的值，并将其存储在VirtMachineParams的files[VM_FILE_BIOS].filename里。那么这里的bios究竟在VM里起什么作用呢？
6. 获取配置文件里kernel的值，并将其存储在VirtMachineParams的files[VM_FILE_KERNEL].filename里。
7. 获取配置文件里cmdline的值，使用comline_subst函数将其保存在VirtMachineParams的cmdline里。
8. 填充VirtMachineParams的tab_drive和drive_count。
9. 填充VirtMachineParams的tab_fs和fs_count。
10. 填充VirtMachineParams的tab_eth和eth_count。
11. 填充VirtMachineParams的display_device和files[VM_FILE_VGA_BIOS]。
12. 依次获取input_device, accel, rtc_local_time，并赋值给VirtMachineParams的input_device, accel_enable, rtc_local_time。
13. 释放变量cfg占用的内存，正常退出返回0，不正常退出返回-1.
