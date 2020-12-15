/*
 *
 *  Nolan Corcoran
 *  ndc466
 *
 */

import java.io.*;
import java.net.*;
import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

public class GraphPanel {

    final static Integer getDelay() {
        return MAX_DELAY - rateSlider.getValue();
    }

    static JFrame mainFrame;
    static Container mainPane;
    static JButton startButton, endButton;
    static JToggleButton pauseButton;
    static JPanel graphPane;
    static JSlider rateSlider;
    static Boolean painting = true;

    static final Integer MAX_DELAY = 50;

    public static void init(Integer WIDTH, Integer HEIGHT, Integer COLUMNS, Integer ROWS) {

        mainFrame = new JFrame("Distributed MST Implementation w/ TCP");
        mainPane = mainFrame.getContentPane();
        mainPane.setLayout(new BorderLayout());
        graphPane = new JPanel();
        graphPane.setLayout(null);
        graphPane.setPreferredSize(new Dimension(WIDTH,HEIGHT));
        mainPane.add(new JScrollPane(graphPane));
        JPanel buttonPanel = new JPanel();

        startButton = new JButton("start");
        startButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
                Server.setStarted(true);
                startButton.setEnabled(false);
            }
        });
        buttonPanel.add(startButton);

        pauseButton = new JToggleButton("pause");
        pauseButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
               synchronized(pauseButton) { pauseButton.notifyAll(); }
            }
        });
        buttonPanel.add(pauseButton);
        endButton = new JButton("terminate");
        endButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
               Server.closeConnections();
               Server.setStarted(false);
               synchronized(endButton) { endButton.notify(); }
            }
        });

        buttonPanel.add(endButton);

        JPanel sliderPanel = new JPanel(new BorderLayout());
        rateSlider = new JSlider(JSlider.VERTICAL,0,MAX_DELAY,MAX_DELAY/2);
        sliderPanel.add(rateSlider);
        sliderPanel.add(new JLabel("slow"),BorderLayout.SOUTH);
        sliderPanel.add(new JLabel("fast"),BorderLayout.NORTH);

        mainPane.add(buttonPanel, BorderLayout.SOUTH);
        mainPane.add(sliderPanel, BorderLayout.EAST);
        mainFrame.pack();
        mainFrame.show();
        mainFrame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent we) {
               Server.closeConnections();
               System.exit(0);
            }
        });
        // keep graph pane refreshed:
        (new Thread() {
            public void run() {
                while (true) {
                    try {
                        Thread.sleep(50);
                    } 
                    catch (InterruptedException ie) {}
                    if (painting) graphPane.repaint();
                }
            }
        }).start();
    }

    public static Integer getDegreeFromUser() {
        Integer maxDegree = Server.nodes.size() - 2;
        if (maxDegree == 2) return 2;
        boolean requireEvenDegree = (Server.nodes.size() % 2 == 0); // dummy node
        painting = false;
        final JSlider s = new JSlider(2, maxDegree, Math.min(maxDegree, Server.degree));
        s.setMajorTickSpacing(maxDegree - 2);
        if (requireEvenDegree) s.setMinorTickSpacing(2);
        else s.setMinorTickSpacing(1);
        s.setSnapToTicks(true);
        s.setPaintTicks(true);
        s.setPaintLabels(true);
        final String suffix = (requireEvenDegree) ? " (must be even)" : "";
        final JLabel label = new JLabel("Your choice: "+ (int) s.getValue() + suffix);
        label.setForeground(Color.blue);
        s.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ce) {
                label.setText("Your choice: "+ (int) s.getValue() + suffix);
            }
        });
        JPanel panel = new JPanel(new BorderLayout());
        panel.add(new JLabel("Select the minimum number of edges per node."), BorderLayout.NORTH);
        panel.add(s);
        panel.add(label, BorderLayout.SOUTH);
        JOptionPane.showMessageDialog(mainFrame, panel,"Choose degree", JOptionPane.QUESTION_MESSAGE);
        painting = true;
        return s.getValue();
    }

    public static void waitForTermination() {
        while (Server.isStarted()) {
            synchronized(endButton) { 
                try { endButton.wait(); } catch (InterruptedException ie) {}
            }
        }
        System.out.println("\n SESSION TERMINATED BY USER \n");
    }

}