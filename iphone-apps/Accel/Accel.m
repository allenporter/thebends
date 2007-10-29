// Author: Allen Porter <allen@thebends.org>
// Demo that gets readings from the accelerometer.

#import "Accel.h"
#import <Foundation/Foundation.h>

@implementation Accel

- (void)acceleratedInX:(float)x Y:(float)y Z:(float)z
{
  [text setText:[NSString stringWithFormat:@"accel! (%0.2f, %0.2f, %0.2f)", x, y, z]];
}

- (void)applicationDidFinishLaunching:(id)unused
{
  NSLog(@"applicationDidFinishLaunching");

  CGRect frame = [UIHardware fullScreenApplicationContentRect];
  frame.origin.x = frame.origin.y = 0;

  text = [[UITextView alloc] initWithFrame:frame];

  main = [[UIView alloc] initWithFrame:frame];
  [main addSubview:text];

  window = [[UIWindow alloc] initWithContentRect:frame];
  [window orderFront: self];
  [window makeKey: self];
//  [window _setHidden: NO];
  [window setContentView: main]; 
}

@end
