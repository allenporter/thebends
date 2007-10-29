//
//  Grabber.java
//  MotionScreen
//
//  Copyright 2007 Allen Porter. All rights reserved.
//

import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.WritableRaster;

import quicktime.*;
import quicktime.qd.*;
import quicktime.std.*;
import quicktime.std.sg.*;
import quicktime.util.RawEncodedImage;

public class Grabber {
  private final int width;
  private final int height;

  private final SequenceGrabber grabber;
  private final SGVideoChannel channel;
  private final RawEncodedImage rowEncodedImage;
  private final BufferedImage image;
  private final WritableRaster raster;

  private final int videoWidth;
  private final int[] pixels;

  public Grabber(int width, int height) throws QTException {
    this.width = width;
    this.height = height;

    QTSession.open();

    QDRect bounds = new QDRect(width, height);
    QDGraphics graphics = new QDGraphics(bounds);

    grabber = new SequenceGrabber();
    grabber.setGWorld(graphics, null);

    channel = new SGVideoChannel(grabber);
    channel.setBounds(bounds);
    channel.setUsage(StdQTConstants.seqGrabPreview);

    //channel.settingsDialog();
    grabber.prepare(true, false);
    grabber.startPreview();

    PixMap pixMap = graphics.getPixMap();
    rowEncodedImage = pixMap.getPixelData();

    videoWidth = width + (rowEncodedImage.getRowBytes() - width * 4) / 4;
    pixels = new int[videoWidth * height];
    image = new BufferedImage(
        videoWidth, height, BufferedImage.TYPE_INT_RGB);
    raster = WritableRaster.createPackedRaster(DataBuffer.TYPE_INT,
        videoWidth, height,
        new int[] { 0x00ff0000, 0x0000ff00, 0x000000ff }, null);
    raster.setDataElements(0, 0, width, height, pixels);
    image.setData(raster);
    QTRuntimeException.registerHandler(new QTRuntimeHandler() {
        public void exceptionOccurred(
                QTRuntimeException e, Object eGenerator,
                String methodNameIfKnown, boolean unrecoverableFlag) {
            System.out.println("what should i do?");
        }
    });
  }

  public void stop() throws QTException {
    try {
      grabber.stop();
      grabber.release();
      grabber.disposeChannel(channel);
      image.flush();
    } catch (Exception e) {
      e.printStackTrace();
    } finally {
      QTSession.close();
    }
  }

  public BufferedImage getNextImage() throws QTException {
    grabber.idle();
    rowEncodedImage.copyToArray(0, pixels, 0, pixels.length);
    raster.setDataElements(0, 0, videoWidth, height, pixels);
    image.setData(raster);
    return image;
  }
}
