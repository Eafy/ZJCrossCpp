//
//  NetViewController.m
//  ZJCrossCppDemo
//
//  Created by lzj on 2021/3/15.
//  Copyright © 2021 ZJ. All rights reserved.
//

#import "NetViewController.h"
#import "CNetRequestDownload.hpp"
#import "CNetRequestUpload.hpp"
#import "CHttpClient.hpp"

using namespace ZJ;

@interface NetViewController ()

@property (nonatomic,assign) CHttpClient *httpClient;

@end

@implementation NetViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    _httpClient = new CHttpClient();
    
    ZJButton *btn = [self btnWithTitle:@"GET" preBtn:nil handle:^(id  _Nonnull x) {
        neb::CJsonObject jsonObj;
        jsonObj.Add("ver", 2);
        jsonObj.Add("method", "sendInstruction");
        jsonObj.Add("devKey", "cd15d1aba85346128811ae17fc2a2378");
        jsonObj.Add("devSecret", "a7866ef45d594ea988554fe633fa987e");
        jsonObj.Add("uuid", "983135884798102");
        jsonObj.Add("proNo", 128);
        jsonObj.Add("cmd", "%7B%22appid%22%3A%22IE444A73F0FB100%22%2C%22cmd%22%3A256%7D");
        jsonObj.Add("token", "123456");
        jsonObj.Add("isw", 3);
        jsonObj.Add("appId", "IE444A73F0FB100");
        
        self.httpClient->Request(CNetRequest::METHOD_TYPE_GET, "http://live.jimivideo.com/srs-os/api", jsonObj, [](void *userData, const std::string strContent) {
            NSLog(@"Get Success：%s", strContent.c_str());
        }, [](void *userData, long statusCode, const std::string strError) {
            NSLog(@"Get Fail[%ld]：%s", statusCode, strError.c_str());
        });
    }];
    [self.view addSubview:btn];
    
    
    btn = [self btnWithTitle:@"POST" preBtn:btn handle:^(id  _Nonnull x) {
        neb::CJsonObject jsonObj;
        jsonObj.Add("email", "lizhijian_21@163.com");
        jsonObj.Add("pass", "123456");
        
        self.httpClient->Request(CNetRequest::METHOD_TYPE_POST, "http://api.securesmart.cn/cameraServer2/v2/userLogin.api", jsonObj, [](void *userData, const std::string strContent) {
            NSLog(@"Post Success：%s", strContent.c_str());
        }, [](void *userData, long statusCode, const std::string strError) {
            NSLog(@"Post Fail[%ld]：%s", statusCode, strError.c_str());
        });
    }];
    [self.view addSubview:btn];
    
    
    btn = [self btnWithTitle:@"DOWNLOAD" preBtn:btn handle:^(id  _Nonnull x) {
            NSString *path = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, true).firstObject;
        self.httpClient->Download("http://trace.eas.asu.edu/yuv/bridge-close/bridge-close_cif.7z", path.UTF8String, "", [](void *userData, const std::string strContent) {
            NSLog(@"Download Success：%s", strContent.c_str());
        }, [](void *userData, long statusCode, const std::string strError) {
            NSLog(@"Download Fail[%ld]：%s", statusCode, strError.c_str());
        }, [](void *userData, unsigned long long totalSize, unsigned long long currentSize) {
            NSLog(@"Download Progress %lld:%lld", totalSize, currentSize);
        });
    }];
    [self.view addSubview:btn];
    
    btn = [self btnWithTitle:@"UPLOAD" preBtn:btn handle:^(id  _Nonnull x) {
        
    }];
    [self.view addSubview:btn];
}


@end
