#import "SetupController.h"

@implementation SetupController

- (void)textDidChange:(NSNotification *)aNotification
{
	NSLog(@"foo");
}

- (BOOL)textShouldEndEditing:(NSText *)textObject
{
  NSLog(@"Should end?");
  return YES;
}

- (void)validate
{
	NSLog(@"validate");
	NSTabViewItem* selectedItem = [tabView selectedTabViewItem];
	int index = [tabView indexOfTabViewItem:selectedItem];
	
 	BOOL continueAllowed = YES;
	if (index == 1)
	{  // Tracker URL pane
		continueAllowed = NO;
	/*
		NSURL* url = [[NSURL URLWithString:[trackerUrlText stringValue]] standardizedURL];
		continueAllowed = NO;
		if (url != nil && [url scheme] != nil && [[url scheme] compare:@"http"] == NSOrderedSame)
		{
			continueAllowed = YES;
		}
		*/
	}
	else if (index == 2)
	{
		continueAllowed = NO;
	}
	
	[goBackButton setEnabled:(index != 0)];
	[continueButton setEnabled:continueAllowed];
	if (index != ([tabView numberOfTabViewItems] - 1))
	{
		[continueButton setTitle:@"Continue"];
	}
	else
	{
		[continueButton setTitle:@"Finish"];
	}
}

- (IBAction)cancelPressed:(id)sender
{
	[NSApp stopModal];
	[NSApp terminate:self];
}

- (IBAction)continuePressed:(id)sender
{
	[tabView selectNextTabViewItem:sender];
	[self validate];
}

- (IBAction)goBackPressed:(id)sender
{
	[tabView selectPreviousTabViewItem:sender];
	[self validate];
}

- (void)awakeFromNib
{
	[tabView selectTabViewItemAtIndex:0];
	[self validate];
}

- (IBAction)textAction:(id)sender
{
	NSLog(@"action");
}

@end
