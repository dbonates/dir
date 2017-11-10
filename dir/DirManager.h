//
//  DirManager.h
//  dir
//
//  Created by Daniel Bonates on 10/11/17.
//  Copyright © 2017 Daniel Bonates. All rights reserved.
//

#ifndef DirManager_h
#define DirManager_h


#endif /* DirManager_h */

#import "Logger.h"

@interface DirManager: NSObject

- (void)createDbFileIfNeeded;

@property (nonatomic, retain) NSString *dbFile;
@property (nonatomic, retain) NSMutableDictionary *allDirs;

@end



@implementation DirManager

@synthesize dbFile;
@synthesize allDirs;


- (NSString *)dbFile {
    NSString *homeDir = NSHomeDirectory();
    return [homeDir stringByAppendingString:@"/.dir_db.plist"];
}


- (void)createDbFileIfNeeded {
    
    NSFileManager *fm = [[NSFileManager alloc] init];
    
    if(![fm fileExistsAtPath: self.dbFile]) {
        
        NSDictionary *plist = [NSDictionary new];
        BOOL success = [plist writeToFile:self.dbFile atomically:YES];
        
        if (success) {
            printf("db file saved!");
        } else {
            printf("error saving plist");
        }
    }
    
    if (self.allDirs == nil) {
        
        NSMutableDictionary *dirs = [[NSMutableDictionary alloc] initWithContentsOfFile: self.dbFile];
        self.allDirs = dirs != nil ? dirs : [NSMutableDictionary dictionary];
        
    }
}

- (int)saveDirWithAlias:(NSString *)alias {
    
    if (![self validAlias: alias deleting: NO]) {
        return 1;
    }
    
    NSString *currentpath = [[NSFileManager defaultManager] currentDirectoryPath];
    
    [Logger log:[NSString stringWithFormat: @"\nadding %s%@%s for %@...", KYEL, alias, KWHT, currentpath]];
    
    // add new dir
    [self.allDirs setValue:currentpath forKey:alias];
    return [self saveUpdatedDirs];
}

- (int)deleteDirWithAlias: (NSString *)alias {
    
    if (![self validAlias: alias deleting: YES]) {
        return 1;
    }
    
    [Logger log:[NSString stringWithFormat: @"\nremoving %s%@ %s...", KYEL, alias, KWHT]];
    
    [self.allDirs removeObjectForKey: alias];
    
    
    return [self saveUpdatedDirs];
}


- (int)saveUpdatedDirs {
    
    BOOL success = [self.allDirs writeToFile: self.dbFile atomically:YES];
    
    if (success) {
        printf("\n%sdone!%s\n\n", KGRN, KWHT);
        return 0;
    } else {
        printf("\n%serror updating plist%s\n\n", KGRN, KWHT);
        return 1;
    }
}


- (BOOL)validAlias: (NSString *)alias deleting:(BOOL)deleting {
    
    if (alias == Nil || alias.length < 1) {
        [Logger printWrongParameters];
        return NO;
    }
    
    BOOL aliasInUse = [[self.allDirs allKeys] containsObject: alias];
    
    if (aliasInUse) {
        
        if(deleting) { //for deleting
            return YES;
            
        } else { // alias in use cannot do save operation
            [Logger log: [NSString stringWithFormat:@"\nthe alias %s%@%s is already in use, please choose another.\n\n", KYEL, alias, KWHT]];
            return NO;
        }
        
    } else {
        
        if(deleting) { //for deleting we need an alias!
            
            [Logger log: [NSString stringWithFormat:@"\nthis alias %s%@%s doesn't exists.\n\n", KYEL, alias, KWHT]];
            return NO;
        }
    }
    
    // dir already saved?
    NSString *currentpath = [[NSFileManager defaultManager] currentDirectoryPath];
    if ([[self.allDirs allValues] containsObject: currentpath]) {
        
        NSString *keyForDir = [[self.allDirs allKeysForObject: currentpath] lastObject];
        [Logger log: [NSString stringWithFormat: @"\nthis dir was alread saved using alias %s%@%s.\n\n", KYEL, keyForDir, KWHT]];
        return NO;
    }
    
    
    return YES;
}


@end
