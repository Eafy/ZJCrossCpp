//
//  BaseDemoViewController.h
//  ZJCrossCppDemo
//
//  Created by eafy on 2021/3/15.
//  Copyright © 2021 ZJ. All rights reserved.
//

#import "ZJBaseViewController.h"

NS_ASSUME_NONNULL_BEGIN

@interface BaseDemoViewController : ZJBaseViewController

- (ZJButton *)btnWithTitle:(NSString *)title preBtn:(nullable ZJButton *)btn toClass:(nullable Class)cls;

- (ZJButton *)btnWithTitle:(NSString *)title preBtn:(nullable ZJButton *)btn handle:(void (^)(id x))nextBlock;

@end

NS_ASSUME_NONNULL_END
