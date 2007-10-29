// LCDView.m
// Author: Allen Porter <allen@thebends.org>
//
// Sample of TelephonyUI stuff.
#import "LCDView.h"
#import <TelephonyUI/TelephonyUI.h>

@implementation LCDView

- (void)applicationDidFinishLaunching:(id)unused
{
  NSLog(@"applicationDidFinishLaunching");
  CGRect rect = [UIHardware fullScreenApplicationContentRect];
  rect.origin.x = rect.origin.y = 0;

  UITextView* text = [[UITextView alloc] initWithFrame:rect];
  [text setText:@"Background\nText\nView\nhere"];

  TPLCDView* lcd = [[TPLCDView alloc] initWithFrame:rect];
  [lcd setText:@"TPLCDView in front"];

  view = [[UIView alloc] initWithFrame:rect];
  [view addSubview:text];
  [view addSubview:lcd];

  window = [[UIWindow alloc] initWithContentRect:rect];
  [window orderFront: self];
  [window makeKey: self];
  [window _setHidden: NO];
  [window setContentView:view];
  [window setNeedsDisplay];
}

@end
