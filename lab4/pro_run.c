// proc_run - make process "proc" running on cpu
// NOTE: before call switch_to, should load  base addr of "proc"'s new PDT
void
proc_run(struct proc_struct *proc) {
// 如果要调度的进程不是当前进程的话进行如下操作
    if (proc != current) {
        bool intr_flag;
        struct proc_struct *prev = current, *next = proc;
// 关闭中断，防止进程调度过程中发生其他中断导致嵌套的进程调度
        local_intr_save(intr_flag);
        {
// 当前进程设为待调度的进程
            current = proc;
// 加载待调度进程的内核栈基地址和页表基地址
            load_esp0(next->kstack + KSTACKSIZE);
            lcr3(next->cr3);
// 保存原线程的寄存器并恢复待调度线程的寄存器
            switch_to(&(prev->context), &(next->context));
        }
// 恢复中断
        local_intr_restore(intr_flag);
    }
}