//
//  Logger.h
//  dir
//
//  Created by Daniel Bonates on 09/11/17.
//  Copyright © 2017 Daniel Bonates. All rights reserved.
//

#ifndef Logger_h
#define Logger_h


#endif /* Logger_h */

#import <Foundation/Foundation.h>

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KWHT  "\x1B[37m"
#define KBLDGRN   "\033[1m\033[32m"
#define RESET   "\033[0m"

#define print(msg) printf("\n%s\n", msg)

@interface Logger: NSObject
+ (void)printHelp;
+ (void)printIntro;
+ (void)printInvalidArgument;
//- (void)printWrongParameters;

@end

@implementation Logger


+ (void)printIntro {
    printf("\n%sdir: a handy tool for those who are attached to a terminal looking for folders!%s%s",KBLDGRN,RESET, KGRN);
    printf("\nfrom https://github.com/dbonates/dir");
    printf("\nby Daniel Bonates © 2017. http://bonates.com");
    [self printHelp];
}


+ (void)printHelp {
    printf("%s", KWHT);
    printf("\n\nusage:");
    printf("\n\n-s <alias>");
    printf("\n  will SAVE the current directory using alias.");
    printf("\n\n-d <alias>");
    printf("\n  will DELETE specified alias.");
    printf("\n\n-l");
    printf("\n  will list all saved aliases.\n\n");
}


+ (void)printReport: (NSMutableDictionary *)dirs {
    
    __block NSString *report = @"\nCurrent saved dirs:";
    NSString *lineStr = [@"\n" stringByPaddingToLength:report.length + 1 withString: @"-" startingAtIndex:0];
    report = [report stringByAppendingString: lineStr];
    
    [dirs enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL * stop) {
        
        NSString *dirInfo = [NSString stringWithFormat: @"\n%s%@ %s⇢ %@", KYEL, key, KWHT, obj];
        report = [report stringByAppendingString: dirInfo];
        
    }];
    
    report = [report stringByAppendingString: [NSString stringWithFormat: @"%@\n", lineStr]];
    
    printf("\n%s\n\n", [report UTF8String]);
    
}


+ (void)printInvalidArgument {
    printf("\n%sInvalid argument, but here's some help:%s", KRED, KWHT);
    [self printHelp];
}



+ (void)printWrongParameters {
    printf("\n%sInvalid parameters for this options, please specify one!%s", KRED, KWHT);
    [self printHelp];
}



+ (void)log: (NSString *)info {
    printf("%s", [info UTF8String]);
}

@end

