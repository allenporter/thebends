# iPhone

**With the release of the Apple SDK, this page is not as useful as it once was,
however i've kept it around for historical value.**

This page is a summary of some work I've done related to the iPhone. I've also
included links to interesting iPhone projects. Dig through some of the
Community links if you want to get started playing with iPhone development.

# MobileSynth

[MobileSynth](https://github.com/allenporter/mobilesynth) is a synthesizer.

# MobileTerminal

[MobileTerminal](https://github.com/allenporter/mobileterminal) is a Terminal
emulator for the iPhone. The current unstable release supports vt100 and
scrollback, as well as support for gestures for scrolling and arrow keys.

# iphonedisk

[iphonedisk](https://github.com/allenporter/iphonedisk) is a A MacFUSE
filesystem that allows you to mount the filesystem of your iPhone when
connected with the USB cable.

# UIKit Examples

Most iPhone GUI applications so far have been based on UIKit. I've create a
few small example programs to play with different parts of the UIKit, and 
share them to provide examples for others.

* HelloApplication - is a mirror of the original "hello world" application for UIKit, plus a small bit of code for listing files on the filesystem in a table.
* TextViewDemo - The TextViewDemo was the start of MobileTerminal, and simply executes /bin/ls /Applications and displays the output to a TextView.
* TextDrawing - The CoreGraphics library is used in TextDrawing to show an alternative way to display text on the screen without using a TextView. This code has been used in the vt100-iterm branch of MobileTerminal. The original version of this demo used UIStringDrawing, but the core graphics drawing is more interesting.
* OpenGLDemo - OpenGLDemo shows how to use the OpenGLES.framework on the iPhone. Currently this demo draws a blue background with a rotating white triangle, and needs a lot of work before it could be used in a real application. Suggestions for better ways to do this are welcome! See Andrew J. Willmott's iphone-gl directory. He has much better demos than I do.
* KeyboardLayout - KeyboardLayout has example code for changing the layout of the keyboard (including landscape mode and sublayouts).
* TileView - TileView is a demo program that uses a UITileView and UITile classes, as well as a UIScroller. I think these might have a use in the vt100-iterm branch of MobileTerminal, which does not currently have any scrolling.
* CoreTelephony - telephony is a demo that uses the CoreTelephony framework. Maybe the Cell ID and Station numbers can be used for obtaining location information? CellStumbler is a project which attempts to log this info.
* MobileMusicLibrary - music-player contains some code that uses the MobileMusicLibrary framework. I have some future plans for this.

# Disassembly

You will probably find that disassembling iPhone frameworks comes in handy, especially since there is no documentation for any UIKit headers. Currently, your best choices are ravel-arm and otool-arm.

Example usage of ravel-arm on an iPhone framework:

```
$ ravel-arm -Talksun UIKit
```

