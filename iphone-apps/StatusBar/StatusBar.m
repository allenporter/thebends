// StatusBar.h
// Author: Allen Porter <allen@thebends.org>

#import "StatusBar.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKeyboardLayoutQWERTYLandscape.h>

@implementation StatusBar

- (void)logRect:(struct CGRect)rect msg:(NSString*)msg
{
  NSLog(@"%@: (%f,%f) -> (%f,%f)", msg,
        rect.origin.x, rect.origin.y,
        rect.size.width, rect.size.height);
}

- (void)statusBarMouseDown:(struct __GSEvent*)event
{
 NSLog(@"Status Bar Mode: %@", [self statusBarMode]);
  [text setText:[NSString stringWithFormat:@"mode=%@", [self statusBarMode]]];
}

- (void)applicationDidFinishLaunching:(id)unused
{
  NSLog(@"applicationDidFinishLaunching");

  CGRect frame = [UIHardware fullScreenApplicationContentRect];
  frame.origin.x = frame.origin.y = 0;

  text = [[UITextView alloc] initWithFrame:frame];
  [text setText:@"some text"];

  main = [[UIView alloc] initWithFrame:frame];
  [main addSubview:text];

  window = [[UIWindow alloc] initWithContentRect:frame];
  [window orderFront: self];
  [window makeKey: self];
  [window setContentView: main]; 

 [self logRect:[self statusBarRect] msg:@"statusBarRect"];
 NSLog(@"Status Bar Mode: %@", [self statusBarMode]);
 [self setStatusBarCustomText:@"foo bar"];
 [self removeStatusBarCustomText];
 [self vibrateForDuration:2];
}

@end
