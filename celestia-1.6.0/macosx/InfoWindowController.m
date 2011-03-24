//
//  InfoWindowController.m
//  celestia
//
//  Created by Jonathan Coles on 3/21/11.
//  Copyright 2011 ZM. All rights reserved.
//

#import "InfoWindowController.h"
#import "CelestiaAppCore.h"
#import "CelestiaSimulation.h"
#import "CelestiaBody.h"
#import "Astro.h"
#import "CelestiaVector.h"
#include <math.h>

#define UNITS_KM 0
#define UNITS_RADII 1
#define UNITS_AU 2

@implementation InfoWindowController

- (IBAction)showWindow:(id)sender
{
    CelestiaSimulation *sim;
    CelestiaSelection *csel;
    CelestiaBody *body;
    sim = [[CelestiaAppCore sharedAppCore] simulation];
    csel = [sim selection];
    body = [csel body];
    if (body != nil) {}
	if ([sim getLanguage])
	{ /* English */
		NSLog(@"-[GotoWindowController english:%@]",sender);

		if ([sim getMode])
		{ /* Expert */
			[deutschExpert setHidden:NO];
			[englishExpert setHidden:YES];
			[deutschNovice setHidden:YES];
			[englishNovice setHidden:YES];
		}
		else
		{ /* Novice */
			[deutschExpert setHidden:YES];
			[englishExpert setHidden:YES];
			[deutschNovice setHidden:YES];
			[englishNovice setHidden:NO];
		}
		
	}
	else
	{ /* Deutsch */
		NSLog(@"-[GotoWindowController deutsch:%@]",sender);

		if ([sim getMode])
		{ /* Expert */
			[deutschExpert setHidden:YES];
			[englishExpert setHidden:NO];
			[deutschNovice setHidden:YES];
			[englishNovice setHidden:YES];
		}
		else
		{ /* Novice */
			[deutschExpert setHidden:YES];
			[englishExpert setHidden:YES];
			[deutschNovice setHidden:NO];
			[englishNovice setHidden:YES];
		}
		
	}
	[[self window] setLevel:NSFloatingWindowLevel];
	[[self window] setMovable:NO];
    [super showWindow:sender];
	
}

@end
