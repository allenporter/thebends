//
//	File:		AboutBox.java
//

import java.awt.*;
import java.awt.event.*;
import java.util.Locale;
import java.util.ResourceBundle;

public class AboutBox extends Frame implements ActionListener
{
    protected Label titleLabel, aboutLabel[];
    protected static int labelCount = 8;
    protected static int aboutWidth = 280;
    protected static int aboutHeight = 230;
    protected static int aboutTop = 200;
    protected static int aboutLeft = 350;
    protected Font titleFont, bodyFont;
    protected ResourceBundle resbundle;
    
    public AboutBox()
    {
        super();
        this.setResizable(false);
        resbundle = ResourceBundle.getBundle ("MotionScreenstrings", Locale.getDefault());
        SymWindow aSymWindow = new SymWindow();
        this.addWindowListener(aSymWindow);	
        
        this.setLayout(new BorderLayout(15, 15));
        Font titleFont = new Font("Lucida Grande", Font.BOLD, 14);
        if (titleFont == null) {
            titleFont = new Font("SansSerif", Font.BOLD, 14);
        }
        Font bodyFont  = new Font("Lucida Grande", Font.PLAIN, 10);
        if (bodyFont == null) {
            bodyFont = new Font("SansSerif", Font.PLAIN, 10);
        }

        this.setLayout(new BorderLayout(15, 15));

        aboutLabel = new Label[labelCount];
        aboutLabel[0] = new Label("");
        aboutLabel[1] = new Label(resbundle.getString("frameConstructor"));
        aboutLabel[1].setFont(titleFont);
        aboutLabel[2] = new Label(resbundle.getString("appVersion"));
        aboutLabel[2].setFont(bodyFont);
        aboutLabel[3] = new Label("");
        aboutLabel[4] = new Label("");
        aboutLabel[5] = new Label("JDK " + System.getProperty("java.version"));
        aboutLabel[5].setFont(bodyFont);
        aboutLabel[6] = new Label(resbundle.getString("copyright"));
        aboutLabel[6].setFont(bodyFont);
        aboutLabel[7] = new Label("");

        Panel textPanel = new Panel(new GridLayout(labelCount, 1));
        for (int i = 0; i<labelCount; i++) {
                aboutLabel[i].setAlignment(Label.CENTER);
                textPanel.add(aboutLabel[i]);
        }
    
        this.add (textPanel, BorderLayout.CENTER);
        this.setLocation(aboutLeft, aboutTop);
        this.setSize(aboutWidth, aboutHeight);
    }
	
    class SymWindow extends java.awt.event.WindowAdapter {
	    public void windowClosing(java.awt.event.WindowEvent event) {
		    setVisible(false);
	    }
    }
    
    public void actionPerformed(ActionEvent newEvent) 
    {
        setVisible(false);
    }	
	
}
