/* LocalService */

#import <Cocoa/Cocoa.h>

@interface LocalService : NSObject
{
	NSMutableDictionary * properties;
}
- (NSMutableDictionary *) properties;
- (void) setProperties: (NSDictionary *)newProperties;
@end
