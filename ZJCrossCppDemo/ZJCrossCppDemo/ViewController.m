//
//  ViewController.m
//  ZJCrossCppDemo
//
//  Created by eafy on 2021/3/4.
//  Copyright © 2021 ZJ. All rights reserved.
//

#import "ViewController.h"
#import "NetViewController.h"

@interface ViewController ()

@property (nonatomic,strong) UIScrollView *scrollView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.navLeftBtn.hidden = YES;
    
    ZJButton *btn = [self btnWithTitle:@"网络模块" preBtn:nil toClass:[NetViewController class]];
    [self.scrollView addSubview:btn];

    self.scrollView.contentSize = CGSizeMake(self.view.zj_width, btn.zj_bottom + 50);
}

- (UIScrollView *)scrollView
{
    if (!_scrollView) {
        _scrollView = [[UIScrollView alloc] initWithFrame:self.view.bounds];
        _scrollView.contentSize = self.view.bounds.size;
        _scrollView.pagingEnabled = NO;
        _scrollView.scrollEnabled = YES;
        _scrollView.bounces = NO;
        _scrollView.showsVerticalScrollIndicator = YES;
        _scrollView.showsHorizontalScrollIndicator = NO;
        _scrollView.backgroundColor = self.view.backgroundColor;
        if (@available(iOS 11.0, *)) {
            _scrollView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
        }
        [self.view addSubview:_scrollView];
    }
    
    return _scrollView;
}

@end
