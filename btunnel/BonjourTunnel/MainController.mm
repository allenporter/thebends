#include <string>
#import "MainController.h"
#import "Peer.h"
#include "ypeer/peers.h"
#include "ypeer/sha1.h"
#include "natpmp/util.h"

using namespace std;

static const int kLocalPort = 9969;

// TODO(allen): We currently don't ever renew our port mapping.
static const int kPortMapLifetime = 30 * 86400;


class FakePeers : public ypeer::Peers {
 public:
	FakePeers() { }
	
	bool Error() { return false; }
	string ErrorString() { return ""; }
	bool Connected() { return true; }
    bool GetPeers(vector<struct ypeer::Peer>* peers)
	{
		struct ypeer::Peer peer;
		peer.peer_id = "A";
		peer.ip = "1.2.3.4";
		peer.port = 5050;
		peers->push_back(peer);
		peer.ip = "1.2.3.5";
		peer.port = 5050;
		peers->push_back(peer);
		peer.ip = "1.2.3.6";
		peer.port = 5050;
		peers->push_back(peer);
		return true;
	}
	void SetCallback(ythread::Callback* callback) {	}
};


static void ConvertHash(uint32_t *hash, string* result) {
  for (int i = 0; i < 5; i++) {
    uint32_t key = hash[i];
    for (int j = 3; j >= 0; j--) {
      unsigned char c = (unsigned char)((key >> (j * 8)) & 0xff);
      result->append(1, c);
    }
  }
}

static string InfoHash(NSString* roomName) {
  const string& room = [roomName cString];
  uint32_t hash[5];
  SHA1 sha1;
  sha1.Input(room.c_str(), room.size());
  if (!sha1.Result(hash)) {
    NSLog(@"Unable to compute hash");
    exit(1);
  }
  string info_hash;
  ConvertHash(hash, &info_hash);
  return info_hash;
}

@implementation MainController

enum Conditions {
	NO_DATA = 0,
	HAS_DATA
};

- (id)init
{
	self = [super init];
	if (self != nil) {
		Peer* peer = [[Peer alloc] init];
		NSMutableDictionary* properties = [peer properties];
		[properties setObject:@"hostport" forKey:@"a:b"];
		
		peers = [NSMutableArray arrayWithObjects:peer, nil];
		condLock = [[NSConditionLock alloc] initWithCondition:NO_DATA];
		[NSThread detachNewThreadSelector:@selector(backgroundThread:)
                             toTarget:self
							 withObject:self];	
	}
	return self;
}

- (void)alertWithMessageText:(NSString*)title informativeText:(NSString*)info
{
		NSAlert* alert =
			[NSAlert alertWithMessageText:title
				defaultButton:@"OK"
				alternateButton:nil
				otherButton:nil
				informativeTextWithFormat:info];
		[alert runModal];
}

- (void)setConnectEnabled:(BOOL)enabled
{
	[trackerUrl setEnabled:enabled];
	[roomName setEnabled:enabled];
	[connect setEnabled:enabled];
}

- (IBAction)connectPressed:(id)sender
{
	NSLog(@"connectPressed");
	NSURL* url = [[NSURL URLWithString:[trackerUrl stringValue]] standardizedURL];
	if (url == nil || [url scheme] == nil || [[url scheme] compare:@"http"] != NSOrderedSame) {
		[self alertWithMessageText:@"Invalid Tracker URL"
			informativeText:@"Tracker URL was not formatted correctly"];
		return;
	}
	
	if ([[roomName stringValue] length] == 0) {
		[self alertWithMessageText:@"Invalid Room Name"
			informativeText:@"Room name must not be empty"];
		return;
	}
	
	[condLock lock];
	// Wake background thread
	[condLock unlockWithCondition:HAS_DATA];
}

- (void)setProgressEnabled:(id)enabled
{
	if (enabled) {
		[progress startAnimation:self];
	} else {
		[progress stopAnimation:self];
	}	
}

- (void)setStatusText:(NSString*)statusText
{
	[status setStringValue:statusText];
}

- (void)setStatusTextFromThread:(NSString*)statusText progressEnabled:(BOOL)enabled
{	
	NSLog(@"setStatusTextFromThread");
	[self performSelectorOnMainThread:@selector(setStatusText:) withObject:statusText waitUntilDone:YES];
	[self performSelectorOnMainThread:@selector(setProgressEnabled:) withObject:(enabled ? self : nil) waitUntilDone:YES];
	NSLog(@"setStatusTextFromThread done");
}

- (void)addPeer:(NSString*)hostPort
{
	BOOL found = NO;
	int count = [peers count];
	for (int i = 0; i < count; ++i) {
		Peer* peer = [peers objectAtIndex:i];
		NSDictionary* properties = [peer properties];
		NSString* peerHostPort = [properties objectForKey:@"hostport"];
		if ([peerHostPort isEqualToString:hostPort]) {
			found = YES;
		}
	}
	if (!found) {
		NSLog(@"Adding peer");
		Peer* peer = [[Peer alloc] init];
		NSMutableDictionary* properties = [peer properties];
		[properties setObject:@"hostport" forKey:hostPort];
		[peers addObject:peer];	
	}
}

- (void)reloadPeers
{
	[tableView reloadData];
	// TODO(aporter): Reload
}

- (void)backgroundThread:(id)object
{
	[[NSAutoreleasePool alloc] init];
	while (true)
	{
		NSLog(@"self: %@", self);
		[self setConnectEnabled:YES];
		[self setStatusTextFromThread:@"Offline" progressEnabled:NO];
	
	    [condLock lockWhenCondition:HAS_DATA];
		
		[self setConnectEnabled:NO];
		[progress startAnimation:self];		
		[self setStatusTextFromThread:@"Creating port map with NAT PMP..." progressEnabled:YES];
	
		uint16_t public_port = kLocalPort;
		uint32_t lifetime = kPortMapLifetime;
		if (!pmp::CreateMap(pmp::TCP,
							kLocalPort,
							&public_port,
							&lifetime)) {
			[self alertWithMessageText:@"NatPMP Failure"
				informativeText:@"Unable to create a port mapping with your router using NAT PMP.  Unsupported?"];
			[condLock unlockWithCondition:NO_DATA];
			continue;
		}
	
//		const char* url_string = [[[[NSURL URLWithString:[trackerUrl stringValue]] standardizedURL] absoluteString] cString];
//		ypeer::Peers* peers = ypeer::NewPeers(url_string, InfoHash([roomName stringValue]), public_port);
		ypeer::Peers* peer = new FakePeers();
		if (peer == NULL) {
			[self alertWithMessageText:@"Internal Error"
					  informativeText:@"Unable to create peer object"];
			[condLock unlockWithCondition:NO_DATA];
			continue;
		}
		// TODO(allen): Allow user to disconnect manually
		bool connected = false;
		while (true) {
			if (peer->Error()) {
				[self alertWithMessageText:@"Connection Error"
					  informativeText:[NSString stringWithCString:peer->ErrorString().c_str()]];
			} else if (peer->Connected()) {
				// Set the status message one time, once connected.
				if (!connected) {
					connected = true;
					[self setStatusTextFromThread:@"Connected... Waiting for peers" progressEnabled:YES];
				}
				
				vector<struct ypeer::Peer> list;
				if (!peer->GetPeers(&list)) {
					[self alertWithMessageText:@"Connection Error"
						informativeText:@"Failed to get peers"];
					break;
				}
				
				for (vector<struct ypeer::Peer>::const_iterator it = list.begin();
					 it != list.end(); ++it) {
					NSString *hostPort = [[NSString alloc] initWithFormat:@"%s:%d", it->ip.c_str(), it->port];
					[self performSelectorOnMainThread:@selector(addPeer:) withObject:hostPort waitUntilDone:NO];
				}
				[self performSelectorOnMainThread:@selector(reloadPeers) withObject:nil waitUntilDone:YES];
			}
			[condLock unlock];
			sleep(1);
			[condLock lock];
		}
		
		delete peers;
		[condLock unlockWithCondition:NO_DATA];
	}
}

@end
