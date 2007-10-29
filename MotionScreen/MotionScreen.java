//
//  MotionScreen.java
//  MotionScreen
//
//  Created by Allen Porter on 6/16/07.
//  Copyright (c) 2007 __MyCompanyName__. All rights reserved.
//
//	For information on setting Java configuration information, including 
//	setting Java properties, refer to the documentation at
//		http://developer.apple.com/techpubs/java/java.html
//

import java.util.Locale;
import java.util.ResourceBundle;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import com.apple.eawt.*;
import quicktime.QTException;
import java.io.FileInputStream;
import com.sun.image.codec.jpeg.JPEGCodec;
import com.sun.image.codec.jpeg.JPEGImageDecoder;

public class MotionScreen extends Frame {

    private Font font = new Font("serif", Font.ITALIC+Font.BOLD, 36);
    protected ResourceBundle resbundle;
    protected AboutBox aboutBox;
    protected PrefPane prefs;
    private Application fApplication = Application.getApplication();

    protected Action newAction, openAction, closeAction, saveAction,
                     saveAsAction, undoAction, cutAction, copyAction,
                     pasteAction, clearAction, selectAllAction;
	
    // Declarations for menus
    static final MenuBar mainMenuBar = new MenuBar();
	
    protected Menu fileMenu;
    protected MenuItem miNew;
    protected MenuItem miOpen;
    protected MenuItem miClose;
    protected MenuItem miSave;
    protected MenuItem miSaveAs;
	
    protected Menu editMenu;
    protected MenuItem miUndo;
    protected MenuItem miCut;
    protected MenuItem miCopy;
    protected MenuItem miPaste;
    protected MenuItem miClear;
    protected MenuItem miSelectAll;

    protected Button buttonReference;
    protected Button buttonBG;
    protected TextComponent thresholdText;

    private final static int width = 320;
    private final static int height = 240;

    private final Controller controller;

    private BufferedImage imageA;
    private BufferedImage imageB;
    private BufferedImage imageC;
    private BufferedImage imageD;

    public MotionScreen() {
      super("");
      WindowAdpt WAdapter = new WindowAdpt();
      this.addWindowListener(WAdapter);
                                
      // The ResourceBundle below contains all of the strings used in this
      // application.  ResourceBundles are useful for localizing applications
      // new localities can be added by adding additional properties files.  
      resbundle = ResourceBundle.getBundle("MotionScreenstrings",
                                           Locale.getDefault());
      setTitle (resbundle.getString("frameConstructor"));
      setLayout(null);
      addMenus();
      addButtons();
      addText();
        
      fApplication.setEnabledPreferencesMenu(true);
      fApplication.addApplicationListener(
        new com.apple.eawt.ApplicationAdapter() {
          public void handleAbout(ApplicationEvent e) {
            if (aboutBox == null) {
              aboutBox = new AboutBox();
            }
            about(e);
            e.setHandled(true);
          }

          public void handleOpenApplication(ApplicationEvent e) { }

          public void handleOpenFile(ApplicationEvent e) { }
       
          public void handlePreferences(ApplicationEvent e) {
            if (prefs == null) {
               prefs = new PrefPane();
            }
            preferences(e);
          }

          public void handlePrintFile(ApplicationEvent e) { }
          
          public void handleQuit(ApplicationEvent e) {
            quit(e);
          }
        });

      imageA = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
      imageB = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
      imageC = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
      imageD = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);

      controller = new Controller(imageA, imageB, imageC, imageD, this,
                                  new Controller.Listener() {
                                    public void updateImages() {
                                      System.err.println("Update");
                                      repaint();
                                    }
                                  });
      controller.start();
      thresholdText.setText(controller.getThreshold() + "");

      setVisible(true);
      setSize(width * 2 + 1, height * 2 + 35);
    }

    public void about(ApplicationEvent e) {
      aboutBox.setResizable(false);
      aboutBox.setVisible(true);
      aboutBox.show();
    }

    public void preferences(ApplicationEvent e) {
      prefs.setResizable(false);
      prefs.setVisible(true);
      prefs.show();
    }

    public void quit(ApplicationEvent e) {
      handleQuit();
    }

    public void createActions() {
      int shortcutKeyMask =
        Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();

      // Create actions that can be used by menus, uttons, toolbars, etc.
      newAction = new newActionClass(resbundle.getString("newItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_N, shortcutKeyMask));
      openAction = new openActionClass(resbundle.getString("openItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_O, shortcutKeyMask));
      closeAction = new closeActionClass(resbundle.getString("closeItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_W, shortcutKeyMask));
      saveAction = new saveActionClass(resbundle.getString("saveItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_S, shortcutKeyMask));
      saveAsAction = new saveAsActionClass(resbundle.getString("saveAsItem"));

      undoAction = new undoActionClass(resbundle.getString("undoItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_Z, shortcutKeyMask));
      cutAction = new cutActionClass(resbundle.getString("cutItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_X, shortcutKeyMask));
      copyAction = new copyActionClass(resbundle.getString("copyItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_C, shortcutKeyMask));
      pasteAction = new pasteActionClass(resbundle.getString("pasteItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_V, shortcutKeyMask));
      clearAction = new clearActionClass(resbundle.getString("clearItem"));
      selectAllAction = new selectAllActionClass(
          resbundle.getString("selectAllItem"),
          KeyStroke.getKeyStroke(KeyEvent.VK_A, shortcutKeyMask) );
    } 
    
    public void addFileMenuItems() {
      miNew = new MenuItem (resbundle.getString("newItem"));
      miNew.setShortcut(new MenuShortcut(KeyEvent.VK_N, false));
      fileMenu.add(miNew).setEnabled(true);
      miNew.addActionListener(newAction);

      miOpen = new MenuItem (resbundle.getString("openItem"));
      miOpen.setShortcut(new MenuShortcut(KeyEvent.VK_O, false));
      fileMenu.add(miOpen).setEnabled(true);
      miOpen.addActionListener(openAction);

      miClose = new MenuItem (resbundle.getString("closeItem"));
      miClose.setShortcut(new MenuShortcut(KeyEvent.VK_W, false));
      fileMenu.add(miClose).setEnabled(true);
      miClose.addActionListener(closeAction);

      miSave = new MenuItem (resbundle.getString("saveItem"));
      miSave.setShortcut(new MenuShortcut(KeyEvent.VK_S, false));
      fileMenu.add(miSave).setEnabled(true);
      miSave.addActionListener(saveAction);

      miSaveAs = new MenuItem (resbundle.getString("saveAsItem"));
      miSaveAs.setShortcut(new MenuShortcut(KeyEvent.VK_S, true));
      fileMenu.add(miSaveAs).setEnabled(true);
      miSaveAs.addActionListener(saveAsAction);

      mainMenuBar.add(fileMenu);
    }
	
    public void addEditMenuItems() {
      miUndo = new MenuItem(resbundle.getString("undoItem"));
      miUndo.setShortcut(new MenuShortcut(KeyEvent.VK_Z, false));
      editMenu.add(miUndo).setEnabled(true);
      miUndo.addActionListener(undoAction);
      editMenu.addSeparator();
		
      miCut = new MenuItem(resbundle.getString("cutItem"));
      miCut.setShortcut(new MenuShortcut(KeyEvent.VK_X, false));
      editMenu.add(miCut).setEnabled(true);
      miCut.addActionListener(cutAction);
		
      miCopy = new MenuItem(resbundle.getString("copyItem"));
      miCopy.setShortcut(new MenuShortcut(KeyEvent.VK_C, false));
      editMenu.add(miCopy).setEnabled(true);
      miCopy.addActionListener(copyAction);
		
      miPaste = new MenuItem(resbundle.getString("pasteItem"));
      miPaste.setShortcut(new MenuShortcut(KeyEvent.VK_V, false));
      editMenu.add(miPaste).setEnabled(true);
      miPaste.addActionListener(pasteAction);
		
      miClear = new MenuItem(resbundle.getString("clearItem"));
      editMenu.add(miClear).setEnabled(true);
      miClear.addActionListener(clearAction);
      editMenu.addSeparator();
		
      miSelectAll = new MenuItem(resbundle.getString("selectAllItem"));
      miSelectAll.setShortcut(new MenuShortcut(KeyEvent.VK_A, false));
      editMenu.add(miSelectAll).setEnabled(true);
      miSelectAll.addActionListener(selectAllAction);
		
      mainMenuBar.add(editMenu);
    }
	
    public void addMenus() {
      editMenu = new Menu(resbundle.getString("editMenu"));
      fileMenu = new Menu(resbundle.getString("fileMenu"));
      addFileMenuItems();
      addEditMenuItems();
      setMenuBar (mainMenuBar);
    }

    public void addButtons() {
      buttonReference = new Button("Get Reference");
      buttonReference.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            controller.captureReference();
          }
        });
      buttonReference.setSize(100, 20);
      buttonReference.setLocation(width - 50, height * 2 + 7);
      this.add(buttonReference);

      buttonBG = new Button("Set Background");
      buttonBG.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            getFile();
          }
        });
      buttonBG.setSize(100, 20);
      buttonBG.setLocation(width * 2 - 120 + 7, height * 2 + 7);
      this.add(buttonBG);
    }

    public void getFile() {
      FileDialog dialog = new FileDialog(this);
      dialog.setVisible(true);
      try {
        FileInputStream fs = new FileInputStream(dialog.getDirectory() + "/" +
                                                 dialog.getFile());
        JPEGImageDecoder decoder = JPEGCodec.createJPEGDecoder(fs);
        BufferedImage img = decoder.decodeAsBufferedImage();
        fs.close();

        Image i = img.getScaledInstance(width, height, Image.SCALE_FAST);
        Graphics2D g = imageB.createGraphics();
        synchronized(this) {
          g.drawImage(i, 0, 0, null);
        }
      } catch (Exception e) {
        e.printStackTrace();
      }
    }

    public void addText() {
      thresholdText = new TextField();
      thresholdText.addTextListener(new TextListener() {
          public void textValueChanged(TextEvent event) {
            try {
              int value = Integer.parseInt(thresholdText.getText());
              controller.setThreshold(value);
            } catch (Exception e) {
              thresholdText.setText("0");
              controller.setThreshold(0);
              e.printStackTrace();
            }
          }
        });
      thresholdText.setLocation(10, height*2 + 7);
      thresholdText.setSize(40, 20);
      this.add(thresholdText);
    }

    public void paint(Graphics g) {
      synchronized(this) {
        g.drawImage(imageA, 0, 0, null);
        g.drawImage(imageB, width + 1, 0, null);
        g.drawImage(imageC, 0, height + 1, null);
        g.drawImage(imageD, width + 1, height + 1, null);
      }
      super.paint(g);
    }
    
    public void handleAbout() {
      aboutBox.setResizable(false);
      aboutBox.setVisible(true);
      aboutBox.show();
    }
	
    public void handleQuit() {	
      controller.shutdown();
      while (true) {
        try {
          controller.join();
          break;
        } catch (InterruptedException e) {
          // oh well?
        }
      }
      System.exit(0);
    }

    public class newActionClass extends AbstractAction {
      public newActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("New...");
      }
    }

    public class openActionClass extends AbstractAction {
      public openActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Open...");
      }
    }
	
    public class closeActionClass extends AbstractAction {
      public closeActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Close...");
      }
    }
	
    public class saveActionClass extends AbstractAction {
      public saveActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Save...");
      }
    }
	
    public class saveAsActionClass extends AbstractAction {
      public saveAsActionClass(String text) {
        super(text);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Save As...");
      }
    }
	
    public class undoActionClass extends AbstractAction {
      public undoActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Undo...");
      }
    }
	
    public class cutActionClass extends AbstractAction {
      public cutActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Cut...");
      }
    }
	
    public class copyActionClass extends AbstractAction {
      public copyActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Copy...");
      }
    }
	
    public class pasteActionClass extends AbstractAction {
      public pasteActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Paste...");
      }
    }
	
    public class clearActionClass extends AbstractAction {
      public clearActionClass(String text) {
        super(text);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Clear...");
      }
    }
	
    public class selectAllActionClass extends AbstractAction {
      public selectAllActionClass(String text, KeyStroke shortcut) {
        super(text);
        putValue(ACCELERATOR_KEY, shortcut);
      }
      public void actionPerformed(ActionEvent e) {
        System.out.println("Select All...");
      }
    }
	
    class WindowAdpt extends java.awt.event.WindowAdapter {
      public void windowClosing(java.awt.event.WindowEvent event) {
        handleQuit();
      }
    }
    
    public static void main(String args[]) {
      new MotionScreen();
    }
}
