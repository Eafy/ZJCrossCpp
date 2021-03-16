//
//  AppDelegate.m
//  ZJCrossCppDemo
//
//  Created by eafy on 2021/3/4.
//  Copyright © 2021 ZJ. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    ZJBaseNavigationController *nav = [[ZJBaseNavigationController alloc] initWithRootViewController:[[ViewController alloc] init]];
    nav.backgroundColor = ZJColorFromRGB(0xE8ECF1);
    
    self.window = ([[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]]);
    [self.window makeKeyAndVisible];
    self.window.rootViewController = nav;
    
    return YES;
}

@end
