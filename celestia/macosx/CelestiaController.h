//
//  CelestiaController.h
//  celestia
//
//  Created by Bob Ippolito on Tue May 28 2002.
//  Copyright (C) 2007, Celestia Development Team
//

#import "CelestiaAppCore.h"
#import "CelestiaSettings.h"
#import "FavoritesDrawerController.h"
#import "RenderPanelController.h"
#import "BrowserWindowController.h"

#define CELESTIA_RESOURCES_FOLDER @"CelestiaResources"

@class CelestiaOpenGLView;
@class SplashWindow;
@class SplashWindowController;
@class EclipseFinderController;
@class ScriptsController;

@interface CelestiaController : NSWindowController 
{
    CelestiaSettings* settings;
    CelestiaAppCore* appCore;
    BOOL threaded;
    BOOL ready;
    BOOL isDirty;
    BOOL isFullScreen;
    IBOutlet SplashWindowController *splashWindowController;
    IBOutlet NSTextView *glInfo;
    IBOutlet NSPanel *glInfoPanel;
    IBOutlet CelestiaOpenGLView *glView;
	IBOutlet NSView *view;
    NSWindow *origWindow;
    IBOutlet FavoritesDrawerController *favoritesDrawerController;
    IBOutlet RenderPanelController *renderPanelController;
    IBOutlet ScriptsController *scriptsController;
    BrowserWindowController *browserWindowController;
    EclipseFinderController *eclipseFinderController;
    NSWindowController *helpWindowController;
    NSTimer* timer;

    NSConditionLock* startupCondition;
    int keyCode, keyTime;
    NSString* lastScript;
    NSString *pendingScript;
    NSString *pendingUrl;
	
	IBOutlet NSButton *btnReset;
	IBOutlet NSButton *btnPlay;
	IBOutlet NSButton *btnStop;
	IBOutlet NSButton *btnObjBrowser;
	IBOutlet NSButton *btnGotoObject;
	IBOutlet NSButton *btnSetTime;
	IBOutlet NSButton *btnExpertMode;
	IBOutlet NSButton *btnNoviceMode;

}

- (IBAction) stopDemo: (id) sender;
- (IBAction) reset: (id) sender;

-(BOOL)applicationShouldTerminate:(id)sender;
-(BOOL)windowShouldClose:(id)sender;
-(IBAction)back:(id)sender;
-(IBAction)forward:(id)sender;
-(IBAction)showGLInfo:(id)sender;
-(IBAction)showInfoURL:(id)sender;
-(void)runScript: (NSString*) path;
-(IBAction)openScript:(id)sender;
-(IBAction)rerunScript: (id) sender;
-(IBAction)toggleFullScreen:(id)sender;
-(BOOL)hideMenuBarOnActiveScreen;
-(void)setDirty;
-(void)forceDisplay;
-(void)resize;
-(void)startInitialization;
-(void)finishInitialization;
-(void)display;
-(void)awakeFromNib;
-(void)delegateKeyDown:(NSEvent *)theEvent;
-(void)keyPress:(int)code hold:(int)time;
-(void)setupResourceDirectory;
+(CelestiaController*) shared;
-(void) fatalError: (NSString *) msg;

-(IBAction) showPanel: (id) sender;

-(IBAction) captureMovie: (id) sender;

-(BOOL)validateMenuItem:(id)item;
-(IBAction)activateMenuItem:(id)item;

-(void)showHelp:(id)sender;

- (IBAction) setDeutsch: (id) sender;
- (IBAction) setEnglish: (id) sender;
- (IBAction) setExpertMode: (id) sender;
- (IBAction) setNoviceMode: (id) sender;
- (IBAction) showInfo: (id) sender;



@end
