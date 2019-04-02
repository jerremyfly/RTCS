package org.rtcs.rtcsroom.resource;

public abstract class InterfaceFactory {
	public static RoomResourceInterface createDefaultInterface() {
		return new RoomResource();
	}
	
	public RoomResourceInterface createInterface() {
		return createDefaultInterface();
	}
}
