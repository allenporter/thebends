#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <UIKit/CDStructures.h>
#import <UIKit/UIPushButton.h>
#import <UIKit/UIThreePartButton.h>
#import <UIKit/UINavigationBar.h>
#import <UIKit/UIWindow.h>
#import <UIKit/UIView-Hierarchy.h>
#import <UIKit/UIHardware.h>
#import <UIKit/UITable.h>
#import "HelloApplication.h"

@implementation HelloApplication

- (void) applicationDidFinishLaunching: (id) unused
{
	UIWindow *window;

    window = [[UIWindow alloc] initWithContentRect: [UIHardware
        fullScreenApplicationContentRect]];

    UITable *table = [[UITable alloc] initWithFrame: CGRectMake(0.0f, 48.0f,
        320.0f, 480.0f - 16.0f - 32.0f)];

    [window orderFront: self];
    [window makeKey: self];
    [window _setHidden: NO];
 
    tableData = [[FileTable alloc] initWithTable:table];
    [table reloadData];

    UINavigationBar *nav = [[UINavigationBar alloc] initWithFrame: CGRectMake(
        0.0f, 0.0f, 320.0f, 48.0f)];
    [nav showButtonsWithLeftTitle: @"Back" rightTitle: @"Bar" leftBack: NO];
//    [nav showBackButton:YES animated:NO];
    [nav setBarStyle: 0];

    struct CGRect rect = [UIHardware fullScreenApplicationContentRect];
    rect.origin.x = rect.origin.y = 0.0f;
    UIView *mainView;
    mainView = [[UIView alloc] initWithFrame: rect];
    [mainView addSubview: nav]; 
    [mainView addSubview: table]; 

    [window setContentView: mainView]; 
}

@end
