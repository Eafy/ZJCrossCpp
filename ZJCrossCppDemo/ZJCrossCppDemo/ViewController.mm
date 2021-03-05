//
//  ViewController.m
//  ZJCrossCppDemo
//
//  Created by eafy on 2021/3/4.
//

#import "ViewController.h"
#import "CNetRequestGet.hpp"
#import "CNetRequestPost.hpp"
#import "CNetRequestDownload.hpp"

@interface ViewController ()

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


@end
