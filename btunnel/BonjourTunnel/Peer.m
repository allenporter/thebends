//
//  Peer.m
//  BonjourTunnel
//
//  Created by aporter on 4/5/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "Peer.h"


@implementation Peer

- (id) init
{
	if (self = [super init])
	{
		NSArray * keys      = [NSArray arrayWithObjects: @"title", @"hostaname", @"date", @"body", nil];
        NSArray * values    = [NSArray arrayWithObjects: @"test@test.com", @"Subject", [NSDate date], [NSString string], nil];
		properties = [[NSMutableDictionary alloc]  initWithObjects: values forKeys: keys];
	}
	return self;
}

- (void) dealloc
{
    [properties release];
    
    [super dealloc];
}

- (NSMutableDictionary *) properties
{
    return properties;
}

- (void) setProperties: (NSDictionary *)newProperties
{
    if (properties != newProperties)
    {
        [properties autorelease];
        properties = [[NSMutableDictionary alloc] initWithDictionary: newProperties];
    }
}

@end
