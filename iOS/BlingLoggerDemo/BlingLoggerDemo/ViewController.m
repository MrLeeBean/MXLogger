//
//  ViewController.m
//  BlingLoggerDemo
//
//  Created by 董家祎 on 2022/3/22.
//

#import "ViewController.h"
#import <BlingLogger/BlingLogger.h>
@interface ViewController ()


@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [BlingLogger shareManager].fileHeader = @"版本号:1.0.0 平台:iOS";
    [BlingLogger shareManager].maxDiskAge = 60 * 60 * 24 * 7; // 一个星期
    [BlingLogger shareManager].maxDiskSize = 1024 * 1024 * 100; // 100M
    
    NSString * path = [BlingLogger shareManager].diskCachePath;
    NSLog(@"日志文件路径:%@",path);
    
    /**下面这些设置都是默认设置 不写也行 **/
    
    [BlingLogger shareManager].storagePolicy = @"yyyy_MM_dd";
    
    [BlingLogger shareManager].shouldRemoveExpiredDataWhenEnterBackground = YES;
    
    [BlingLogger shareManager].shouldRemoveExpiredDataWhenTerminate = YES;
    
    [BlingLogger shareManager].fileName = @"blinglog";
    
    [BlingLogger shareManager].consoleLevel = 0;
    /// 默认情况下 debug数据不会写入到文件中 如果设置 [BlingLogger shareManager].fileLevel = 0 那么debug数据才会写入到日志文件
    [BlingLogger shareManager].fileLevel = 1;
    
    //设置为NO 控制台不再打印数据
    [BlingLogger shareManager].consoleEnable = YES;
    
    //设置为NO 不再写入日志文件
    [BlingLogger shareManager].fileEnable = YES;
    
    //控制台输出格式
    [BlingLogger shareManager].consolePattern = @"[%d][%p]%m";
    //问价输出格式
    [BlingLogger shareManager].filePattern = @" [%d][%t][%p]%m";
    
    [BlingLogger shareManager].isAsync = YES;
   
    
    
 
    
    
}
- (IBAction)tenThousandAction:(UIButton*)sender {
    
    /// 同步写入测试
    [BlingLogger shareManager].isAsync = NO;
    
    NSDate * dateStart=   [NSDate dateWithTimeIntervalSinceNow:0];
    NSTimeInterval start =[dateStart timeIntervalSince1970];

    for (int i = 0; i < 100000; i++) {
       
        NSString * string = [NSString stringWithFormat:@"第%d条数据",i];
        [BlingLogger info:string];
    }
    
    NSDate * dateEnd=   [NSDate dateWithTimeIntervalSinceNow:0];
    NSTimeInterval end =[dateEnd timeIntervalSince1970];
    
    [sender setTitle:[NSString stringWithFormat:@"写10万条数据耗时:%f s",end-start] forState:UIControlStateNormal];
    
    NSLog(@"时间:%f",end - start);
    
}



- (IBAction)defaultButtonAction:(id)sender {
    [BlingLogger debug:@"这是一条debug,会输出到控制台"];
    [BlingLogger info:@"这是一条infoLog,会输出到控制台"];
    [BlingLogger warn:@"这是一条warn,会输出到控制台"];
    [BlingLogger error:@"这是一条error,会输出到控制台"];
    [BlingLogger fatal:@"这是一条fatal,会输出到控制台"];
   
}
- (IBAction)syncLogAction:(id)sender {
    
    [BlingLogger syncLogFile:NULL level:1 msg:@"这是同步方法，只写入文件不会输出到控制台" tag:NULL];
}
- (IBAction)asyncLogAction:(id)sender {
    [BlingLogger asyncLogFile:NULL level:1 msg:@"这是异步方法，只写入文件不会输出到控制台" tag:NULL];
}
- (IBAction)logSizeAction:(UIButton*)sender {
    NSUInteger logSize =   [BlingLogger shareManager].logSize;
      double size =  logSize/1024.0/1024.0;
  
    [sender setTitle:[NSString stringWithFormat:@"日志大小:%0.2fM",size] forState:UIControlStateNormal];
}


@end
