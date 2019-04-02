package org.rtcs.rtcsroom.protocol;

import com.corundumstudio.socketio.SocketIOClient;

interface NSCallBack {

    void onConnect(SocketIOClient client);
    
    void onDisconnect(SocketIOClient client);
    
    void onPing(SocketIOClient client);    
    
    String onClientReply(String json, SocketIOClient client);
    
    boolean onUserData(String json,SocketIOClient client);
    
    boolean onBuffer(SocketIOClient client,byte[] buffer);
}
