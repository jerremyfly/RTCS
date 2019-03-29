package org.rtcs.rtcsroom.resource;

import java.util.ArrayList;

public interface ServerCallBack {
	// userid <= -2 向所有登录的用户广播buffer消息 userid > 0 向指定用户发送buffer
	public boolean Server_TransBuffer(int userid, byte[] buffer);
	
	public boolean Server_TransBuffer(int pluginID,int userid,byte[] buffer);
	
	// 向指定roomid 的所有用户广播buffer
	public boolean Server_TransBufferToRoom(String roomname,byte[] buffer);
	
	public boolean Server_TransBufferToRoom(int pluginID,String roomname,byte[] buffer);
	
	// 获取可用用户数
	public int Server_GetEnableUser();
	
	//获取房间内用户
	public ArrayList<Integer> GetUsersInRoomId(String roomname);
}
