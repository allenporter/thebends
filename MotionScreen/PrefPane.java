//
//	File:		PrefPane.java
//

import java.awt.*;
import java.awt.event.*;

public class PrefPane extends Frame implements ActionListener
{
    protected Button okButton;
    protected Label prefsText;

    public PrefPane()
    {
        super();
        this.setLayout(new BorderLayout(15, 15));
        this.setFont(new Font ("SansSerif", Font.BOLD, 14));

        prefsText = new Label ("MotionScreen Preferences...");
        Panel textPanel = new Panel(new FlowLayout(FlowLayout.CENTER, 15, 15));
        textPanel.add(prefsText);
        this.add (textPanel, BorderLayout.NORTH);
		
        okButton = new Button("OK");
        Panel buttonPanel = new Panel(new FlowLayout(FlowLayout.CENTER, 15, 15));
        buttonPanel.add (okButton);
        okButton.addActionListener(this);
        this.add(buttonPanel, BorderLayout.SOUTH);
        this.setSize(100,100);
    }
	

    public void actionPerformed(ActionEvent newEvent) 
    {
        setVisible(false);
    }	
	
}
