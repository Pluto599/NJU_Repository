
import java.awt.event.KeyEvent;

import asciiPanel.AsciiPanel;
import hulu.World;

public class WorldScreen implements Screen {

    private World world;

    public WorldScreen() throws Exception {
        world = new World();

    }

    @Override
    public void displayOutput(AsciiPanel terminal) {

        for (int x = 0; x < World.WIDTH; x++) {
            for (int y = 0; y < World.HEIGHT; y++) {
                terminal.write(world.get(x, y).getGlyph(),
                        x,
                        y,
                        world.get(x, y).getColor());
            }
        }
    }

    int i = 0;

    @Override
    public Screen respondToUserInput(KeyEvent key) {

        if (i < world.steps().length) {
            world.next(i);
            i++;
        }

        return this;
    }

}
