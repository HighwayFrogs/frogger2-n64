// Exports the collision + scenics from a Frogger 2 N64 ROM. Uses a hardcoded number of files.
//@author Kneesnap
//@category Functions
//@keybinding
//@menupath
//@toolbar

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Symbol;

import java.lang.Throwable;
import javax.swing.*;
import java.util.*;
import java.lang.RuntimeException;
import java.lang.StringBuilder;
import java.io.*;
import java.nio.file.Files;

public class GhidraCollisionScenicExporter extends GhidraScript {	
	private static final int FILE_COUNT = 52;

    @Override
    protected void run() throws Exception {
		Address addr = getSymbol("levelNames").getAddress();
		
		List<String> names = new ArrayList<>(FILE_COUNT);
		for (int i = 0; i < FILE_COUNT; i++) {
			byte[] temp = getBytes(addr, 32);
			
			StringBuilder nameBuilder = new StringBuilder();
			for (int j = 0; j < temp.length; j++) {
				if (temp[j] == 0)
					break;
				
				nameBuilder.append((char)temp[j]);
			}
			
			names.add(nameBuilder.toString());
			addr = addr.add(32);
		}
		
		List<Long> collisionBanksStart = new ArrayList<>();
		for (int i = 0; i < FILE_COUNT; i++) {
			collisionBanksStart.add((long)getInt(addr) & 0xFFFFFFFFL);
			addr = addr.add(4);
		}
		
		List<Long> collisionBanksEnd = new ArrayList<>();
		for (int i = 0; i < FILE_COUNT; i++) {
			collisionBanksEnd.add((long)getInt(addr) & 0xFFFFFFFFL);
			addr = addr.add(4);
		}
		
		List<Long> scenicBanksStart = new ArrayList<>();
		for (int i = 0; i < FILE_COUNT; i++) {
			scenicBanksStart.add((long)getInt(addr) & 0xFFFFFFFFL);
			addr = addr.add(4);
		}
		
		List<Long> scenicBanksEnd = new ArrayList<>();
		for (int i = 0; i < FILE_COUNT; i++) {
			scenicBanksEnd.add((long)getInt(addr) & 0xFFFFFFFFL);
			addr = addr.add(4);
		}
		
		// Read files.
		StringBuilder colFileLnk = new StringBuilder();
		StringBuilder sceFileLnk = new StringBuilder();
		for (int i = 0; i < FILE_COUNT; i++) {
			String name = names.get(i);
			
			long colFileStart = collisionBanksStart.get(i);
			long colFileEnd = collisionBanksEnd.get(i);
			byte[] colFileData = getBytes(toAddr(colFileStart), (int) (colFileEnd - colFileStart));
			File colFile = new File(name + ".bin");
			Files.write(colFile.toPath(), colFileData);
			
			long sceFileStart = scenicBanksStart.get(i);
			long sceFileEnd = scenicBanksEnd.get(i);
			byte[] sceFileData = getBytes(toAddr(sceFileStart), (int) (sceFileEnd - sceFileStart));
			File sceFile = new File(name + ".bi2");
			Files.write(sceFile.toPath(), sceFileData);
			
			String colBase = "collBank" + String.format("%03d", i);
			colFileLnk.append(";--\n\n")
				.append(colBase).append("      group   obj()\n")
				.append(colBase).append("bss   group   obj(),bss\n\n")
				.append("_").append(colBase).append("SegmentRomStart    alias   _").append(colBase).append("_org\n")
				.append("_").append(colBase).append("SegmentRomEnd      alias   _").append(colBase).append("_orgend\n\n")
				.append("           section.16\t").append(colBase).append(".data,").append(colBase).append("\n\n")
				.append("cBank").append(String.format("%03d", i)).append("  incbin  \"levbanks/bin/").append(name).append(".bin\",").append(colBase).append(".data,64\n\n");
				
			String sceBase = "scenBank" + String.format("%03d", i);
			sceFileLnk.append(";--\n\n")
				.append(sceBase).append("      group   obj()\n")
				.append(sceBase).append("bss   group   obj(),bss\n\n")
				.append("_").append(sceBase).append("SegmentRomStart    alias   _").append(sceBase).append("_org\n")
				.append("_").append(sceBase).append("SegmentRomEnd      alias   _").append(sceBase).append("_orgend\n\n")
				.append("           section.16\t").append(sceBase).append(".data,").append(sceBase).append("\n\n")
				.append("cBank").append(String.format("%03d", i)).append("  incbin  \"levbanks/bin/").append(name).append(".bi2\",").append(sceBase).append(".data,64\n\n");
		}
		
		println(colFileLnk.append(sceFileLnk.toString()).toString());
    }
		
	private Symbol getSymbol(String name) throws Exception {
		List<Symbol> symbols = getSymbols(name, null);
		if (symbols.size() != 1)
			throw new RuntimeException("Found " + symbols.size() + " symbols named '" + name + "'. (Expected: 1)");
		return symbols.get(0);
	}
}
