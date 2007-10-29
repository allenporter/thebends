#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UIKit/UIApplication.h>
#import <UIKit/UITable.h>

@interface FileTable : UIApplication {
  NSMutableArray* list;
  UITable* table;
  NSString* path;
}

- (id) initWithTable: (UITable*)table;

@end
