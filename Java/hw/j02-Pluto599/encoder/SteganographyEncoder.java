package encoder;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;

public class SteganographyEncoder {
    private final BufferedImage bi;
    private int bitsFromColor;
    private int mask;

    public SteganographyEncoder(BufferedImage bufferedImage) {
        this(bufferedImage, 2);
    }

    public SteganographyEncoder(BufferedImage bufferedImage, int bitsFromColor) {
        setBitsFromColor(bitsFromColor);
        this.bi = bufferedImage;
    }

    public BufferedImage encodeString(String message) throws IllegalArgumentException {
        if (message == null || message.length() == 0) {
            throw new IllegalArgumentException("Message can not be empty!");
        }
        char[] characters = message.toCharArray();
        byte[] messageLen = intToByteArray(message.length());
        byte[] bytes = new byte[4 + characters.length];
        for (int i = 0; i < 4; i++) {
            bytes[i] = messageLen[i];
        }
        for (int i = 0; i < characters.length; i++) {
            bytes[i + 4] = (byte) characters[i];
        }

        return encode(bytes);
    }

    public byte[] decodeByteArray() throws DecodingException {
        byte[] bytes = decode();
        int nameSize = byteArrayToInt(Arrays.copyOfRange(bytes, 0, 4));
        if (nameSize <= 0 || nameSize > (bytes.length - 8)) {
            throw new DecodingException("NameSize", nameSize);
        }
        int fileSize = byteArrayToInt(Arrays.copyOfRange(bytes, 4, 8));
        if (fileSize < 0 || fileSize > (bytes.length - 8)) {
            throw new DecodingException("DecodedFileSize", fileSize);
        }
        if (nameSize + fileSize > (bytes.length - 8)) {
            throw new DecodingException("NameSize and DecodedFileSize", nameSize + fileSize);
        }
        return Arrays.copyOfRange(bytes, 8 + nameSize, 8 + nameSize + fileSize);
    }

    public String decodeString() {
        StringBuilder sb = new StringBuilder();
        byte[] decodedByteArray = decode();

        int messageLen = byteArrayToInt(Arrays.copyOfRange(decodedByteArray, 0, 4));

        for (int i = 0; i < messageLen; i++) {
            char character = (char) decodedByteArray[i + 4];
            sb.append(character);
        }

        return sb.toString();
    }

    public BufferedImage encodeFile(File file) throws IOException {
        byte[] bytes = IOUtils.toByteArray(new FileInputStream(file));
        byte[] sizeBytes = intToByteArray(bytes.length);

        char[] nameChars = file.getName().toCharArray();
        byte[] nameBytes = new byte[nameChars.length];
        for (int i = 0; i < nameChars.length; i++) {
            nameBytes[i] = (byte) nameChars[i];
        }
        byte[] sizeNameBytes = intToByteArray(nameBytes.length);

        byte[] finalBytes = new byte[4 + 4 + nameBytes.length + bytes.length];
        System.arraycopy(sizeNameBytes, 0, finalBytes, 0, 4);
        System.arraycopy(sizeBytes, 0, finalBytes, 4, 4);
        System.arraycopy(nameBytes, 0, finalBytes, 8, nameBytes.length);
        System.arraycopy(bytes, 0, finalBytes, 8 + nameBytes.length, bytes.length);

        return encode(finalBytes);
    }

    public File decodeFile(String resultPath) throws DecodingException {
        byte[] bytes = decode();
        int nameSize = byteArrayToInt(Arrays.copyOfRange(bytes, 0, 4));
        if (nameSize <= 0 || nameSize > (bytes.length - 8)) {
            throw new DecodingException("NameSize", nameSize);
        }
        int fileSize = byteArrayToInt(Arrays.copyOfRange(bytes, 4, 8));
        if (fileSize < 0 || fileSize > (bytes.length - 8)) {
            throw new DecodingException("DecodedFileSize", fileSize);
        }
        if (nameSize + fileSize > (bytes.length - 8)) {
            throw new DecodingException("NameSize and DecodedFileSize", nameSize + fileSize);
        }
        byte[] nameBytes = Arrays.copyOfRange(bytes, 8, 8 + nameSize);
        byte[] fileBytes = Arrays.copyOfRange(bytes, 8 + nameSize, 8 + nameSize + fileSize);

        StringBuilder sb = new StringBuilder();
        for (byte nameByte : nameBytes) {
            sb.append((char) nameByte);
        }
        String name = sb.toString();
        File file = new File(resultPath + "decoded_" + name);
        try {
            FileUtils.writeByteArrayToFile(file, fileBytes);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return file;
    }

    public int getBitsFromColor() {
        return bitsFromColor;
    }

    public void setBitsFromColor(int bitsFromColor) {
        checkBitsFromColor(bitsFromColor);
        this.bitsFromColor = bitsFromColor;
        mask = calculateMask(bitsFromColor);
    }

    public int getMaxNoOfBytes() {
        int nrOfPixels = this.bi.getWidth() * this.bi.getHeight();
        return Math.floorDiv(nrOfPixels * bitsFromColor * 3, 8);
    }

    private BufferedImage encode(byte[] bytes) {
        int[] pixels = this.bi.getRGB(0, 0, this.bi.getWidth(), this.bi.getHeight(), null, 0, this.bi.getWidth());
        int maxNoOfBytes = getMaxNoOfBytes();
        if (bytes.length > maxNoOfBytes) {
            throw new IllegalArgumentException("File to big, max no of bytes: " + maxNoOfBytes);
        }

        int smallMask = (int) (Math.pow(2, bitsFromColor) - 1);
        int curColor = 2;
        int curPix = 0;
        int charOffset = 0;

        pixels[0] &= mask;
        for (byte aByte : bytes) {
            while (charOffset < 8) {
                if (curColor < 0) {
                    curColor = 2;
                    curPix++;
                    pixels[curPix] &= mask;
                }

                char temp = (char) ((aByte >> 8 - bitsFromColor - charOffset) & smallMask);
                pixels[curPix] |= (temp << curColor * 8);

                charOffset += bitsFromColor;
                curColor--;
            }
            charOffset %= 8;
        }

        BufferedImage bufferedImage = new BufferedImage(this.bi.getWidth(), this.bi.getHeight(),
                BufferedImage.TYPE_INT_ARGB);
        bufferedImage.setRGB(0, 0, this.bi.getWidth(), this.bi.getHeight(), pixels, 0, this.bi.getWidth());
        return bufferedImage;
    }

    private byte[] decode() {
        int[] pixels = this.bi.getRGB(0, 0, this.bi.getWidth(), this.bi.getHeight(), null, 0, this.bi.getWidth());
        int maxNoOfBytes = getMaxNoOfBytes();
        byte[] result = new byte[maxNoOfBytes];
        int smallMask = (int) (Math.pow(2, bitsFromColor) - 1);
        int curColor = 2;
        int curPix = 0;
        int charOffset = 0;

        // TODO: Optimize this code to decode only needed number of bytes and not the
        // whole byte array
        for (int i = 0; i < maxNoOfBytes; i++) {
            byte oneByte = 0;
            while (charOffset < 8) {
                if (curColor < 0) {
                    curColor = 2;
                    curPix++;
                }
                char temp = (char) (pixels[curPix] >> (8 * curColor) & smallMask);
                oneByte |= temp << 8 - bitsFromColor - charOffset;

                charOffset += bitsFromColor;
                curColor--;
            }
            result[i] = oneByte;
            charOffset %= 8;
        }
        return result;
    }

    private void checkBitsFromColor(int bitsFromColor) {
        if (!Arrays.asList(1, 2, 4, 8).contains(bitsFromColor)) {
            throw new IllegalArgumentException("Number of used bits from color must be in set {1,2,4,8}");
        }
    }

    private int calculateMask(int bitsFromColor) {
        int temp = (int) (Math.pow(2, bitsFromColor) - 1);
        int mask = 0;
        for (int i = 0; i < 3; i++) {
            mask <<= 8;
            mask |= temp;
        }
        return ~mask;
    }

    private byte[] intToByteArray(int integer) {
        byte[] result = new byte[4];
        for (int i = 3; i >= 0; i--) {
            result[3 - i] = (byte) (integer >> (i * 8));
        }
        return result;
    }

    private int byteArrayToInt(byte[] bytes) {
        if (bytes.length != 4) {
            return 0;
        }
        int result = 0;
        int littleMask = 255;
        for (byte aByte : bytes) {
            int intFromByte = littleMask & aByte;
            result <<= 8;
            result |= intFromByte;
        }

        return result;
    }
}
