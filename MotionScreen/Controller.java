//
//  Controller.java
//  MotionScreen
//
//  Copyright 2007 Allen Porter. All rights reserved.
//

import java.awt.image.BufferedImage;
import quicktime.QTException;

public class Controller extends Thread {
  private Grabber grabber;

  private final BufferedImage imageA;
  private final BufferedImage imageB;
  private final BufferedImage imageC;
  private final BufferedImage imageD;
  private final Object lock;

  private boolean stopping = false;
  private boolean getReference = false;
  private int count = 0;
  private int width;
  private int height;

  private int threshold = 10;

  public interface Listener {
    public void updateImages();
  }
  private final Listener listener;

  Controller(BufferedImage imageA, BufferedImage imageB,
             BufferedImage imageC, BufferedImage imageD,
             Object lock, Listener listener) {
    width = imageA.getWidth();
    height = imageA.getHeight();
    this.imageA = imageA;
    this.imageB = imageB;
    this.imageC = imageC;
    this.imageD = imageD;
    this.lock = lock;
    this.listener = listener;
  }

  public void run() {
    try {
      System.out.println("Starting grabber...");

      grabber = new Grabber(imageA.getWidth(), imageA.getHeight());
      BufferedImage nextImage = grabber.getNextImage();
      BufferedImage refImage = new BufferedImage(nextImage.getWidth(),
                                                 nextImage.getHeight(), 
                                                 nextImage.getType());
      int count = 0;
      SequenceDiff diff = null;;
      while (!isStopping()) {
        System.out.println("Capturing...");
        synchronized(this) {
          if (getReference) {
            getReference = false;
            refImage.setData(nextImage.getData());
            diff = new SequenceDiff(refImage, 20);
          }
        }
        nextImage = grabber.getNextImage();
        BufferedImage absImage = null;
        BufferedImage combineImage = null;
        if (diff != null) {
          count++;
          if (count % 5 == 0) {
            diff = new SequenceDiff(refImage, getThreshold());
          }
          diff.addImage(nextImage);
          SequenceDiff.Image diffImage = diff.getAbsolute();
          absImage = filter(nextImage, diffImage);
          combineImage = combine(nextImage, imageB, diffImage);
        }
        synchronized(lock) {
          imageA.setData(nextImage.getData());
          if (diff != null) {
            imageC.setData(absImage.getData());
            imageD.setData(combineImage.getData());
          }     
        }
        listener.updateImages();
        try {
          Thread.sleep(100);
        } catch (InterruptedException e) { }
      } 
    } catch (QTException e) {
      e.printStackTrace();
    }
  }

  private final BufferedImage filter(BufferedImage start,
                                     SequenceDiff.Image filter) {
    BufferedImage r = new BufferedImage(width, height, start.getType());
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        if (filter.get(x, y) > 0) {
          r.setRGB(x, y, start.getRGB(x, y));
        } else {
          r.setRGB(x, y, 0xff0000);
        }
      }
    }
    return r;
  }

  private final BufferedImage combine(BufferedImage start,
                                      BufferedImage bg,
                                      SequenceDiff.Image filter) {
    BufferedImage r = new BufferedImage(width, height, start.getType());
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        if (filter.get(x, y) > 0) {
          r.setRGB(x, y, start.getRGB(x, y));
        } else {
          r.setRGB(x, y, bg.getRGB(x, y));
        }
      }
    }
    return r;
  }

  public synchronized void captureReference() {
    getReference = true;
    count = 0;
  }

  private synchronized boolean isStopping() {
    return stopping;
  }

  public synchronized void shutdown() {
    stopping = true;
  }


  public synchronized int getThreshold() {
    return threshold;
  }

  public synchronized void setThreshold(int threshold) {
    this.threshold = threshold;
  }

}
