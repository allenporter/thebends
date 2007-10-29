// StatusBar.h
#import <UIKit/UIKit.h>

@interface StatusBar : UIApplication {
  UITextView* text;
  UIView* main;
  UIWindow* window;
}

- (void)applicationDidFinishLaunching: (id)unused;

@end
