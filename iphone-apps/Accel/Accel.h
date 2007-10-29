#import <UIKit/UIKit.h>

@interface Accel : UIApplication {
  UIWindow* window;
  UIView* main;
  UITextView* text;

}

- (void)applicationDidFinishLaunching: (id)unused;
- (void)acceleratedInX:(float)x Y:(float)y Z:(float)z;

@end
