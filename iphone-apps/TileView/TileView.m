// TileView.m
// Author: Allen Porter <allen@thebends.org>
//
// Fun with a UITileView and a UIScroller.  Lets get some fast drawing going!

#import "TileView.h"
#import <Foundation/Foundation.h>
#import <CoreSurface/CoreSurface.h>
#import <UIKit/UITextView.h>
#import <UIKit/UITile.h>
#import <UIKit/UITiledView.h>
#import <UIKit/UIView-Internal.h>
#import <UIKit/UIView-Hierarchy.h>
#include <math.h>

@interface RandomColorTile : UITile
{
}
@end

@implementation RandomColorTile

- (void)logRect:(struct CGRect)rect;
{
  NSLog(@"(%f,%f) -> (%f,%f)", rect.origin.x, rect.origin.y,
        rect.size.width, rect.size.height);
}

- (id)initWithFrame:(struct CGRect)frame
{
  self = [super initWithFrame:frame];
  NSLog(@"init with frame");
  [self logRect:frame];
  return self;
}

- (void)drawRect:(struct CGRect)rect
{
  CGContextRef context = UICurrentContext();
  float r = (float)(rand() % 100) / 100.0;
  float g = (float)(rand() % 100) / 100.0;
  float b = (float)(rand() % 100) / 100.0;
  CGContextSetRGBFillColor(context, r, g, b, 1.0);
  CGContextFillRect(context, rect);
}

@end

@interface SomeTiledView : UITiledView
{
}
@end

@implementation SomeTiledView

- (id)initWithFrame:(struct CGRect)rect
{
  // width and height of each tile
  float w = ceil(rect.size.width / 1);
  float h = 10; //ceil(rect.size.height / 40);

  self = [super initWithFrame:rect];
  [self setFirstTileSize:CGSizeMake(w, h)];
  [self setOpaque:YES];
  [self setNeedsDisplay];
  [self setNeedsLayout];
// Interesting when not drawing
//  [self setDrawsGrid:YES];
  [self setTileSize:CGSizeMake(w, h)];
  [self setTileDrawingEnabled:YES];
  [self setTilingEnabled:YES];
  return self;
}

- (void)logRect:(struct CGRect)rect;
{
  NSLog(@"(%f,%f) -> (%f,%f)", rect.origin.x, rect.origin.y,
        rect.size.width, rect.size.height);
}

+ (Class)tileClass
{
  return [RandomColorTile class];
}

@end

@implementation TileView

- (void) applicationDidFinishLaunching: (id) unused
{
  NSLog(@"applicationDidFinishLaunching");

  CGRect rect = [UIHardware fullScreenApplicationContentRect];
  rect.origin.x = rect.origin.y = 0;
  UIWindow *window = [[UIWindow alloc] initWithContentRect:rect];
  [window orderFront: self];
  [window makeKey: self];
  [window _setHidden: NO];

  UIScroller* scroller = [[UIScroller alloc] initWithFrame:rect];

  // Make some scrollz!!!1!!!!!11
  rect.size.height += 1000; 

  SomeTiledView* view = [[SomeTiledView alloc] initWithFrame:rect];

  [scroller setContentSize:rect.size];
  [scroller addSubview:view];
  [scroller setAllowsRubberBanding:YES];
  [scroller displayScrollerIndicators];

  [window setContentView:scroller];
  [window setNeedsDisplay];
}

@end
