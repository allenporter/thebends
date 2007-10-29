// OpenGLDemo.m
// Author: Allen Porter <allen@thebends.org>
//
// Currently just draws a triangle on the screen and rotates it around the
// the center of the screen, then goes black.
//
// You should add the OpenGLES/ headers to your toolchain include directory.
//
// TODO: Find out how to make the screen buffer stay around, so we don't have
// to block the main thread to keep our image on the screen.

#import "OpenGLDemo.h"
#import <Foundation/Foundation.h>
#import <CoreSurface/CoreSurface.h>
#import <LayerKit/LKLayer.h>
#import <OpenGLES/gl.h>
#import <OpenGLES/egl.h>
#include <unistd.h>

static EGLint const attribute_list[] = {
  EGL_RED_SIZE, 1,
  EGL_GREEN_SIZE, 1,
  EGL_BLUE_SIZE, 1,
  EGL_NONE
};

EGLConfig config;
EGLContext context;
EGLSurface surface;
EGLDisplay display;
EGLint num_config;
CoreSurfaceBufferRef native_window;

void teardown() {
  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(display, surface);
  eglDestroyContext(display, context);
  eglTerminate(display);
}

void sig_handler(int ignored) {
  teardown();
  exit(0);
}

@implementation LKLayer (FooLayer)

- (void)drawInContext:(struct CGContext *)fp8
{
  NSLog(@"drawInContext");
  //  return [super drawInContext:fp8]
  const GLbyte KVertices []=
  {
     0, 1, 0,
    -1, 0, 0,
     1, 0, 0,
  };

  double i;
  for (i = 0; i < 360; i += 10.0) {
    /* connect the context to the surface */
    eglMakeCurrent(display, surface, surface, context);

    /* clear the color buffer */
    glClearColor(0.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glLoadIdentity();
    glVertexPointer(3, GL_BYTE , 0, KVertices);  
    glRotatef(i, 0.0, 0.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glFlush();
    eglSwapBuffers(display, surface);

    usleep(10 * 1000);
  }
}

@end

@interface OpenGLView : UIView { } @end

@implementation OpenGLView

- (id)initWithFrame:(CGRect)rect
{
  self = [super initWithFrame:rect];
  return self;
}

- (void)drawRect:(CGRect)rect
{
  NSLog(@"drawRect");
  return [super drawRect:rect];
}

@end

@implementation OpenGLDemo

- (void)applicationExited:(struct __GSEvent *)fp8;      // IMP=0x323b24c4
{
  teardown();
}

- (void) applicationDidFinishLaunching: (id) unused
{
  NSLog(@"applicationDidFinishLaunching");

  // Register a handler for ^C, so we can shutdown cleanly when running 
  signal(SIGINT, &sig_handler);

  CGRect rect = [UIHardware fullScreenApplicationContentRect];
  rect.origin.x = rect.origin.y = 0;
  UIWindow *window = [[UIWindow alloc] initWithContentRect:rect];
  [window orderFront: self];
  [window makeKey: self];
  [window _setHidden: NO];

  /* get an EGL display connection */
  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  /* initialize the EGL display connection */
  eglInitialize(display, NULL, NULL);
  /* get an appropriate EGL frame buffer configuration */
  eglChooseConfig(display, attribute_list, &config, 1, &num_config);
  /* create an EGL rendering context */
  context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
  /* create a native window */
  OpenGLView* view = [[OpenGLView alloc] initWithFrame:rect];
  native_window = [[view _layer] contents];

  /* create an EGL window surface */
  surface = eglCreateWindowSurface(display, config, native_window, NULL);
  /* connect the context to the surface */
  eglMakeCurrent(display, surface, surface, context);

  [window setContentView:view];
}

@end
