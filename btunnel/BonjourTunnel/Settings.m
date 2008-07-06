#import "Settings.h"

@implementation Settings

- (NSString *)trackerUrl
{
	return trackerUrl;
}

- (void)setTrackerUrl:(NSString *)newTrackerUrl
{
	if (newTrackerUrl != trackerUrl)
	{
		[trackerUrl release];
		trackerUrl = [newTrackerUrl copy];
	}
}

- (BOOL)validateTrackerUrl:(id *)ioValue error:(NSError **)outError {
    if (*ioValue == nil || ![*ioValue isKindOfClass:[NSString class]]) {
        return NO;
    }
	NSURL* url = [[NSURL URLWithString:*ioValue] standardizedURL];
	if (url != nil && [url scheme] != nil && [[url scheme] compare:@"http"] == NSOrderedSame) {
		*ioValue = [url absoluteString];
		return YES;
	}
	NSString *errorString =
		NSLocalizedStringFromTable(
			@"Invalid URL Format",
			@"Tracker Url",
			@"validation: invalid url error");
	NSDictionary *userInfoDict =
        [NSDictionary dictionaryWithObject:errorString
                forKey:NSLocalizedDescriptionKey];
	NSError *error =
		[[[NSError alloc] initWithDomain:@"org.thebends.btunnel"
			code:0
            userInfo:userInfoDict] autorelease];
	*outError = error;
	return NO;
}

- (NSString *)roomName
{
	return roomName;
}

- (void)setRoomName:(NSString *)newRoomName
{
	if (newRoomName != roomName)
	{
		[roomName release];
		roomName = [newRoomName copy];
	}
}

- (BOOL)validateRoomName:(id *)ioValue error:(NSError **)outError {
    if (*ioValue == nil || ![*ioValue isKindOfClass:[NSString class]]) {
        return NO;
    }
	NSString* string = *ioValue;
	if ([string length] > 0) {
		return YES;
	}
	NSString *errorString =
		NSLocalizedStringFromTable(
			@"Room name was empty",
			@"Room Name",
			@"validation: invalid room name error");
	NSDictionary *userInfoDict =
        [NSDictionary dictionaryWithObject:errorString
                forKey:NSLocalizedDescriptionKey];
	NSError *error =
		[[[NSError alloc] initWithDomain:@"org.thebends.btunnel"
			code:0
            userInfo:userInfoDict] autorelease];
	*outError = error;
	return NO;
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
    [encoder encodeObject:trackerUrl forKey:@"trackerUrl"];
    [encoder encodeObject:roomName forKey:@"roomName"];
}

- initWithCoder:(NSCoder *)decoder
{
    [self setTrackerUrl:[decoder decodeObjectForKey:@"trackerUrl"]];
    [self setRoomName:[decoder decodeObjectForKey:@"roomName"]];
    return self;
}

@end
