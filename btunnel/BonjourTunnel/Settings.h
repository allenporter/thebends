/* Settings */

#import <Cocoa/Cocoa.h>

@interface Settings : NSObject
{
	NSString *trackerUrl;
	NSString *roomName;
}
- (NSString*)trackerUrl;
- (void)setTrackerUrl:(NSString*)newTrackerUrl;
- (NSString*)roomName;
- (void)setRoomName:(NSString*)newRoomName;
@end