#import "LocalService.h"

@implementation LocalService


- (id) init
{
	if (self = [super init])
	{
		NSArray * keys = [NSArray arrayWithObjects: @"address", @"subject", @"date", @"body", nil];
		NSArray * values = [NSArray arrayWithObjects: @"test@test.com", @"Subject", [NSDate date], [NSString string], nil];
		properties = [[NSMutableDictionary alloc] initWithObjects: values forKeys: keys];
	}
	return self;
}


@end
