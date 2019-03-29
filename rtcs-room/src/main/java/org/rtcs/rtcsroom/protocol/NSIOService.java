package org.rtcs.rtcsroom.protocol;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import org.rtcs.rtcsroom.protocol.NSServer.NSServerInterface;
import org.rtcs.rtcsroom.protocol.ServerNativeImpl.RTCSAuth;
import org.rtcs.rtcsroom.resource.RoomResourceInterface;
import org.rtcs.rtcsroom.resource.ServerCallBack;
import com.corundumstudio.socketio.SocketIOClient;
import com.corundumstudio.socketio.SocketIOServer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;

public class NSIOService implements NSCallBack,ServerCallBack {

    private static final Logger log = LoggerFactory.getLogger(NSIOService.class);
    
    private SocketIOServer ns=null;
    
    private Map<Integer, SocketIOClient> userClient=new ConcurrentHashMap<Integer,SocketIOClient>();
    
    private NSServerInterface callback;
    
    private RoomResourceInterface roomResourceInterface=null;
        
    private NSClientAPI clientAPI = null;
    
    public void setRoomResouce(RoomResourceInterface roomResouce) {
        clientAPI.setRoomResouce(roomResouce);
    }

    public NSIOService(SocketIOServer ns,NSServerInterface _callbak) {
        this.ns=ns;
        callback=_callbak;
        clientAPI = new NSClientAPI();
            
        clientAPI.setUserClient(userClient);
    }
    
    public void BindRoomResourceInterface(RoomResourceInterface _callback) {
        roomResourceInterface=_callback;
        roomResourceInterface.onBindServerCallback(this);
        clientAPI.setRoomResourceInterface(roomResourceInterface);
    }

	public boolean disconnectUser(Integer userid) {
        if(!userClient.containsKey(userid))
            return true;
        SocketIOClient client=userClient.get(userid);
        client.disconnect();
        userClient.remove(userid);
        RTCSAuth.INSTANCE.RemoveUser(userid);
        // 用户掉线 主动断开
        roomResourceInterface.onUserLogout(userid, 0);
        return true;
    }
    
    @Override
    public void onConnect(SocketIOClient client) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void onDisconnect(SocketIOClient client) {
        if(userClient.size() >0) {
            Iterator<Map.Entry<Integer, SocketIOClient>> iterator =userClient.entrySet().iterator();
            while (iterator.hasNext()) {
                Map.Entry<Integer, SocketIOClient> entry=iterator.next();  
                if(entry.getValue().getSessionId() == client.getSessionId()) {
                    // 用户掉线
                    RTCSAuth.INSTANCE.RemoveUser(entry.getKey());
                    roomResourceInterface.onUserLogout(entry.getKey(), 1);
                    iterator.remove();
                    break;
                }
            }
        }
        
    }

    @Override
    public void onPing(SocketIOClient client) {
        
        
    }
    
    @Override
    public String onClientReply(String json, SocketIOClient client) {            
        return clientAPI.Reply(json, client);
    }
    @Override
    public boolean onUserData(String json,SocketIOClient client) {
        ObjectMapper mapper = new ObjectMapper();
        
        int userid=-1;
        try {
            JsonNode root = mapper.readTree(json);
            userid=root.path("userid").asInt();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        if(userid <0)
            return false;
        int ret = RTCSAuth.INSTANCE.PutUser(userid, client.getSessionId().toString());
        if(0 == ret) {
            userClient.put(userid, client);
            String sa = client.getRemoteAddress().toString();
            String clientIp = sa.substring(1, sa.indexOf(":"));// remote device ip
            roomResourceInterface.onLoginSuccess(userid, clientIp);
            return true;
        }
        else {
            log.info("Can't connect more user,contact administrator get more user registeration");
        }
        return false;
    }

    @Override
    public boolean onBuffer(SocketIOClient client, byte[] buffer) {
        TypedTransBuffer trans=null;
        try {
            trans=TypedTransBuffer.parseFrom(buffer);
        } catch (InvalidProtocolBufferException e1) {
            log.error("unsupport byte buffer from socketio buffer event");
            e1.printStackTrace();
            return false;
        }
        
        int touser=trans.getTouser();
        //broadcast
        if(touser < 0) {
            // 我也不知道为什么上面这个不起作用 发了一个就不发了 下面那种同样的写法就可以
            //ns.getBroadcastOperations().sendEvent("onbuffer",client,buffer);
            for(SocketIOClient remote : ns.getAllClients()) {
                if(!remote.getSessionId().equals(client.getSessionId())) {
                    remote.sendEvent("onbuffer", buffer);
                }
            }
            
            return true;
        }
        
        // to server
        if(touser == 0) {
            int pluginID=trans.getPluginID();
            if(0== pluginID)
                return roomResourceInterface.onUserTransBuffer(trans.getFromuser(),trans.getBufferdata().toByteArray());
            else
                return roomResourceInterface.onUserTransBuffer(pluginID,trans.getFromuser(),trans.getBufferdata().toByteArray());    
        }
        
        int fromuser=trans.getFromuser();
        
        // broadcast to rooms
        if(touser == Integer.MAX_VALUE) {
            String roomid=trans.getRoomid();
            ArrayList<Integer> users=callback.getUsersInRoomId(roomid);
            if(null == users || users.size() ==0) {
                log.error("当前房间没有用户");
                return false;
            }
            for(Integer userid : users) {
                if(!userClient.containsKey(userid))
                {
                    log.error("socketio-user:{} 当前列表无该客户端",userid);
                    continue;
                }
                
                if(userid == fromuser)
                {
                    // excluded self
                    continue;
                }
                
                SocketIOClient toroom=userClient.get(userid);
                toroom.sendEvent("onbuffer", buffer);
            }
            return true;
        }
        
        if(!userClient.containsKey(touser)) {
            log.error("user {} do not online,can't send to ");
            return false;
        }
        SocketIOClient toClient=userClient.get(touser);
        toClient.sendEvent("onbuffer", buffer);    
        return true;
    }

    @Override
    public boolean Server_TransBuffer(int userid, byte[] buffer) {
        return Server_TransBuffer(0,userid,buffer);
    }

    @Override
    public boolean Server_TransBufferToRoom(String roomname, byte[] buffer) {
        return Server_TransBufferToRoom(0,roomname,buffer);
    }

    @Override
    public boolean Server_TransBuffer(int pluginID, int userid, byte[] buffer) {
        TypedTransBuffer.Builder builder=TypedTransBuffer.newBuilder();
        builder.setFromuser(0);
        builder.setTouser(userid);
        builder.setPluginID(pluginID);
        builder.setBufferdata(ByteString.copyFrom(buffer));
        builder.setRoomid("");
        TypedTransBuffer data=builder.build();
        if(userid < -1) {//广播给所有登录的
            for(SocketIOClient remote : ns.getAllClients()) {
                remote.sendEvent("onbuffer", data.toByteArray());
            }
            return true;
        }
        else {
            if(userClient.containsKey(userid)) {
                userClient.get(userid).sendEvent("onbuffer", data.toByteArray());
                return true;
            }
        }
        return false;
    }
    @Override
    public ArrayList<Integer> GetUsersInRoomId(String roomname){
        String roomid = NuveClient.getOrCreateRoomByName("extendserver",ns.getConfiguration().getHostname(),roomname);
        ArrayList<Integer> users=callback.getUsersInRoomId(roomid);
        return users;
    }
    @Override
    public boolean Server_TransBufferToRoom(int pluginID, String roomname, byte[] buffer) {
        String roomid = NuveClient.getOrCreateRoomByName("extendserver",ns.getConfiguration().getHostname(),roomname);
        ArrayList<Integer> users=callback.getUsersInRoomId(roomid);
        if(null == users || users.size() ==0) {
            log.error("当前房间没有用户");
            return false;
        }
        
        TypedTransBuffer.Builder builder=TypedTransBuffer.newBuilder();
        builder.setFromuser(0);
        builder.setPluginID(pluginID);
        builder.setBufferdata(ByteString.copyFrom(buffer));
        builder.setRoomid(roomid);
        
        for(Integer userid : users) {
            if(!userClient.containsKey(userid))
            {
                log.error("socketio-user:{} 当前列表无该客户端",userid);
                continue;
            }
            builder.setTouser(userid);
            TypedTransBuffer data=builder.build();
            SocketIOClient toroom=userClient.get(userid);
            toroom.sendEvent("onbuffer", data.toByteArray());
        }
        return true;
    }

    @Override
    public int Server_GetEnableUser() {
        return RTCSAuth.INSTANCE.GetEnableUser();
    }
}
