// LCDView.h
// Author: Allen Porter <allen@thebends.org>
#import <UIKit/UIKit.h>

@interface LCDView : UIApplication {
  UIWindow* window;
  UIView* view;
}

- (void)applicationDidFinishLaunching: (id)unused;

@end
