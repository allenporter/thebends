// TextDrawing.m
// Author: Allen Porter <allen@thebends.org>
//
// Uses a background thread to 

#import "TextDrawing.h"
#import <Foundation/Foundation.h>
#import <unistd.h>

@interface MyView : UIView { }
@end
@implementation MyView

int height = 40;
int width = 47;

NSLock* screenLock;

- (void)threadCallback:(id)ignored
{
  while (1) {
    [self performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:YES];
  }
}

- (int)setRandomColor:(CGContextRef)context
{
  float r = (float)(rand() % 10) / 10.0;
  float g = (float)(rand() % 10) / 10.0;
  float b = (float)(rand() % 10) / 10.0;
  CGContextSetRGBFillColor(context, r, g, b, 1);
}

- (void)drawRect:(struct CGRect)rect
{
  [screenLock lock];

  CGContextRef context = UICurrentContext();
  float w, h;
  w = rect.size.width;
  h = rect.size.height;
  float lineHeight = h / height;
  float charWidth = w / width;
  CGContextSelectFont(context, "CourierNewBold", lineHeight,
                      kCGEncodingMacRoman);
  CGContextSetTextDrawingMode(context, kCGTextFill);
  CGAffineTransform myTextTransform;
  // Flip text, for some reason its written upside down by default
  myTextTransform = CGAffineTransformMake(1, 0, 0, -1, 0, h/30);
  CGContextSetTextMatrix(context, myTextTransform);

  int i, j;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      char c;
      switch (rand() % 3) {
        case 0:
          c = '0' + rand() % 10;
          break;
        case 1:
          c = 'a' + rand() % 26;
          break;
        default:
          c = 'A' + rand() % 26;
      }
      [self setRandomColor:context];
      CGContextShowTextAtPoint(context, j * charWidth, (i + 1) * lineHeight,
                               &c, 1);
    }
  }

  [screenLock unlock];
  return [super drawRect:rect];
}
@end

@implementation TextDrawing

- (void) applicationDidFinishLaunching: (id) unused
{
  srand(time(NULL));
  screenLock = [[NSLock alloc] init];

  NSLog(@"applicationDidFinishLaunching");

  UIWindow *window =
    [[UIWindow alloc]
      initWithContentRect:[UIHardware fullScreenApplicationContentRect]];
  [window orderFront: self];
  [window makeKey: self];
  [window _setHidden: NO];

  MyView* view = [[MyView alloc] initWithFrame:CGRectMake(0.0f, 0.0f,
                                                          320.0f, 460.0f)];
  [view becomeFirstResponder];
  [view setNeedsDisplay];

  struct CGRect rect = [UIHardware fullScreenApplicationContentRect];
  rect.origin.x = rect.origin.y = 0.0f;

  UIView *mainView = [[UIView alloc] initWithFrame: rect];
  [mainView addSubview:view];

  [window setContentView: mainView]; 

  [NSThread detachNewThreadSelector:@selector(threadCallback:)
                           toTarget:view
                         withObject:nil];
}

@end
