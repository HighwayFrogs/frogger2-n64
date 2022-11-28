package me.kneesnap;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Main {

    public static void main(String[] args) throws IOException {
        //File n64Proto = new File("I:\\Blitz Games Archives\\Frogger 2\\Prototypes\\N64\\Final Known N64 Build\\Frogger 2 (USA) (Overdump Removed).n64");
        //removeOverdump(n64Proto, n64Proto, 0x9B8600);

        File linkerFile = new File("I:\\Playground\\frogger2-n64-cancelled\\N64\\Block.lnk");
        File inputFolder = new File("I:\\Playground\\frogger2-n64-cancelled\\GameData\\Exported Files\\");
        File outputFolder = new File("I:\\Playground\\frogger2-n64-cancelled\\GameData\\");
        renameExportedFiles(linkerFile, inputFolder, outputFolder);
    }

    public static void renameExportedFiles(File linkerFile, File inputFolder, File outputFolder) throws IOException {
        if (!linkerFile.isFile() || !linkerFile.exists() || !inputFolder.isDirectory() || !inputFolder.exists() || !outputFolder.isDirectory() || !outputFolder.exists()) {
            System.out.println("Input files/folders are not valid.");
            return;
        }

        List<String> lines = Files.readAllLines(linkerFile.toPath());

        String ghidraName = null;
        Map<File, byte[]> seenFiles = new HashMap<>();
        for (String line : lines) {
            String[] split = line.split("\\s+");

            for (int i = 0; i < split.length; i++) {
                String temp = split[i];
                if (temp.startsWith(";"))
                    break; // Commented out.

                if (temp.equals("alias")) {
                    String newKey = split[i - 1];
                    if (!newKey.endsWith("End") || ghidraName == null)
                        ghidraName = newKey;
                } else if (temp.equals("incbin")) {
                    String actualFilePath = split[i + 1].split(",")[0].replace("\"", "");

                    if (ghidraName == null) {
                        System.out.println("Couldn't determine input file name for '" + actualFilePath + "'.");
                        continue;
                    }

                    File inputFile = new File(inputFolder, ghidraName);
                    if (!inputFile.exists()) {
                        System.out.println("Couldn't find the file '" + ghidraName + "' to represent '" + actualFilePath + "'.");
                        continue;
                    }

                    File outputFile = new File(outputFolder, actualFilePath);
                    if (outputFile.exists()) {
                        byte[] existingData = seenFiles.computeIfAbsent(outputFile, readFile -> {
                            try {
                                return Files.readAllBytes(readFile.toPath());
                            } catch (IOException ex) {
                                throw new RuntimeException(ex);
                            }
                        });

                        byte[] inputBytes = Files.readAllBytes(inputFile.toPath());
                        if (Arrays.equals(existingData, inputBytes)) {
                            if (!inputFile.delete())
                                System.out.println("Failed to delete duplicated file '" + ghidraName + "'.");
                            ghidraName = null;
                        } else {
                            System.out.println("WARNING: Duplicate file '" + ghidraName + "' does not match '" + actualFilePath + "'.");
                        }

                        continue;
                    }

                    if (!outputFile.getParentFile().exists() && !outputFile.getParentFile().mkdirs())
                        System.out.println("Failed to create parent folder(s) for '" + actualFilePath + "'.");

                    if (inputFile.renameTo(outputFile)) {
                        System.out.println("Renamed '" + ghidraName + " to '" + actualFilePath + "'.");
                    } else {
                        System.out.println("FAILED to rename '" + ghidraName + " to '" + actualFilePath + "'.");
                    }
                }
            }
        }

        System.out.println("Done.");
    }

    public static void removeOverdump(File inputFile, File outputFile, int overdumpStart) throws IOException {
        byte[] bytes = Files.readAllBytes(inputFile.toPath());
        byte[] outputBytes = new byte[bytes.length];
        System.arraycopy(bytes, 0, outputBytes, 0, 0x9B8600);
        for (int i = overdumpStart; i < bytes.length; i += 0x10000) {
            int count = 0;
            for (int j = 0; j < Math.min(bytes.length - i, 0x10000); j++)
                if (bytes[i + j] != (byte)0x00)
                    count++;

            if (count > 0)
                System.out.println("Section 0x" + Integer.toHexString(i).toUpperCase() + " has " + count + " non-zero bytes.");
        }

        Files.write(outputFile.toPath(), outputBytes);
        System.out.println("Removed overdump.");
    }
}
