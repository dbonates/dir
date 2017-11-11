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
@property (nonatomic, retain) NSString *aliasFile;
@property (nonatomic, retain) NSMutableDictionary *allDirs;

@end



@implementation DirManager

@synthesize dbFile;
@synthesize allDirs;


- (NSString *)dbFile {
    NSString *homeDir = NSHomeDirectory();
    return [homeDir stringByAppendingString:@"/.dir_db.plist"];
}

- (NSString *)aliasFile {
    NSString *homeDir = NSHomeDirectory();
    return [homeDir stringByAppendingString:@"/.db_alias"];
}

- (NSString *)bashProfileFile {
    NSString *homeDir = NSHomeDirectory();
    return [homeDir stringByAppendingString:@"/.bash_profile"];
}

- (NSString *)bashProfileFileForBackup {
    NSString *homeDir = NSHomeDirectory();
    NSString *bashfile_bkp = [NSString stringWithFormat: @"/.bash_profile_%@", [[NSProcessInfo processInfo] globallyUniqueString]];
    return [homeDir stringByAppendingString:bashfile_bkp];
}


- (void)createDbFileIfNeeded {
    
    NSFileManager *fm = [[NSFileManager alloc] init];
    
    if(![fm fileExistsAtPath: self.dbFile]) {
        
        NSDictionary *plist = [NSDictionary new];
        BOOL success = [plist writeToFile:self.dbFile atomically:YES];
        
        if (success) {
            printf("db file created!");
        } else {
            printf("error creating plist");
        }
    }
    
    if (self.allDirs == nil) {
        
        NSMutableDictionary *dirs = [[NSMutableDictionary alloc] initWithContentsOfFile: self.dbFile];
        self.allDirs = dirs != nil ? dirs : [NSMutableDictionary dictionary];
        
    }
}

- (void)createAliasFileIfNeeded {
    
    NSFileManager *fm = [[NSFileManager alloc] init];
    
    if(![fm fileExistsAtPath: self.aliasFile]) {
        
        NSError *error;
        BOOL success = [@"" writeToFile:self.dbFile atomically:YES encoding:NSUTF8StringEncoding error: &error];
        
        if (success) {
            printf("alias file created!");
        } else {
            printf("error creating alias file");
        }
    }
    
}



- (int)saveDirWithAlias:(NSString *)alias {
    
    if (![self validAlias: alias shouldExist: NO]) {
        return 1;
    }
    
    NSString *currentpath = [[NSFileManager defaultManager] currentDirectoryPath];
    
    [Logger log:[NSString stringWithFormat: @"\nadding %s%@%s for %@...", KYEL, alias, KWHT, currentpath]];
    
    // add new dir
    [self.allDirs setValue:currentpath forKey:alias];
    
    if ([self updateShell] != 0) {
        [Logger log: @"\nShell not updated. Try it running `dir -u` at end.\n\n"];
    }
    return [self saveUpdatedDirs];
}

- (int)deleteDirWithAlias: (NSString *)alias {
    
    if (![self validAlias: alias shouldExist: YES]) {
        return 1;
    }
    
    [Logger log:[NSString stringWithFormat: @"\nremoving %s%@ %s...", KYEL, alias, KWHT]];
    
    [self.allDirs removeObjectForKey: alias];
    
    if ([self updateShell] != 0) {
        [Logger log: @"\nShell not updated. Try it running `dir -u` at end.\n\n"];
    }

    return [self saveUpdatedDirs];
}

- (int)updateShell {
    
    __block NSString *dirList = @"";
    
    [allDirs enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL * stop) {
        
        NSString *dirInfo = [NSString stringWithFormat: @"alias %@='cd %@'\n", key, obj];
        dirList = [dirList stringByAppendingString: dirInfo];
        
    }];
    
    [self createAliasFileIfNeeded];
    
    [self updateBashProfileIfNeeded];
    
    
    NSError *error;
    
    BOOL success = [dirList writeToFile: self.aliasFile atomically:YES encoding:NSUTF8StringEncoding error:&error];
    
    if (!success) {
        [Logger log: @"\ncouldn't save alias file, exiting...\n"];
        return 1;
    }
    
    [Logger log: @"\nalias file updated.\n\n"];
    
    system("source ~/.bash_profile");
    
    return 0;
}
                              
                              
- (void)updateBashProfileIfNeeded {
      
      NSError *error;
      NSString *bashProfileContents = [NSString stringWithContentsOfFile: [self bashProfileFile] encoding:NSUTF8StringEncoding error: &error];
      
      if (error != nil) {
          [Logger log: @"\nerror getting .bash_profile\n"];
          return;
      }
      
      NSString *injectString = @"source ~/.db_alias";
      
      if ([bashProfileContents rangeOfString:injectString].location == NSNotFound) {
          
          BOOL success = [bashProfileContents writeToFile: [self bashProfileFileForBackup] atomically:YES encoding:NSUTF8StringEncoding error:&error];
          
          if (!success) {
              [Logger log: @"\ncouldn't backup .bash_profile file, too dangerous to continue, exiting...\n"];
              return;
          }
          
          bashProfileContents = [bashProfileContents stringByAppendingString:[NSString stringWithFormat:@"\n%@\n", injectString]];
          success = [bashProfileContents writeToFile:[self bashProfileFile] atomically:YES encoding:NSUTF8StringEncoding error:&error];
          
          if (!success) {
              [Logger log: @"\ncouldn't save .bash_profile file, too dangerous to continue, exiting...\n"];
              return;
          }
          
          [Logger log: @"\n.bash_profile updated\n"];
          
      }
                                  
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


- (BOOL)validAlias: (NSString *)alias shouldExist:(BOOL)shouldExist {
    
    if (alias == Nil || alias.length < 1) {
        [Logger printWrongParameters];
        return NO;
    }
    
    BOOL aliasInUse = [[self.allDirs allKeys] containsObject: alias];
    
    if (aliasInUse) {
        
        if(shouldExist) { //should exist to proceed
            return YES;
            
        } else { // alias in use cannot do save operation
            [Logger log: [NSString stringWithFormat:@"\nthe alias %s%@%s is already in use, please choose another.\n\n", KYEL, alias, KWHT]];
            return NO;
        }
        
    } else {
        
        if(shouldExist) { //should exist to proceed
            
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
