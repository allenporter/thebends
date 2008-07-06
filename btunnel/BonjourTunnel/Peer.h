// Peer.h
// Author: Allen Porter <allen@thebends.org>

#import <Cocoa/Cocoa.h>


@interface Peer : NSObject {
	NSMutableDictionary * properties;
}
- (NSMutableDictionary *) properties;
- (void) setProperties: (NSDictionary *)newProperties;
@end
