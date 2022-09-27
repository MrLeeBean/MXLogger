[![license](https://img.shields.io/badge/license-BSD_3-brightgreen.svg?style=flat)](https://github.com/coder-dongjiayi/MXLogger/blob/main/LICENSE.TXT)    [![Platform](https://img.shields.io/badge/Platform-%20iOS%20%7C%20Android%20%7C%20Flutter-brightgreen.svg)](https://github.com/coder-dongjiayi/MXLogger)

<p align="center" >
<img src="./icon/logo_400.png" alt="MXLogger"  title="MXLogger" style="zoom:20%;" />
</p>

# MXLogger

MXLogger 是基于mmap内存映射机制的跨平台日志库，支持AES CFB 128位加密，支持iOS Android Flutter。核心代码使用C/C++实现， Flutter端通过ffi调用，性能几乎与原生一致。 底层序列化使用Google开源的flat_buffers实现，高效稳定。

<!--ts-->

* [起因](#起因)

* [性能测试](#性能测试)

* [MXLogger介绍](#MXLogger介绍)

* [安装引入](#安装引入)

  

<!--te-->


### 整体结构如下

<img src="./icon/jiegoutu.jpg" alt="jiegoutu" style="zoom:30%;" />

MXLogger 目前主要是解决日志写入和日志分析的问题。至于日志上报时机，应该由业务决定，MXLogger已经把日志的路径暴露出来，需要开发调用原生平台的文件上传代码，传到自己的服务器，然后通过自己的服务器下载日志文件，再通过我提供的**mxlogger_analyzer ** 进行解析。如果觉的这个流程麻烦，也可以自己写web端的可视化页面，解析代码可以查看 **mxlogger_analyzer** 的源码。

# 起因

当app用户量特别大的时候，异常的概率也越来越大。过去的一年多里我们收到了很多技术支持反馈的线上用户的问题，而这些问题绝大多数都是我们测试同学无法复现的。这个时候就需要开发同学根据代码逻辑进行推理、猜测、验证。把可能出现异常的地方记录下来，然后进行分析。早期的时候,我们记录日志的方式很原始，就是使用系统自带的api进行写入，产出txt文件。但是这种简单粗暴的方式，随着产品不断迭代，功能的增加，写入日志越来越频繁，日志文件越来越大。单纯依靠Ctrl+F搜索txt文件，很多时候不太容易追踪问题。我希望最终的日志产出能够导入数据库，通过sql进行数据的基本分析。最好能直接在app上看到日志（这样非常方便直接在测试期间查看问题）,于是我决定重新造一个轮子，既能保证写入效率和性能又能导入数据库进行基本的分析。




# 性能测试 

### 对比了一下 美团的 Logan-ios(1.2.2)

下面测试结果从三个维度进行比较 写入速度、内存波动、日志体积。以下仅为iOS端测试结果。

> 测试环境

* 测试设备 iphone8pluse 系统版本 14.0

* 每条数据大小约为104个字节 for循环10万次

* 测试10次，取平均值

> 测试结果

* 写入速度  MXLogger 仅为0.3s 左右，Logan平均在31s左右

  <img src="./icon/haoshi.jpg" alt="haoshi" style="zoom:50%;" />

* 内存波动  下图为写入10万条数据内存波动截图


MXLogger | Logan 
---- | ---
<img src="./icon/mxlogger_ memory.jpg" alt="mxlogger_ memory" style="zoom:25%;" /> | <img src="./icon/logan_ memory.jpg" alt="logan_ memory" style="zoom:25%;" /> 
内存波动比较小 100k左右 | 内存波动比较大 将近40M 


> 日志文件体积

Logan 对数据进行了压缩，实际产生的体积会比较小。 MXLogger 仅仅对数据进行了二进制序列化，会比纯文本小一点。

MXLogger | Logan 
---- | ---
10M左右 | 1M左右




# MXLogger介绍



* 写入原理

  根据``mmap``映射原理，需要准备一页内存用于随时写入数据，操作系统负责将映射的内存写入到文件中，``mmap``的优势就在于不必担心app闪退导致日志丢失的问题。毕竟写日志的操作会贯穿app的整个生命周期。

* 数据信息

  ``MXLogger``写入的数据包括日志等级(``debug`` `` info``  ``warn``  ``error``  ``fatal``) 、日志写入时间(精确到微妙)、线程ID，是否为主线程、tag 以及日志信息等。底层序列化使用google开源的``flatbuffers``，其性能优于protobuf和json

* 设计模式

   MXLogger是使用多例的设计模式，日志存储路径是唯一实例。也就是说同一个存储路径在底层是操作的是同一个对象。在一个大型app中可能多个团队的代码会集成到同一个工程中，这个时候为了避免触碰到其他团队所使用的日志对象，我提供了一个命名空间的参数，通常可以使用域名反转的方式填入。日志操作会贯穿整个app的生命周期，为了使用起来简单方便，我建议使用者在自己的业务层进行单例的封装。

* 存储空间大小与写入效率

   **在存储空间和写入时间之间无法达成一个完美的解决方案，只能选择一个可以接受的折中方案 **

   

   使用flatbuffers序列化之后的二进制数据占用的存储空间会比纯文本要小一点。出于写入性能的考虑，我认为一边压缩数据一边写入并不是一个很好的解决方法。为此，我选择了性能，牺牲了存储空间。但毕竟手机存储空间还是有限的，如果多数的存储空间都被日志文件占用了，这是一个得不偿失的行为，也不可接受。为了缓解手机存储空间的问题我将使用以下两个办法

   1. 设置日志最大存储时长和最大存储大小，超过最大时长或最大存储大小的文件将会被删除。保留距离现在时间最近的日志。
   2. 对已生成的日志文件 进行整体压缩(待实现)

* 最大边界

   MXLogger理论上单条数据最大存储为2^32次方 也就是4GB，但是MXLogger并没有对最大值进行边界检查，如果你的单条数据存储大于了这个最大值，那么当前这个日志文件无法被解析。

* 日志查看

   为了解析产出的二进制数据，我写了一个配套的可以查看的解析工具[mxlogger_analyzer](./mxlogger_analyzer.dmg),他大概长下面这个样子。

| <img src="./icon/mx_1.jpg" alt="mx_1" style="zoom:25%;" /> | <img src="./icon/mx_2.jpg" alt="mx_2" style="zoom:25%;" /> |
|  ----  | ----  |
| <img src="./icon/mx_3.jpg" alt="mx_3" style="zoom:25%;" /> | <img src="./icon/mx_4.jpg" alt="mx_4" style="zoom:25%;" /> |



如果你使用的是Mac电脑你可以直接下载我写好的[客户端](https://github.com/coder-dongjiayi/MXLogger/blob/main/mxlogger_analyzer.dmg)，把dmg里面的app拖到应用程序目录中就可以了。如果你使用的是windows系统，那么你可能需要自行配置Flutter环境，编译mxlogger_analyzer的源码打包出exe文件。因为个人精力有限，我可能只会关注mac端的适配情况。目前这个工具的功能还比较单一，但是满足了基本的分析查询功能，后期我会逐渐完善这个分析工具。如果你有特殊的需求分析，可以直接读取这个SQL数据库,通过操作sql进行自己的定制分析。

mxlogger_analyzer 使用flutter编写，目前全平台支持，你也可以定制代码直接跑在自己的手机app上，实时查看日志数据。或者自己编译web版本部署在自己公司的服务器上。

# 安装引入

## iOS

``` pod 'MXLogger', '~> 0.1.2 '```

## Android

``` implementation 'io.github.coder-dongjiayi:mxlogger:0.1.2'```

### Flutter

没有往[pub](https://pub.flutter-io.cn/)上传，可以[下载源码](https://github.com/coder-dongjiayi/MXLogger/tree/main/flutter_mxlogger) 放到自己公司的git或者作为本地插件的形式集成到flutter工程中



# 快速开始

设置日志存储目录不要设置在一个可能被系统清理的目录 比如说 ios 的```library/cache```下，MXLogger并不会在每次写入数据的时候检测目录是否存在，只会在启动的时候创建，如果再app运行中日志文件被系统清理，程序不会报错，也不会闪退，但是日志也不会被记录。

更详细的使用，请查看demo

> 关于日志加密的说明

 MXLogger 提供AES CFB 128为加密机制，代码来自于[MMKV](https://github.com/Tencent/MMKV/tree/master/Core/aes)，加密逻辑使用汇编实现，最大程度保障了写入性能。加密使用的cryptKey和iv 应为长度16的英文字符串。

## 设置存储策略

> MXLogger 提供按月、周、天、小时，4种存储策略。默认情况下是按天存储。

```objective-c

	yyyy_MM                 按月存储
  yyyy_MM_dd              按天存储
  yyyy_ww                 按周存储
  yyyy_MM_dd_HH     		  按小时存储
```

> 初始化，以及基本使用

* iOS

  ```objective-c
  MXLogger * logger =  [MXLogger initializeWithNamespace:@"com.youdomain.logger.space",storagePolicy:@"yyyy_MM_dd_HH"];
  logger.maxDiskAge = 60*60*24*7; // 一个星期
  logger.maxDiskSize = 1024 * 1024 * 10; // 10M
  logger.fileLevel = 0;// 设置文件写入等级 小于这个等级的日志 不写入文件
  [logger debug:@"mxlogger" msg:@"这是debug信息" tag:@"response"];
  [logger info:@"mxlogger" msg:@"这是info信息" tag:@"request"];
  [logger warn:@"mxlogger" msg:@"这是warn信息" tag:@"step"];
  [logger error:@"mxlogger" msg:@"这是error信息" tag:NULL];
  [logger fatal:@"mxlogger" msg:@"这是fatal信息" tag:NULL];
  ```

  

* Android

  ```java
  MXLogger logger = new MXLogger(this.getContext(),"com.dongjiayi.mxlogger");
  logger.maxDiskAge = 60*60*24*7; // 一个星期
  logger.maxDiskSize = 1024 * 1024 * 10; // 10M
  logger.debug("request","mxlogger","this is debug");
  logger.info("response","mxlogger","this is info");
  logger.warn("tag","mxlogger","this is warn");
  logger.error("404","mxlogger","this is error");
  logger.fatal("200","mxlogger","this is fatal");
  ```

  

* Flutter

  ```dart
   MXLogger logger = await MXLogger.initialize(
          nameSpace: "flutter.mxlogger",
          storagePolicy: "yyyy_MM_dd_HH",
          cryptKey: "abcuioqbsdguijlk",
          iv: "bccuioqbsdguijiv");
  
   logger.setMaxDiskAge(60*60*24*7);
   logger.setMaxDiskSize(1024*1024*10);
   logger.setFileLevel(0);
  
   logger.debug("这是debug数据", name: "mxlogger", tag: "D");
   logger.info("这是info数据", name: "mxlogger", tag: "w");
   logger.warn("这是warn数据", name: "mxlogger", tag: "w");
   logger.error("这是error数据", name: "mxlogger", tag: "e");
   logger.fatal("这是fatal数据", name: "mxlogger", tag: "f");
  
          
  ```
  
  



# 后续版本迭代安排

1. 日志文件压缩 
2. flutter端在插件中使用MXLogger
3. mxlogger_analyzer 支持根据tag搜索、分析数据
4. 强化控制台日志输出


# 参考代码

* [MMKV](https://github.com/Tencent/MMKV)
* [spdlog](https://github.com/gabime/spdlog)
* [log4cplus](https://github.com/log4cplus/log4cplus)
* [SDWebImage](https://github.com/SDWebImage/SDWebImage)
* [KSCrash](https://github.com/kstenerud/KSCrash)

   

   





