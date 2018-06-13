解读riscvemu的实现原理。

#### 编译过程

1. 把\*.c编译为\*.o。对于文件riscv_cpu.c，-DMAX_XLEN参数定义了宏MAX_XLEN的大小，进而决定生成目标文件的名称。(例如参数为64则生成riscv_cpu64.o)
2. 把多个*.o编译为riscvemu。生成了3个文件：riscvemu32，riscvemu64，riscvemu128。**riscv模拟器**
3. 把多个*.o文件编译为x86emu。**x86模拟器**
4. riscvemu是对riscvemu64的链接。
5. build_filelist是从3个*.o文件编译而成。**对目录构建一个文件列表**
6. splitimg是编译自splitimg.c。**为HTTP块设备创建多文件磁盘镜像**

#### 运行过程

##### 入口函数

- main(riscvemu.c)函数是四个模拟器共用的入口函数。如果定义了宏`CONFIG_CPU_RISCV`，则是RISCV模拟器。如果定义了宏`CONFIG_CPU_X86`，则是X86模拟器。

- 参数：可选的参数，加一个必选的配置文件。

  ```
  -m		# 指定内存大小
  -rw		# 允许写镜像
  -ctrlc	# C-c组合键的作用是停止模拟器，而不是发送给模拟器
  -append cmdline	# 向内核命令行追加cmdline
  -b		# 仅用于RISCV，设置寄存器宽度
  -no-accel	# 禁用VM加速
  ```

  



