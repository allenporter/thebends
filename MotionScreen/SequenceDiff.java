//
//  SequenceDiff.java
//  MotionScreen
//
//  Copyright 2007 Allen Porter. All rights reserved.
//

import java.awt.image.BufferedImage;

public class SequenceDiff {
  private final int width;
  private final int height;
  private final int type;
  private final int threshhold;

  // grayscale images
  private final Image reference;
  private final Image absolute;
  private final Image positive;
  private final Image negative;

  // color images
  private final BufferedImage referenceImage;
  private final BufferedImage lastImage;

  public static class Image {
    private final int width;
    private final int height;
    private final int[] pixels;

    Image(int width, int height) {
      this.width = width;
      this.height = height;
      this.pixels = new int[width * height];
    }

    public int get(int x, int y) {
      if (x > width || y > height) {
        throw new IllegalArgumentException("x=" + x + ",y=" + y);
      }
      return pixels[y * width + x];
    }

    public void set(int x, int y, int c) {
      if (x > width || y > height) {
        throw new IllegalArgumentException("x=" + x + ",y=" + y);
      }
      pixels[y * width + x] = c;
    }
  }

  public SequenceDiff(BufferedImage reference, int threshold) {
    this.width = reference.getWidth();
    this.height = reference.getHeight();
    this.type = reference.getType();
    this.threshhold = threshold;

    this.lastImage = new BufferedImage(width, height, type);
    this.lastImage.setData(reference.getData());
    this.referenceImage = new BufferedImage(width, height, type);
    this.referenceImage.setData(reference.getData());

    // init grayscal images
    this.reference = new Image(this.width, this.height);
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        this.reference.set(x, y, gray(reference.getRGB(x, y)));
      }
    }
    this.absolute = new Image(this.width, this.height);
    this.positive = new Image(this.width, this.height);
    this.negative = new Image(this.width, this.height);
  }

  private static int gray(int argb) {
    return 255 * (r(argb) + g(argb) + b(argb)) / (255 * 3);
  }

  private static int gray2rgb(int gray) {
    return (gray << 16) | (gray << 8) | gray;
  }

  private static int r(int argb) {
    return (argb & 0x00ff0000) >> 16;
  }

  private static int g(int argb) {
    return (argb & 0x0000ff00) >> 8;
  }

  private static int b(int argb) {
    return (argb & 0x000000ff);
  }

  public void addImage(BufferedImage image) {
    if (image.getWidth() != width || image.getHeight() != height ||
        image.getType() != type) {
      throw new IllegalArgumentException("image mismatch");
    }
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        int r = reference.get(x, y);
        int f = gray(image.getRGB(x, y));
        if (f > 255) {
          throw new RuntimeException("invalid gray value");
        }
        if (Math.abs(r - f) > threshhold) {
          absolute.set(x, y, absolute.get(x, y) + 1);
        }
        if ((r - f) > threshhold) {
          positive.set(x, y, positive.get(x, y) + 1);
        }
        if ((r - f) < (0 - threshhold)) {
          negative.set(x, y, negative.get(x, y) + 1);
        }
      }
    }
    lastImage.setData(image.getData());
  }

  public final Image getAbsolute() {
    return absolute;
  }

  public final Image getPositive() {
    return positive;
  }

  public final Image getNegative() {
    return negative;
  }
}
