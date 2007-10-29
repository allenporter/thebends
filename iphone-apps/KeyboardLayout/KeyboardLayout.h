#import <UIKit/UIKit.h>

@interface KeyboardLayout : UIApplication {
  UIWindow* window;
  UIView* main;
  UITextView* text;
  UIKeyboardImpl* keyboard;

  int lastOrientation;
  float lastAngle;
}

- (void)applicationDidFinishLaunching: (id)unused;

@end
