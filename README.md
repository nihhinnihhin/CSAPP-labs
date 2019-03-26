# CSAPP-labs
跟着cmu-ics-fall2015课程做labs


# CSAPP

## data lab

## bomb lab
首先使用`objdump -t`得到程序符号表，`objdump -d`得炸弹程序的反汇编代码, 阅读反汇编代码，可以得到以下主要信息：main程序的结构，包含6个phase，每个phase要求输入特定的字符才可通过，不会引爆炸弹。因此通过gdb调试程序，在explode_bomb打断点以不会引爆炸弹，并结合分析反汇编代码和通过gdb寻找密码即可破解。

1. phase_1: 
    * if (string_not_equal(char *s1, char *s2)) skip,
    * 在gdb中使用命令 x /s 0x402400 打印出字符串
    * solution:Border relations with Canada have never been better.

2. phase_2:
    * read_six_numbers(char *buf, char *modifers),                    * buf（0x4023a8）保存输入的6个数字，modifers通过读分配的栈地址%rsp指向的字符串得到"%d %d %d %d %d %d"
    * 接着由下面的汇编语言表示的循环与比较可知
    * solution: "1 2 4 8 16 32"
    关键词：
3. phase_3:
    * 类似phase_2，首先通过sscanf(buf, "%d %d", &n, &num)读入两数字，该程序是swich(n) case 结构，对比num与程序中的数字，相同即通过，所以可以有多个答案
    * solution:0 207;...

4. phase_4
    *  ret = sscanf(str, "%d %d", &n1, &n2)，同理读入两个数字，接着以此为参数调用func4(n1, 0 , 14), func4主要进行一些数学运算：加减乘除，移位，比较最终结果，符合即通过

5. phase_5
    * 首先通过程序可知个字符库strhidden[100]，通过用户输入的字符与0xf进行与运算得到6个偏移量，接着使用该偏移量以该字符库某个基地址寻找6个与目标相符的字符，即可进行拆解。
```
  1 // pseudo code
  1 // pseudo code
  2 phase_5(char *user_str)
  3 {
  4  char offset[7];
  5  long i = 0;
  6  len = len(user_str); // the len() is python's function, not c
  7  char string[7];
  8  char str_hidden[100] = "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?";
  9  if (len == 6)
 10  {
 11    while(i != 6)
 12    {
 13      *(offset + i) = *(user_str + i) & 0xf;
 14      *(string + i) = *(str_hidden + *(offset + i));
 15      i++;
 16    }
 17    offset[6] = '\0';
 18  }
 19  char target[7]="flyers"; // addr of target is only provided by bomb.s
 20  if (! string_not_equal(string, target))
 21    Done;
Trials to get correct string:

```

## attack lab 

思路：通过缓冲溢出，来注入代码，然后其
代码注入攻击：
1. level1，执行getbuf后返回到指定函数

* 通过objdump -d 获得ctarget的反汇编代码，得到目标函数地址，buffer_size；
* 根据上面设计输入字符串，溢出部分覆盖原程序的返回地址为目标函数地址(bite representation of starting address for touch1)
* byte ordering(big endian, small endian)

sol:
```
00 00 00 00 00 00 00 00 00 00 /* BUFFERSIZE = 40, thus 40 bytes allocated */
  2 00 00 00 00 00 00 00 00 00 00 /* i.e. 80 hex digits */
  3 00 00 00 00 00 00 00 00 00 00
  4 00 00 00 00 00 00 00 00 00 00
  5 c0 17 40 /* addr of function touch1: 00000000004017c0 <touch1> */
  6 /* a8 17 40 for fun, this would cause an infinite loop, since it is addr of getbuf, but failed. */
```

level2:执行getbuf后将cookie作为参数执行目标函数

* 思路同上，但是增加了两点，就是缓冲溢出，设计一段Z指令序列：覆盖返回地址为即将执行恶意操作的指令地址（栈地址），该指令为：将cookie存入%edi寄存器作为第一参数，接着将目标函数的地址压入栈（top）中，执行返回（C3），即跳转到目标函数中.
* 将该指令序列通过hex2raw编程字符串形式。

level3：任务同上，不过传入的参数时字符串

* 思路同上，不同的有两点：一是要注入cookie的字符串（ascii码，结尾\0）的十六进制表示，二是将字符串首地址作为参数压入%edi中

面向返回编程：

由于操作系统采取随机分配函数栈地址的策略及限制栈中数据的属性，即注入代码不能作为可执行代码指令，所以要通过另外一种思路。

被攻击程序里中翻译成cpu的指令都可以执行，因此可以在程序里找到如下结构的指令：
gadget code  c3
意思是执行某些操作，然后返回。popq，pushq（%rsp）

* 首先设计攻击代码，需要执行哪些操作，譬如对于一个操作，可以有多个instruction move $0x11, %rdi; push then pop then move 等等，写成汇编语言的文件example.s
* 接着gcc -c example.s 得到obj文件，接着objdump -d example.o > example.d 得到一个比较规范的含十六进制表示和汇编语言表示的文件，根据该内容得到要注入的指令，接着再用./hextoraw得到二进制表示的字符串
通过覆盖栈的返回地址为
* 按照上述的操作重复level2, level3的攻击。

## cache lab
* 写一个缓存模拟器，根据trace文件的instructions，（load, store, modify）对缓存的hit，miss，eviction进行计数，。三个for loop（hit, miss, miss and eviciton）
 
* cache: artrary s, E, b, use malloc function
* I(instruction, no needed) L M S(data cache performance)
* 假设所有instruction的内存访问都是对齐正确的，就是单个的内存访问不会越界（block boundaries）
* getopt 来解析指令
 
 LRU(弄错了)：注意区分： LFU, LRU， least frequently used, least recently used
一个是使用频率最小，一个是最久使用，
假如最近一次使用A，上一次B，上上一次C,
而A一共访问了3次， B 4次，C 5次
则按照LFU，A是victim
每读一条指令：更新访问时间，对E个lines更新访问时间，只要line有效及时间大于当前line就将其access time减1.最后令当前访问的line为E，即表示最新访问的。

 接着利用区块技术来进行优化矩阵转置函数：增加命中.conflict miss（对角线元素）

调试：先对小traces进行调试（通过pencil and paper可以知道答案的）

LRU：
写一个脚本来帮助重复输入命令
```
initCaches(line *caches, int s, int E)
{ accessTime=0;}

void updateAccessTime(line *caches, int setInd, int lineNo)
{
	for(int i = 0; i < E; i++)
	{
		if(caches[setInd*E + i].validBit == 1 &&
					caches[setInd*E + i].accessTime >
					 (*(caches + setInd*E + lineNo)).accessTime)
		{
			--caches[setInd*E + i].accessTime;
		}
	}
	(*(caches + setInd*E + lineNo)).accessTime = E;
}
```




## shell lab 

### 进程相关，exception
1. 进程
    * 进程的概念及进程的两个抽象
    * 并发程序，并行程序，进程切换
    * 进程的三个状态：running（执行中、准备执行并最终一定会被scheduled）、stopped（被挂起，若没有SIGCONT信号不会被scheduled）、terminated（永久stopped，触发原因：收到信号SIGINT？从main
exit()）
    * terminated 的进程叫做僵尸进程，仍然占用系统资源，需要收割（sigchld_handler调用pid = waitpid()来使父进程收割，父进程则通过全局变量pid的置位与否来确定挂起等待前台进程还是继续进行）
    * 若某个子进程terminated缺父进程早死了，则有init来repeap
    * 上下文：环境变量字符串，参数字符串，某些库的栈帧

几个系统函数：exit

* void exit(int status)
    * 被调用一次，不会返回

* int fork (void)
    * 返回两遍，0即为子进程，pid即为父进程，pid为子进程的pid
    * 子进程将会获得一份父进程的私有虚拟内存空间的copy，文件描述符的copy

int wait
父进程收到SIGINT信号，即使用wait(int *child_status)，挂起当前进程（父进程，即python），把子进程收割完，再返回。检测 child_status，可知道是否子进程由于不明信号导致的终止

int execve 
* called once never return except if there is an eror

        
2. 进程上下文


3. 信号（small message that n）

发送：内核通过目标进程的上下文的状态来发送信号（内核检测到事件，如Ctrl-D；进程通过kill来触发内核来

接受：目标进程被内核强制以某种方式来回应某个信号（）

信号不排序，任何

命令行：
ps来

sig handler guild line
* 简单最好
* 避免使用非同步安全函数（printf,sprintf, malloc），保证同步，即保证在访问共享数据时阻塞，避免race。
* 通过屏蔽所有信号来同步共享数据的访问.addjob, deletejob
* 全局变量用volatile来声明
* global flag用volatile sig_atomic_t来声明（只有读写操作，flag=1 而非flag++）只需一条指令

### 实验内容

`ps`查看进程， `top`查看进程和线程等等

实现一个shell：
取决于用户的交互式的命令行解释器，重复输出一个提示信息（>）stdin的命令行，解析命令行并执行动作。

解析命令行：built-in command(or pathname of executable file) arguments

如果是built-in command 立即执行；
否则fork出子进程（job）来执行，父进程shell则waitpid，事实上可通过pipe执行多个job（管道通讯），支持IO重定向（dup2）；job 可后台或前台。built-incommand有quit，jobs，fg， bg

shell提供job control：
* jobs, kill, bg, fg, 
* change the process state(running,stopped, terminated),Ctrl-C(SIGINT, terminate，包括该进程的子进程), Ctrl-Z(SIGTSTP, stop the foreground job，包括该进程的子进程), 唤醒stopped程序。
* 支持process ID， job ID， 来指定特定job
* 收割所有僵尸子进程，如果一个进程因为收到一个未catch到的信号而结束的话，shell应该打印出该jobID和offending信号。WIFSIGNALED

注意点：

handler guideline

* 何时需要Sipromask，为了同步，其中有addjob,deletejob
* 用全局变量 volatile pid_t pid来实现waitfg
* waitpid(WUNTRACED WNOHANG), kill, fork ,execve, setpgid, sigpromask.  
* kill 用 -pid
* SIGINT, SIGTSTP handlers
* waitfg 用一个busy loop around the sleep function; sigchld_handler 调用一次waitpid
* eval解析命令时，在fork子进程时，需要锁住SIGCHLD信号然后解锁（防止race，父进程收到SIGCHILD信号，child被sigchld_handler收割, 加入一个已经remove的job，或者delete一个还未add的job），接着用addjob来将子进程加入joblist时时又要锁住所有信号。因为子进程继承父进程blocked vector，所以需要解锁SIGCHILD后才能执行program， 每次addjob都要block SIGCHLD（）
* 因为myshell 是unix shell的子进程，一旦按Ctrl-C就会将myshell及其所有的foreground group中的子进程（jobs）给杀掉。在fork后execve前，myshell的子进程job给setpgid(0,0)，即将子进程放入新的进程组，组号是它的pid，这样保证myshell是Unix shell的foreground process group的位移进程，当收到SIGINT信号（Ctrl-C）时，unix shell就会将信号传给正确的process group


## proxy lab

### 知识复习

#### 网络编程：

* 是一组函数，与I/O结合在一起，用以创见网络应用。从Linux内核的角度看，一个套接字就是通信的一个端点。从Linux的程序角度看，套接字就是一个有相应描述符的打开文件。
* echo客户端的示例，见如下代码：
* EOF( _CSAPP_, P665)：并没有EOF字符这个东西，它是一种由内核检测到的一种条件：  应用程序在它接收到一个由read函数返回的零返回码时；磁盘文件的当前文件位置超出文件长度时；因特网连接中，一个进程关闭连接的一端时，当另外一端试图读取流中最后一个字节之后的字节时，会检测到EOF。
* Web基础：HTML（包含指针，即超链接），静态内容、动态内容
* HTTP事务： 见网络
* 服务动态内容：CGI（通用网关接口）解决参数传递，子进程生成内容的传递（类似于管道？），子进程的标准输出。
    服务器fork出子进程，子进程根据URL设置QUERY_STRING环境变量（cgi 程序形参数），执行/cgin-bin/adder，dup2（负责输出），父进程不知子进程生成的内容，所以由子进程负责生成响应报头及写回客户端
* mmap：serve_static中，首先打开文件，接着用将该文件描述符将此文件的前n个字节映射到进程的私有虚拟内存中，一旦映射到内存，就不需要它的描述符了，可以将其关闭，最后再释放映射的虚拟内存
    
#### 并发编程（服务器）

* 并发(concurrent)：逻辑流在时间上相互重叠，不一定是并行(parallel)，并行是并发的真子集。
* 并发程序：使用应用及并发的应用程序
* 三种基本的构造并发程序的方法：进程（内核调度）；I/O多路复用（应用程序调度）；线程（内核调度）

3. 基于进程

* 进程间的通信：进程有独立的虚拟内存空间，要和其他流通信，就需要显式的进程间的**通信机制**：ICP：管道，信号，先进先出FIFO，系统V共享内存，以及系统V信号量（semaphore））
* 父进程与子进程必须各自不对应的套接字描述符副本，避免内存泄漏；像shell lab中每次尽可能回收更多的zombie
* 优点：私有虚拟内存(不怕相互覆盖)，IPC通信使其较慢，开销高；进程切换开销高（上下文多）
* 上下文：栈、数据、堆、共享库等

4. I/O多路复用

* 应用程序在一个进程的上下文显式地调度他们的逻辑流，逻辑流被模拟化成状态机，数据到达文件描述符后，主进程从切换状态。
* Select要求内核挂起进程，直到描述符集合（不同类型的IO流，如终端输入，和客户端请求）中任意一个IO事件发生，控制权交还服务器程序处理事件请求，更细粒度的多路复用解决一个IO事件处理事件过长的使另外一个事件等待时间缩短。
* 并发事件驱动服务器，客户端池，处理终端；客户端请求（accept, add_clients, check_clients，written数据回去）

5. socket:
[Linux IO模式及 select、poll、epoll详解][13]
用户空间与内核空间；
* 进程切换；
* 缓存IO（数据准备在内核的缓冲区，复制数据到进程的虚拟内存空间）；
* 进程的阻塞（正在执行的进程期待的事情未发声，则（系统。。阻塞原语）；
* 使自己由运行状态变为阻塞状态，进程的阻塞是进程自身的一种主动行为，也因此只有处于运行态的进程（获得CPU），才可能将其转为阻塞状态。当进程进入阻塞状态，是不占用CPU资源的），
[soket.md][14]
[五大I/O模型比较][15]
* 阻塞式 I/O
* 非阻塞式 I/O
* I/O 复用（select 和 poll）
* 信号驱动式 I/O（SIGIO）
* 异步 I/O（AIO）

Buffer I/O implementation:
```
// read from socket to kernal
#define RIO_BUFSIZE 8192
typedef struct {
    int rio_fd;/* Descriptor for this internal buf */
    int rio_cnt; /* Unread bytes in internal buf */
    char *rio_bufptr;/* Next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE];/* Internal buffer */
} rio_t;

//copy from kernal to usrbuf, i.e. in the vm of process

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    int n, rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) { 
        if ((rc = rio_read(rp, &c, 1)) == 1) {
	    *bufp++ = c;
	    if (c == '\n') {
                n++;
     		break;
            }
	} else if (rc == 0) {
	    if (n == 1)
		return 0; /* EOF, no data read */
	    else
		break;    /* EOF, some data was read */
	} else
	    return -1;	  /* Error */
    }
    *bufp = 0;
    return n-1;
}
```
[I/O多路复用][16]，[epoll源码][17]

3. 比较 
select 和 poll 的功能基本相同，不过在一些实现细节上有所不同。

- select 会修改描述符，而 poll 不会；
- select 的描述符类型使用数组实现，FD_SETSIZE 大小默认为 1024，因此默认只能监听 1024 个描述符。如果要监听更多描述符的话，需要修改 FD_SETSIZE 之后重新编译；而 poll 的描述符类型使用链表实现，没有描述符数量的限制；
- poll 提供了更多的事件类型，并且对描述符的重复利用上比 select 高。
- 如果一个线程对某个描述符调用了 select 或者 poll，另一个线程关闭了该描述符，会导致调用结果不确定。

select 和 poll 速度都比较慢。

- select 和 poll 每次调用都需要将全部描述符从应用进程缓冲区复制到内核缓冲区。
- select 和 poll 的返回结果中没有声明哪些描述符已经准备好，所以如果返回值大于 0 时，应用进程都需要使用轮询的方式来找到 I/O 完成的描述符。

- epoll事先通过epoll_ctl()来注册一 个文件描述符，一旦基于某个文件描述符就绪时，内核会采用类似callback的回调机制，迅速激活这个文件描述符，当进程调用epoll_wait() 时便得到通知。(此处去掉了遍历文件描述符，而是通过监听回调的的机制。这正是epoll的魅力所在。)
- 监听描述符数量不受限制，epoll不同于select和poll轮询的方式，而是通过每个fd定义的回调函数来实现的。只有就绪的fd才会执行回调函数。

============

5. 线程

* 线程是运行在一个单一进程上下文的逻辑流。其他两个混合体（系统调度逻辑流，像IO多路复用共享虚拟内存空间）
* 线程上下文：线程ID、栈、栈指针、程序计数器、通用目的寄存器
* **与进程的区别：** 上下文小，切换快；线程组成一个对等线程池，而非进程的严格的父子关系
* 分离线程：非分离进程需要由其他进程回收，分离的进程可终止后由系统回收，因此应分离，避免内存泄漏
* **竞争** ：线程例程的赋值语句与主线程中的赋值语句形成竞争。分配独立空间（Malloc）
    ```
    // main
    connfdp = Accept();
    // thread()
    int connfd = *((int *)vargp);
    ```
* 用信号量同步线程：
    * 实现互斥，互斥锁
    ```
    volatile long cnt = 0;
    sem_t mutex;
    ```
    * 调度共享资源：
        * 生产者消费者（请求描述符）：互斥访问缓冲区，slots和items分别记录空槽位和可用项目的数量。
        * 读写锁（读者优先，写者优先）：代码见下面
* 线程安全：
    * 线程不安全：不保护共享变量；当期调用依赖前一调用的中间结果的函数（若未同步发生race则依赖无效）；返回指向静态变量的指针的函数（返回之前，静态变量被其他线程悄悄覆盖了，通过加锁复制解决）；调用线程不安全函数导函数
    * 安全函数：可重入函数：函数内不访问共享数据（生成随机数的rand形参改为非共享指针而非共享next_seed）
    * 死锁


```
/* 
mutex同步对readcnt的访问, w同步写者对缓冲区的访问, 而读者中只有第一个有P(&w),最后一个有V(&w)，即非第一个读者进 入时不会减w，也就不会导致进入禁止区，任意进入；一旦第一个读者 进入，w变成0，只有读者离开，写者才能写，如果一直有读者进入，一直w都是0，写者就必须一直等待
*/
int reader(int serverfd, char *uri) {
    int hit= 0;
    P(&mutex);
    readcnt++;
    if (readcnt == 1) {
        P(&w);
    }
    V(&mutex);

    for (int i = 0; i < 10; ++i) {
        if (!strcmp(cache.cache_lines[i].id, uri)) {
            Rio_writen(serverfd, cache.cache_lines[i].object, MAX_OBJECT_SIZE);
            hit = 1;
            break;
        }
    }
    P(&mutex);
    readcnt--;
    if (readcnt == 0) {
        V(&w);
    }
    V(&mutex);
    return hit;
}

void writer(char *uri, char *buf) {
    P(&w);
    strcpy(cache.cache_lines[cache.resident_cnt].id, uri);
    strcpy(cache.cache_lines[cache.resident_cnt].object, buf);
    ++cache.resident_cnt;
    V(&w);
}
```

### 实验内容

实现一个代理，能实现基本功能，并发，缓存。curl来调试，其中有个难点就是readline结束的判断条件应该是EOF，亦即n=Readline(...) == 0的条件。


编译与连接

转换思路，如对如下代码，变量 `encrypted` 是否被使用了,要从逻辑流的角度去看，代码不一定会被执行

```
String encryptPassword(const string& password)
{
  string encrypted;
  
  if(password.length() < MINIMUM_PASSWORD_LENGTH)
  {
    throw logic_error("Password is too short");
  }
  
  do whatever is necessary to place an encrypted version of password in encrypted;
  return encrypted
}
```



## 虚拟内存 Malloc Lab

进程对内存的需求大，进程之间假如共享数据呢？为什么进程需要独立的内存空间？

1. **虚拟内存空间**：$2^{64}$
2.  **VM的作用**：
    * 缓存：将主存作为虚拟内存空间的cache
    * 内存管理：所有进程都有同样形式的地址空间，如不同区域的起始地址是一致的，static data等。
    * 内存保护：每个page有相应的权限码，孤立进程的地址空间，进程间不能访问相互的地址空间，用户进程不能访问内核数据和代码

3. **主存作为cache**：
    * 类似CPU的缓存，利用了locality，其中hit，miss（page fault, sig handler, reexecute）.
4. **内存管理**：按页分配，进程可共享某个文件（虚拟内存映射），进程的内存分配（虚拟地址空间如code、data、heap一致，有占用所有内存的抽象，简化linking；简化loading，分配）

5. **VP translation**（磁盘存取慢，为减少miss的penalty而设计）:
multi-level page table（一个地址有多个VPN，virtual page number），MMU，MMU和主存间的L1 cache for page，TLB（地址缓存）

6. **Virtual Address Space** of a Linux Process(通过mm_struct, vm_area_struct 链表来管理Area，如Data，text)
内核虚拟内存、用户栈、映射过来的共享库、堆、未初始化的数据(.bss) 、已初始化的数据、程序文本

7. **内存映射** read, copy-on-write, mmap， munmap
可通过mmap， munmap或者sbrk(堆的生长和收缩)来实现malloc动态内存分配器

8. **Allocator**：动态内存管理：显式分配器；隐式分配器
分配器的specification：内存对齐()、只使用堆等。
目标：最大化吞吐率（时间）；最大化内存利用率（空间）
碎片：内部（取决于过去，由padding，隐式指针的head和foot等引起）；外部取决过去操作和未来操作（要存的对象小于内存块）

* 操作：free block组织；放置，分割，合并

* 组织1：单向或双向的隐式链表；空闲链表是真个内存块链表的子集
        * pro：简单；cons：时间O(n)，空间（由于对齐原因如8bytes，每个块不论payload大小，均需要改大小以上）
        * 维护：结构：header, payload, footer
        * 放置操作：first fit; next fit; best fit(时间空间各有优缺点)
        * 分割空闲块
        * 合并：根据footer和header来判断前后能否合并

* 组织2：显式链表：空闲块中存储两指针(pred, next)指向前向空闲块和后继空闲块:。
        * pro：时间减少到（O(#空间块)）cons：空间块足够大包含next，pred指针；链表操作比较复杂，
        * 维护：后进先出（新释放的块放于表头，链表的断裂与连接操作）；基于红黑树以地址为key来实现

* 组织3：分离的空闲链表
        * 维护：多个不同大小的链表；
        * 插入操作：基于sizeof(object)从小到大找对应的链表，然后遍历，找出合适的block，插入后将多余的bloc加入到其他大小的free list中，若无合适的block往更大的找

* 垃圾回收：（C/C++保守的垃圾收集器，不能维持垃圾回收有向图的精确表示，即可达与不可达的结点标记可能相反，java、python，不需要显式free）
        *  基本概念：无论何时需要malloc，若没有空间，垃圾收集器就去识别垃圾并调用free，垃圾收集器返回时malloc重试，否则扩展堆
        *  Mark & Sweep：从多个根节点开始用dfs来mark可达结点，接着扫描所有block，未mark的就是垃圾，不可达，调用free
        *  C语言的Mark&Sweep：对isPtr没有明显的方式来判断其输入指针为真正的指针(可能是某个block中的float类型数据伪装成的指针)，防止出错，保守地将一些不可达标记为可达；，或者其指向一个allocated block ，所以采用保守的Mark&Sweep方式。

9. 内存泄漏：



10. 常见内存错误：C，
```
////////////// 越界等//////////////
////////////// 内存未分配失败//////////////
int *i=malloc(sizeof(int));//失败的话 i==NULL;
*i=4;
////////////// dereferencing bad pointer //////////////
int val;
...
scanf("%d",val); // scanf("%d", &val);
////////////// buffer overflow //////////////
////////////// 读未初始化的指针//////////////
/* return y = Ax */
int *matvec(int **A, int *x) {  int *y = malloc(N*sizeof(int)); int i, j;
for (i=0; i<N; i++) for (j=0; j<N; j++)
y[i] += A[i][j]*x[j]; return y;
}
//////////////覆盖内存//////////////
// 分配错误大小
int **p;
p = malloc(N*sizeof(int));
for (i=0; i<N; i++) {
    p[i] = malloc(M*sizeof(int));
}
// buffer overflow
char s[8];
int i;
gets(s);
//////////////指向不存在的对象//////////////
// 指向悬挂指针
int *foo () {
    int val;
    return &val;
}
///////////多次free一个block或者忘记free/////////////
x = malloc(N*sizeof(int));
<manipulate x> free(x);
...
y = malloc(M*sizeof(int)); for (i=0; i<M; i++)
y[i] = x[i]++;
//////////////引用已free的block//////////////
x = malloc(N*sizeof(int));
<manipulate x> free(x);
...
y = malloc(M*sizeof(int)); 
for (i=0; i<M; i++)
    y[i] = x[i]++;
//////////////未free全//////////////
struct list {
    int val;
    struct list *next;
};
foo() {
    struct list *head = malloc(sizeof(struct list));       head->val = 0;
    head->next = NULL;
    <create and manipulate the rest of the list>
    ...
    free(head); return;
}

```
        
