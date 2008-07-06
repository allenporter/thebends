/* MainController */

#import <Cocoa/Cocoa.h>

namespace ypeer { class Peers; }

@interface MainController : NSObject
{
    IBOutlet NSProgressIndicator *progress;
    IBOutlet NSTextField *roomName;
    IBOutlet NSTextField *status;
	IBOutlet NSButton *connect;
    IBOutlet NSTableView *tableView;
    IBOutlet NSTextField *trackerUrl;
    IBOutlet NSWindow *window;

	NSMutableArray *peers;

	NSConditionLock* condLock;
}
- (id)init;
- (void)alertWithMessageText:(NSString*)title informativeText:(NSString*)info;
- (void)setStatusText:(NSString*)statusText;
- (void)setStatusTextFromThread:(NSString*)statusText progressEnabled:(BOOL)enabled;
- (IBAction)connectPressed:(id)sender;
@end
