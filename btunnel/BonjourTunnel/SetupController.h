/* SetupController */

#import <Cocoa/Cocoa.h>

@interface SetupController : NSObject
{
    IBOutlet NSButton *continueButton;
    IBOutlet NSButton *goBackButton;
    IBOutlet NSTextField *roomNameText;
//    IBOutlet Settings *settings;
    IBOutlet NSTabView *tabView;
    IBOutlet NSTextField *trackerUrlText;
}
- (void)textDidChange:(NSNotification *)aNotification;
- (BOOL)textShouldEndEditing:(NSText *)textObject;
- (IBAction)cancelPressed:(id)sender;
- (IBAction)continuePressed:(id)sender;
- (IBAction)goBackPressed:(id)sender;
- (IBAction)textAction:(id)sender;
@end
