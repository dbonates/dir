//
//  main.m
//  dir
//
//  Created by Daniel Bonates on 09/11/17.
//  Copyright © 2017 Daniel Bonates. All rights reserved.
//

#import <AppKit/AppKit.h>
#import "DirManager.h"
#import "Logger.h"


int main(int argc, const char * argv[]) {
    @autoreleasepool {
        
        DirManager *manager = [[DirManager alloc] init];
        [manager createDbFileIfNeeded];
        
        NSArray *arguments = [[NSProcessInfo processInfo] arguments];
        
        if (arguments.count < 2) {
            [Logger printIntro];
            return 1;
        }
        
        
        NSString *commandArgument = [arguments objectAtIndex: 1];
        NSString *requestedAlias = arguments.count < 3 ? Nil : [arguments objectAtIndex: 2];
        
        
        if ([commandArgument isEqualToString:@"-l"]) {
            
            NSMutableDictionary *allDirs = [manager allDirs];
            [Logger printReport:allDirs];
            return 0;
            
        }
        else if ([commandArgument isEqualToString:@"-s"]) {
            
            return [manager saveDirWithAlias: requestedAlias];
        }
        else if([commandArgument isEqualToString:@"-d"]) {
            
            return [manager deleteDirWithAlias: requestedAlias];
            
        } else {
            
            [Logger printInvalidArgument];
            return 1;
        }
            
    }
    
    return 0;
}

