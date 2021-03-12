//
//  ViewController.m
//  ZJCrossCppDemo
//
//  Created by eafy on 2021/3/4.
//

#import "ViewController.h"
#import <ZJBaseUtils/ZJBaseUtils.h>
#import "CNetRequestGet.hpp"
#import "CNetRequestPost.hpp"
#import "CNetRequestDownload.hpp"

using namespace ZJ;

@interface ViewController ()

@property (nonatomic,strong) UIScrollView *scrollView;

@property (nonatomic,assign) CNetRequestGet *requestGet;
@property (nonatomic,assign) CNetRequestPost *requestPost;
@property (nonatomic,assign) CNetRequestDownload *requestDownload;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.requestGet = new CNetRequestGet();
    self.requestGet->AddParameter("ver", 2);
    self.requestGet->AddParameter("method", "sendInstruction");
    self.requestGet->AddParameter("devKey", "cd15d1aba85346128811ae17fc2a2378");
    self.requestGet->AddParameter("devSecret", "a7866ef45d594ea988554fe633fa987e");
    self.requestGet->AddParameter("uuid", "983135884798102");
    self.requestGet->AddParameter("proNo", 128);
    self.requestGet->AddParameter("cmd", "%7B%22appid%22%3A%22IE444A73F0FB100%22%2C%22cmd%22%3A256%7D");
    self.requestGet->AddParameter("token", "123456");
    self.requestGet->AddParameter("isw", 3);
    self.requestGet->AddParameter("appId", "IE444A73F0FB100");
    self.requestGet->Start("http://live.jimivideo.com/srs-os/api");
    
    
//    self.requestPost = new CNetRequestPost();
//        self.requestPost->AddParameter("email", "lizhijian_21@163.com");
//        self.requestPost->AddParameter("pass", "123456");
//        self.requestPost->Start("http://api.securesmart.cn/cameraServer2/v2/userLogin.api");
    
//    self.requestDownload = new CNetRequestDownload();
//    NSString *path = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, true).firstObject;
//    NSString *fileDir = [NSString stringWithFormat:@"%@/TEST.zip", path];
//    self.requestDownload->Start("www.caimomo.com.cn/download/oem/cy.zip", fileDir.UTF8String);
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

+ (ZJButton *)demoTyleBtn:(ZJButton *)btn {
    ZJButton *btn1 = [[ZJButton alloc] initWithFrame:CGRectMake(30, btn ? btn.zj_bottom + 15 :120, kZJScreenWidth -60, 48)];
    btn1.style = ZJButtonStyleColor;
    return btn1;
}

- (void)bindTouchUpInside:(ZJButton *)btn toClass:(Class)cls
{
    [btn zj_addSingleTap:^(UITapGestureRecognizer * _Nonnull obj) {
        ZJBaseViewController *vc = [[cls alloc] init];
        vc.title = btn.norTitle;
        [self.navigationController pushViewController:vc animated:YES];
    }];
}

@end
