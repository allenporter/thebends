#import "FileTable.h"
#import <CoreFoundation/CoreFoundation.h>
#import <UIKit/UITableColumn.h>
#import <UIKit/UIImageAndTextTableCell.h>


@implementation FileTable

- (void) listFiles
{
	NSLog(@"listFiles");

	// release previous items
	int i;
	for (i = 0; i < [list count]; i++) {
	  [[list objectAtIndex:i] release];
	}
	[list removeAllObjects];
	[table reloadData];

	NSString *file;
	NSArray* files = [[NSFileManager defaultManager] directoryContentsAtPath:path];
    NSEnumerator *dirEnum = [files objectEnumerator];
	while ((file = [dirEnum nextObject])) {
		UIImageAndTextTableCell* pbCell = [[UIImageAndTextTableCell alloc] init];
		[pbCell setTitle: file]; 
		[list addObject: pbCell];
	}
    [table reloadData];
	NSLog(@"listFiles done");
}

- (id) initWithTable: (UITable*)uitable
{
// TODO: dealloc
    list = [[NSMutableArray alloc] initWithCapacity:30];
	table = uitable;
	path = @"/";

    UITableColumn *col = [[UITableColumn alloc] initWithTitle: @"HelloApp"
        identifier: @"hello" width: 320.0f];
    [table addTableColumn: col];
	
    [table setDataSource: self];
    [table setDelegate: self];

	[self listFiles];

	return self;
}

- (int) numberOfRowsInTable: (UITable *)table
{
    NSLog(@"count:%d", [list count]);
    return [list count];
}

- (UITableCell *) table: (UITable *)table cellForRow: (int)row column: (int)col
{
    NSLog(@"cellForRow %d (%d)", row, [list count]);
    return [list objectAtIndex:row];
}

- (void)tableSelectionDidChange:(id)fp8;        // IMP=0x3241e474
{
    UIImageAndTextTableCell* cell = [list objectAtIndex:[table selectedRow]];
    [cell setSelected:NO];
  
    NSLog(@"tableSelectionDidChange: %@", [cell title]);
    NSLog(@"current path: %@", path);
    path = [[NSString stringWithString:[path stringByAppendingPathComponent:[cell title]]] retain];
    [self listFiles];
    NSLog(@"done: %@", path);
}

@end