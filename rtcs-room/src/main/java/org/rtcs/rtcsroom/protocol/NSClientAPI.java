package org.rtcs.rtcsroom.protocol;

import java.io.IOException;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import org.rtcs.rtcsroom.protocol.ServerNativeImpl.RTCSAuth;
import org.rtcs.rtcsroom.resource.RoomResourceInterface;
import org.rtcs.rtcsroom.resource.RoomResourceInterface.UnSupportUserException;
import org.rtcs.rtcsroom.resource.RoomResourceInterface.User;
import org.rtcs.rtcsroom.resource.RoomResourceInterface.UserOverFlowException;
import com.corundumstudio.socketio.SocketIOClient;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;

public class NSClientAPI {
    
    private static final Logger log = LoggerFactory.getLogger(NSIOService.class);
    
    private RoomResourceInterface roomResouce = null;
    
    private RoomResourceInterface roomResourceInterface = null;
    
    private Map<Integer, SocketIOClient> userClient= null;
    
    public NSClientAPI(){}
    
    public void setRoomResourceInterface(RoomResourceInterface roomResourceInterface) {
        this.roomResourceInterface = roomResourceInterface;
    }


    public void setUserClient(Map<Integer, SocketIOClient> userClient) {
        this.userClient = userClient;
    }
    
    public void setRoomResouce(RoomResourceInterface roomResouce) {
        this.roomResouce = roomResouce;
    }
    
    public String Reply(String json,SocketIOClient client) {
        
        ObjectMapper mapper = new ObjectMapper();
        String type = null;
        try {
            JsonNode root = mapper.readTree(json);
            type = root.path("type").asText();
        } catch (IOException e) {
            log.error("userLogin Post json format not support:{}", json);
            return "";
        }
        String reply = null;
        switch(type) {
        case "login":
            reply = onUserLogin(json, client);
            break;
        case "ssologin":
            reply = onSSOLogin(json,client);
            break;
        case "tokenbyname":
            reply = createTokenByRoomName(json, client);
            break;
        }        

        return reply;
    }

    private Map<String, Integer> useridmap() {
        Map<Integer, User> users = roomResouce.getUsersMap();

        Map<String, Integer> useridmap = new ConcurrentHashMap<String, Integer>();

        for (Map.Entry<Integer, User> entry : users.entrySet()) {
            useridmap.put(entry.getValue().getUsername(), entry.getKey());
        }
        return useridmap;
    }
    
    private String createTokenByRoomName(String json, SocketIOClient client) {
        ObjectMapper mapper = new ObjectMapper();
        ObjectNode result = mapper.createObjectNode();
        result.put("type", "tokenbyname");
        
        String roomname = null;
        String user = null;
        String role = null;
        String roompassword = null;
        try {
            JsonNode root = mapper.readTree(json);
            roomname = root.path("roomname").asText();
            user = root.path("user").asText();
            role = root.path("role").asText();
            roompassword = root.path("roompwd").asText();
        } catch (IOException e) {
            log.info("createTokenByRoomName json parse error:{}", e.getMessage());
            return "";
        }
        Map<String, Integer> usermap = useridmap();
        if (!usermap.containsKey(user)) {
            log.info("cant't get current user's userid:username{}", user);
            return "";
        }

        int res = roomResouce.onPrepareEnterRoom(usermap.get(user), roomname, roompassword);

        if (res > 0) {            
            try {
                result.put("roomname", roomname);
                String sa = client.getRemoteAddress().toString();
                String remoteAddr = sa.substring(1, sa.indexOf(":"));// remote device ipaddr
                String token = NuveClient.createTokenByRoomname(remoteAddr, NSServer.getSocketIOIP(), roomname, user, role);
                result.put("token", token);
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
                return "";
            }
            
        } else {
            return "";
        }
    }

    private String onUserLogin(String json, SocketIOClient client) {
        ObjectMapper mapper = new ObjectMapper();
        ObjectNode result = mapper.createObjectNode();
        String user = null;
        String pwd = null;
        int userid=-1;
        try {
            JsonNode root = mapper.readTree(json);
            user = root.path("username").asText();
            pwd = root.path("password").asText();
            result.put("type", "login");
        } catch (IOException e) {
            log.error("userLogin Post json format not support:{}", json);
            return "";
        }

        if (user.isEmpty()) {
            log.error("userLogin Post json format not support:{}", json);
            return "";
        }

        User loginuser = null;
        try {
            loginuser = roomResouce.onVerifyUser(user, pwd);            
            userid = loginuser.getUserid();            
        } catch (UnSupportUserException eunsupport) {
            log.error("login faild:{}", eunsupport.getMessage());
            result.put("login", "fail");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
                return "";
            }
        } catch (UserOverFlowException eoverflow) {
            log.error("login useroverflow:{}", eoverflow.getMessage());
            result.put("login", "useroverflow");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
                return "";
            }
        }

        int opt = RTCSAuth.INSTANCE.AuthUser(loginuser.getUserid());
        if (opt == 1) {
            // has login
            result.put("login", "repeat");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
                return "";
            }
        } else if (opt == 0) {
            log.info("user login success:{},CurrentThreadid:{}", loginuser.getUsername(),
                    Thread.currentThread().getId());
            result.put("login", "success");
            ObjectNode child = mapper.createObjectNode();
            child.put("username", loginuser.getUsername());
            child.put("userid", loginuser.getUserid());
            child.put("role", loginuser.getRole());
            result.set("user", child);
        } else if (opt == -1) {
            // forbid login
            result.put("login", "forbidden");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        
        //onUserData
        if(userid <0)
            return "";
        int ret = RTCSAuth.INSTANCE.PutUser(userid, client.getSessionId().toString());
        if(0 == ret) {
            userClient.put(userid, client);
            String sa = client.getRemoteAddress().toString();
            String clientIp = sa.substring(1, sa.indexOf(":"));// remote device ip
            roomResourceInterface.onLoginSuccess(userid, clientIp);
            //return true;
            try {
                return mapper.writeValueAsString(result);
            }catch (JsonProcessingException e) {
                e.printStackTrace();
            }            
        }
        else {
            log.info("Can't connect more user,contact administrator get more user registeration"); 
            result.put("login", "useroverflow");
            try {
                return mapper.writeValueAsString(result);
            }catch (JsonProcessingException e) {
                e.printStackTrace();
            }    
        }
        return "";
    }
    
    private String onSSOLogin(String json,SocketIOClient client) {
        ObjectMapper mapper = new ObjectMapper();
        ObjectNode result = mapper.createObjectNode();
        String param = null;
        int userid=-1;
        try {
            JsonNode root = mapper.readTree(json);
            param = root.path("param").asText();
            result.put("type", "login");
        } catch (IOException e) {
            log.error("userLogin Post json format not support:{}", json);
            return "";
        }

        User loginuser = null;
        try {
            loginuser = roomResouce.onSSOVerifyUser(param);            
            userid = loginuser.getUserid();            
        } catch (UnSupportUserException eunsupport) {
            log.error("login faild:{}", eunsupport.getMessage());
            result.put("login", "fail");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
                return "";
            }
        } catch (UserOverFlowException eoverflow) {
            log.error("login useroverflow:{}", eoverflow.getMessage());
            result.put("login", "useroverflow");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
                return "";
            }
        }

        int opt = RTCSAuth.INSTANCE.AuthUser(loginuser.getUserid());
        if (opt == 1) {
            // has login
            result.put("login", "repeat");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
                return "";
            }
        } else if (opt == 0) {
            log.info("user login success:{},CurrentThreadid:{}", loginuser.getUsername(),
                    Thread.currentThread().getId());
            result.put("login", "success");
            ObjectNode child = mapper.createObjectNode();
            child.put("username", loginuser.getUsername());
            child.put("userid", loginuser.getUserid());
            child.put("role", loginuser.getRole());
            result.set("user", child);
        } else if (opt == -1) {
            // forbid login
            result.put("login", "forbidden");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        
        //onUserData
        if(userid <0)
            return "";
        int ret = RTCSAuth.INSTANCE.PutUser(userid, client.getSessionId().toString());
        if(0 == ret) {
            userClient.put(userid, client);
            String sa = client.getRemoteAddress().toString();
            String clientIp = sa.substring(1, sa.indexOf(":"));// remote device ip
            roomResourceInterface.onLoginSuccess(userid, clientIp);
            try {
                return mapper.writeValueAsString(result);
            }catch (JsonProcessingException e) {
                e.printStackTrace();
            }            
        }
        else {
            log.info("Can't connect more user,contact administrator get more user registeration"); 
            result.put("login", "useroverflow");
            try {
                return mapper.writeValueAsString(result);
            }catch (JsonProcessingException e) {
                e.printStackTrace();
            }    
        }
        return "";
    }
}
