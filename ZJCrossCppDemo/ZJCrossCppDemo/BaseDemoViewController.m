//
//  BaseDemoViewController.m
//  ZJCrossCppDemo
//
//  Created by eafy on 2021/3/15.
//  Copyright © 2021 ZJ. All rights reserved.
//

#import "BaseDemoViewController.h"

@interface BaseDemoViewController ()

@end

@implementation BaseDemoViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (ZJButton *)btnWithTitle:(NSString *)title preBtn:(nullable ZJButton *)btn toClass:(nullable Class)cls
{
    ZJButton *btnT = [[ZJButton alloc] initWithFrame:CGRectMake(30, btn ? btn.zj_bottom + 15 :120, kZJScreenWidth -60, 48)];
    btnT.style = ZJButtonStyleColor;
    btnT.norTitle = title;
    
    if (cls) {
        [[btnT rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
            ZJBaseViewController *vc = [[cls alloc] init];
            vc.title = btnT.norTitle;
            [self.navigationController pushViewController:vc animated:YES];
        }];
    }
    
    return btnT;
}

- (ZJButton *)btnWithTitle:(NSString *)title preBtn:(nullable ZJButton *)btn handle:(void (^)(id x))nextBlock
{
    ZJButton *btnT = [[ZJButton alloc] initWithFrame:CGRectMake(30, btn ? btn.zj_bottom + 15 :120, kZJScreenWidth -60, 48)];
    btnT.style = ZJButtonStyleColor;
    btnT.norTitle = title;
    
    if (nextBlock) {
        [[btnT rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
            nextBlock(x);
        }];
    }
    
    return btnT;
}

@end
