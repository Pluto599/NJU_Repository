import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JFrame;

import asciiPanel.AsciiFont;
import asciiPanel.AsciiPanel;
import hulu.World;

public class Main extends JFrame implements KeyListener {
    private AsciiPanel terminal;
    private Screen screen;

    public Main(int method) {
        super();
        terminal = new AsciiPanel(World.WIDTH, World.HEIGHT, AsciiFont.HACK_64_64);
        add(terminal);
        pack();
        screen = new WorldScreen(this::repaint, method);
        addKeyListener(this);
        repaint();
    }

    @Override
    public void repaint() {
        terminal.clear();
        screen.displayOutput(terminal);
        super.repaint();
    }

    @Override
    public void keyTyped(KeyEvent e) {

    }

    @Override
    public void keyPressed(KeyEvent e) {
        screen = screen.respondToUserInput(e);
        repaint();
    }

    @Override
    public void keyReleased(KeyEvent e) {

    }

    public static void main(String[] args) {
        int method = 0; // Default value
        if (args.length > 0) {
            method = Integer.parseInt(args[0]);
        }
        else
        {
            System.out.println("No sorting method specified. Using default (0). \n 0: Bubble Sort, 1: Insertion Sort");
        }
        Main app = new Main(method);
        app.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        app.setVisible(true);
    }

}
