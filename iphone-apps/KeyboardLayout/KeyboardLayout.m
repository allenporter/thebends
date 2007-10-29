// KeyboardLayout.m
// Author: Allen Porter <allen@thebends.org>
//
// Broken keyboard layout for rotation.

#import "KeyboardLayout.h"
#import <Foundation/Foundation.h>
#import <GraphicsServices/GraphicsServices.h>
#import <UIKit/UIView-Deprecated.h>
#import <UIKit/UIKeyboardLayoutQWERTY.h>
#import <UIKit/UIKeyboardLayoutQWERTYLandscape.h>

@implementation KeyboardLayout

- (void)logRect:(struct CGRect)rect
{
  [text setText:[NSString stringWithFormat:@"(%f,%f) -> (%f,%f)",
                   rect.origin.x, rect.origin.y,
                   rect.size.width, rect.size.height]];
}

- (void)_updateRotation:(int)orientation
{
  NSLog(@"layout: %d", orientation);
  // Only support these two modes to make the demo simple
  if (lastOrientation == orientation ||
      (orientation != kOrientationHorizontalLeft &&
       orientation != kOrientationVertical)) {
    return;
  }

  [self setUIOrientation:orientation];
  CGRect frame = [UIHardware fullScreenApplicationContentRect];
  frame.origin.x = frame.origin.y = 0;

  if (orientation == kOrientationHorizontalLeft) {
    int x = frame.size.width;
    frame.size.width  = frame.size.height;
    frame.size.height = x;
    [main setOrigin:CGPointMake(70, 70)];
    [main setRotationDegrees:90 duration:0.1];

//    int keyboardStart = frame.size.height - [keyboard frame].size.height;
    [keyboard setSize:CGSizeMake(460, 200)];
//    [keyboard setOrigin:CGPointMake(-75, 210)]; //0, 0)]; //keyboardStart)];
    [keyboard showLayout:[UIKeyboardLayoutQWERTYLandscape class]];
  } else {
    [main setOrigin:CGPointMake(0,0)];
    [main setRotationDegrees:0 duration:0.1];

    [keyboard setSize:CGSizeMake(320, 216)];
    
    int keyboardStart = frame.size.height - [keyboard frame].size.height;
    [keyboard setOrigin:CGPointMake(0, keyboardStart)];
    [keyboard showLayout:[UIKeyboardLayoutQWERTY class]];
  }
  [main setBounds:frame];
  [text setBounds:frame];
  [text setText:[NSString stringWithFormat:@"Ch: %d\n", orientation]];

  [main setNeedsDisplay];
  [text setNeedsDisplay];
  [keyboard setNeedsDisplay];

  lastOrientation = orientation;
}

- (void)deviceOrientationChanged:(GSEvent *)event;
{
  NSLog(@"deviceOrientationChanged");
  [self _updateRotation:GSEventDeviceOrientation(event)];
}

- (void)updateRotation
{
  [self _updateRotation:[UIHardware deviceOrientation:NO]];
}

- (void)applicationDidFinishLaunching:(id)unused
{
  NSLog(@"applicationDidFinishLaunching");
  lastOrientation = -1;

  CGRect frame = [UIHardware fullScreenApplicationContentRect];
  frame.origin.x = frame.origin.y = 0;

  text = [[UITextView alloc] initWithFrame:frame];
  keyboard = [UIKeyboardImpl sharedInstance];

  main = [[UIView alloc] initWithFrame:frame];
  [main addSubview:text];
  [main addSubview:keyboard];

  window = [[UIWindow alloc] initWithContentRect:frame];
  [window orderFront: self];
  [window makeKey: self];
  [window _setHidden: NO];
  [window setContentView: main]; 

  [self updateRotation];
}

@end
