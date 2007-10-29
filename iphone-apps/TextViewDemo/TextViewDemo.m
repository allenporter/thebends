#import "TextViewDemo.h"
#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <UIKit/CDStructures.h>
#import <UIKit/UIView.h>
#import <UIKit/UIView-Hierarchy.h>
#import <UIKit/UINavigationBar.h>
#import <UIKit/UIWindow.h>
#import <UIKit/UIHardware.h>
#import <UIKit/UITextView.h>
#import <UIKit/UIKeyboard.h>

@implementation TextViewDemo

- (void) applicationDidFinishLaunching: (id) unused
{
    UIWindow *window = [[UIWindow alloc] initWithContentRect: [UIHardware 
        fullScreenApplicationContentRect]];
    [window orderFront: self];
    [window makeKey: self];
    [window _setHidden: NO];
 
    UITextView* view = [[UITextView alloc]
        initWithFrame: CGRectMake(0.0f, 0.0f, 320.0f, 240.0f)];
    [view setEditable:NO];  // don't mess up my pretty output

    FILE* fp = popen("/bin/ls /Applications", "r");
    if (fp == NULL) {
      NSLog(@"popen failed");
      exit(1);
    }

    static const int kBlockSize = 32;  // arbitarry
    int c;
    do {
      char buf[32];
      c = fread(buf, sizeof(char), kBlockSize, fp);
      if (c <= 0) {
        break;
      }
      printf("Read %d bytes\n", c);
      buf[c] = '\0';
      NSString* existing = [view text];
      NSString* out = [NSString stringWithCString:buf
          encoding:[NSString defaultCStringEncoding]];
      [view setText:[existing stringByAppendingString: out]];
   } while (1);

    UIKeyboard* keyboard = [[UIKeyboard alloc]
        initWithFrame: CGRectMake(0.0f, 240.0, 320.0f, 480.0f)];

    struct CGRect rect = [UIHardware fullScreenApplicationContentRect];
    rect.origin.x = rect.origin.y = 0.0f;
    UIView *mainView;
    mainView = [[UIView alloc] initWithFrame: rect];
    [mainView addSubview: view]; 
    [mainView addSubview: keyboard];

    [window setContentView: mainView]; 
}

@end
