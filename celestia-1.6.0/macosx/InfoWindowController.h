/*
 *  InfoWindowController.h
 *  celestia
 *
 *  Created by Jonathan Coles on 3/21/11.
 *  Copyright 2011 ZM. All rights reserved.
 *
 */

@interface InfoWindowController : NSWindowController
{
    IBOutlet NSImageView *englishExpert;
    IBOutlet NSImageView *englishNovice;

    IBOutlet NSImageView *deutschExpert;
    IBOutlet NSImageView *deutschNovice;

}
- (IBAction)showWindow:(id)sender;
@end
