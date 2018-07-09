#### 问题描述

qemu支持的机器有none，sifive_e, sifive_u, spike_v1.10, spike_v1.9.1, virt，而riscvemu支持的机器是riscvemu32和riscvemu64，那么qemu和riscvemu支持的机器有什么异同呢？

##### qemu支持的机器

在hw/riscv/目录下可以看到相关的代码。

- none 

  empty machine

- sifive_e

  SiFive E SDK

  **UART** + **CLINT** + **PLIC** + **PRCI** + **Registers emulated as RAM** + **Flash emulated as RAM**

- sifive_u

  SiFive U SDK

  **UART** + **CLINT** + **PLIC**

- spike_v1.10

  Spike Board(Privileged ISA v1.10)

  **HTIF** + **CLINT** + **PLIC**

- spike_v1.9.1

  Spike Board(Privileged ISA v1.9.1)

  **HTIF** + **CLINT** + **PLIC**

- virt

  VirtIO Board(priv 1.10)

  **UART16550a** + **VirtIO MMIO**

##### riscvemu支持的机器

在[riscvemu](https://bellard.org/riscvemu/)可以看到相关信息

spec 2.2 + priv 1.10

**VirtIO input** + **PCI bus** + **VirtIO PCI** + **user mode network interface**

#### 关于virtIO

[IBM developerWorks](https://www.ibm.com/developerworks/cn/linux/l-virtio/index.html)