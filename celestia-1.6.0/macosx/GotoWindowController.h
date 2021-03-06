//
//  GotoWindowController.h
//  celestia
//
//  Created by Bob Ippolito on Tue May 28 2002.
//  Copyright (c) 2002 Chris Laurel. All rights reserved.
//

@interface GotoWindowController : NSWindowController
{
    IBOutlet NSTextField *distanceField;
    IBOutlet NSTextField *latitudeField;
    IBOutlet NSTextField *longitudeField;
    IBOutlet NSTextField *objectField;
    IBOutlet NSPopUpButton *unitsButton;
	
	IBOutlet NSTextField *distanceFieldLabel;
    IBOutlet NSTextField *latitudeFieldLabel;
    IBOutlet NSTextField *longitudeFieldLabel;
    IBOutlet NSTextField *objectFieldLabel;
}
- (IBAction)gotoObject:(id)sender;
- (IBAction)showWindow:(id)sender;
@end
