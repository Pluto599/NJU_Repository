package encoder;

import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.tools.JavaCompiler;
import javax.tools.ToolProvider;

import java.awt.image.BufferedImage;

public class SteganographyFactory {

    private static void compile(String classSource) {

        File sourceFile = new File(classSource);
        JavaCompiler compiler = ToolProvider.getSystemJavaCompiler();
        compiler.run(null, null, null, sourceFile.getPath());
    }

    public static void getSteganography(String classSource, String originImage) throws IOException {
        String className = classSource.substring(0, classSource.indexOf(".")).replace("/", ".");
        SteganographyFactory.compile(classSource);
        BufferedImage image = ImageIO.read(new File(originImage));
        SteganographyEncoder steganographyEncoder = new SteganographyEncoder(image);

        BufferedImage encodedImage = steganographyEncoder.encodeFile(new File(classSource.replace("java", "class")));
        ImageIO.write(encodedImage, "png", new File(className + ".png"));

    }

    public static void main(String[] args) throws IOException {

        SteganographyFactory.getSteganography("hulu/InsertSorter.java", "issac_logo.jpeg");

    }

}
