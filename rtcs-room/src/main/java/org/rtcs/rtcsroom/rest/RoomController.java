package org.rtcs.rtcsroom.rest;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import org.rtcs.rtcsroom.protocol.NSServer;
import org.rtcs.rtcsroom.protocol.NSServer.NSServerInterface;
import org.rtcs.rtcsroom.protocol.ServerNativeImpl.RTCSAuth;
import org.rtcs.rtcsroom.protocol.NuveClient;
import org.rtcs.rtcsroom.protocol.ScheduleService;
import org.rtcs.rtcsroom.resource.InterfaceFactory;
import org.rtcs.rtcsroom.resource.RoomResourceInterface;
import org.rtcs.rtcsroom.resource.RoomResourceInterface.UnSupportUserException;
import org.rtcs.rtcsroom.resource.RoomResourceInterface.User;
import org.rtcs.rtcsroom.resource.RoomResourceInterface.UserOverFlowException;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;

@RestController
@RequestMapping("/API")
public class RoomController implements NSServerInterface {

    private static final Logger log = LoggerFactory.getLogger(RoomController.class);

    // scheduleservice
    private ScheduleService scheduleservice;

    @Autowired
    public void setScheduleservice(ScheduleService service) {
        this.scheduleservice = service;
    }

    @Autowired
    public void setNuveConfig(NuveConfig config) {
        NuveClient.setNuveConfig(config);
    }

    public RoomController() {
        NSServer.setNSInterface(this);
    }

    private RoomResourceInterface roomResouce = null;
    
    private RoomResourceInterface getRoomResourceInterface() {
        if (null == roomResouce) {
            // 这里初始化你自己的RoomResourceInterface
            // roomResouce = InterfaceFactory.createDefaultInterface();
            InterfaceFactory roomfactory = null;


            if (null == roomfactory) {
//                if (customconfig.getJar().equals("extendRoom.jar")) {
//                    roomfactory = new RoomServiceFactoryImpl();
//                    roomResouce = roomfactory.createInterface();
//                } else {
//                    roomResouce = InterfaceFactory.createDefaultInterface();
//                }
                roomResouce = InterfaceFactory.createDefaultInterface();
            } else {
                roomResouce = roomfactory.createInterface();
            }            
        }
        return roomResouce;
    }

    @RequestMapping("/index")
    public String index() {
        ObjectMapper mapper = new ObjectMapper();
        ObjectNode root = mapper.createObjectNode();
        ObjectNode child = mapper.createObjectNode();
        child.put("Method", "Http method");
        child.put("description", "describe this API");
        child.put("json example", "example of json format");
        root.set("API", child);

        // user Login
        child = mapper.createObjectNode();
        child.put("Method", "POST");
        child.put("description", "user login to the custom server");
        child.put("json exmaple", "{\"username\":\"zhaoj\",\"password\":\"zhaoj\"}");
        root.set("userLogin", child);

        // ssoLogin
        child = mapper.createObjectNode();
        child.put("Method", "POST");
        child.put("description", "user ssologin to the custom server");
        child.put("json exmaple", "{xxxxxx}");
        root.set("ssoLogin", child);

        // createRoom
        child = mapper.createObjectNode();
        child.put("Method", "POST");
        child.put("description", "Create a room in MCU Server by roomname");
        child.put("json exmaple", "{\"roomname\":\"zhaojtestroom\"}");
        root.set("createRoom", child);

        // getRoom
        child = mapper.createObjectNode();
        child.put("Method", "POST");
        child.put("description", "get specified roominfo by roomid");
        child.put("json exmaple", "{\"roomid\":\"5aa0a6eeb239871ba6c61933\"}");
        root.set("getRoom", child);

        // deleteRoom
        child = mapper.createObjectNode();
        child.put("Method", "POST");
        child.put("description", "delete specified room by roomid");
        child.put("json exmaple", "{\"roomid\":\"5aa0a6eeb239871ba6c61933\"}");
        root.set("deleteRoom", child);

        // getRooms
        child = mapper.createObjectNode();
        child.put("Method", "GET");
        child.put("description", "get all rooms in MCU");
        child.put("json exmaple", "no json");
        root.set("getRooms", child);

        // getUser
        child = mapper.createObjectNode();
        child.put("Method", "POST");
        child.put("description", "get specified user in specified room");
        child.put("json exmaple", "{\"roomid\":\"5aa0a6eeb239871ba6c61933\",\"user\":\"zhaoj\"}");
        root.set("getUser", child);

        // createToken
        child = mapper.createObjectNode();
        child.put("Method", "POST");
        child.put("description", "create token by serviceKey,connect MCU with token");
        child.put("json exmaple",
                "{\"roomid\":\"5aa0a6eeb239871ba6c61933\",\"user\":\"zhaoj\",\"role\":\"presenter\"}");
        root.set("createToken", child);

        // deleteUser
        child = mapper.createObjectNode();
        child.put("Method", "POST");
        child.put("description", "delete specified user in specified room");
        child.put("json exmaple", "{\"roomid\":\"5aa0a6eeb239871ba6c61933\",\"user\":\"zhaoj\"}");
        root.set("deleteUser", child);

        // APIRouter

        try {
            return mapper.writeValueAsString(root);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
            return "API description error";
        }
    }

    @RequestMapping(value = "/apiRouter", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String apiRouter(HttpServletRequest request, @RequestBody String json) {
        return roomResouce.onAPIRouter(request.getRemoteAddr(), json);
    }

    @RequestMapping(value = "/ssoLogin", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String ssoLogin(@RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
        ObjectNode result = mapper.createObjectNode();
        User loginuser = null;
        try {
            loginuser = roomResouce.onSSOVerifyUser(json);
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

            child = mapper.createObjectNode();
            child.put("host", String.format("http://%s:%d", NSServer.getSocketIOIP(), NSServer.getSocketIOPort()));
            result.set("socketio", child);
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
            return "";
        } else if (opt == -1) {
            // forbid login

        }
        return "";
    }

    @RequestMapping(value = "/userLogin", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String UserLogin(@RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
        ObjectNode result = mapper.createObjectNode();
        String user = null;
        String pwd = null;
        try {
            JsonNode root = mapper.readTree(json);
            user = root.path("username").asText();
            pwd = root.path("password").asText();
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

            child = mapper.createObjectNode();
            child.put("host", String.format("http://%s:%d", NSServer.getSocketIOIP(), NSServer.getSocketIOPort()));
            result.set("socketio", child);
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
            return "";
        } else if (opt == -1) {
            // forbid login
            result.put("login", "forbidden");
            try {
                return mapper.writeValueAsString(result);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return "";
    }

    @RequestMapping(value = "/userLogout", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public boolean userLogout(@RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
        int userid = -1;
        try {
            JsonNode root = mapper.readTree(json);
            userid = root.path("userid").asInt();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return NSServer.getIOService().disconnectUser(userid);
    }

    @RequestMapping(value = "/createRoom", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String createRoom(HttpServletRequest request, @RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
        String roomname = null;
        try {
            JsonNode root = mapper.readTree(json);
            roomname = root.path("roomname").asText();
        } catch (IOException e) {
            e.printStackTrace();
            return "";
        }

        if (null == roomname || roomname.isEmpty())
            return "";

        return NuveClient.getOrCreateRoomByName(request.getRemoteAddr(), NSServer.getSocketIOIP(), roomname);
    }

    @RequestMapping(value = "/getRoom", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String getRoom(@RequestBody String json) {
        return NuveClient.GetRoomInfo(json);

    }

    @RequestMapping(value = "/deleteRoom", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String deleteRoom(@RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
        ObjectNode root = mapper.createObjectNode();
        root.put("result", NuveClient.DeleteRoom(json));

        try {
            return mapper.writeValueAsString(root);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
            return "API description error";
        }
    }

    @RequestMapping("/updateRoom")
    public String updateRoom() {
        return NuveClient.UpdateRoom();
    }

    @RequestMapping("patchRoom")
    public String patchRoom() {
        return NuveClient.PatchRoom();
    }

    @RequestMapping("/getRooms")
    public String getRooms() {
        return NuveClient.getRooms();
        // return null;
    }

    @RequestMapping(value = "/getUserMap", method = RequestMethod.GET)
    public String getUserMap() {

        Map<Integer, User> allusers = roomResouce.getUsersMap();

        ObjectMapper mapper = new ObjectMapper();
        ArrayNode arrayNode = mapper.createArrayNode();

        for (Map.Entry<Integer, User> enrty : allusers.entrySet()) {
            ObjectNode root = mapper.createObjectNode();
            root.put("username", enrty.getValue().getUsername());
            root.put("userid", enrty.getValue().getUserid());
            root.put("role", enrty.getValue().getRole());
            arrayNode.add(root);
        }

        return arrayNode.toString();
    }

    private Map<String, Integer> useridmap() {
        Map<Integer, User> users = roomResouce.getUsersMap();

        Map<String, Integer> useridmap = new ConcurrentHashMap<String, Integer>();

        for (Map.Entry<Integer, User> entry : users.entrySet()) {
            useridmap.put(entry.getValue().getUsername(), entry.getKey());
        }
        return useridmap;
    }

    @RequestMapping(value = "/getUsersByRoomName", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String getUsersByRoomName(@RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
        String roomname = null;
        try {
            JsonNode root = mapper.readTree(json);
            roomname = root.path("roomname").asText();
        } catch (IOException e) {
            e.printStackTrace();
            log.info("getUsersByRoomName parse json post error");
            return "";
        }

        String users = NuveClient.getUsersByRoomName(roomname);

        return parseRoomUsersToJson(users);
    }

    private String parseRoomUsersToJson(String roomusers) {
        ObjectMapper mapper = new ObjectMapper();
        Map<String, Integer> userid = useridmap();

        ArrayNode result = mapper.createArrayNode();
        try {
            JsonNode root = mapper.readTree(roomusers);
            if (!root.isArray()) {
                return "";
            }
            Iterator<JsonNode> iterator = root.iterator();
            while (iterator.hasNext()) {
                ObjectNode info = mapper.createObjectNode();
                JsonNode node = iterator.next();
                String name = node.get("name").asText();
                int id = userid.get(name);
                info.put("username", name);
                info.put("userid", id);
                info.put("role", node.get("role").asText());
                info.set("permissions", node.get("permissions"));
                result.add(info);
            }
        } catch (IOException e) {
            e.printStackTrace();
            return "";
        }
        return result.toString();
    }

    @RequestMapping(value = "/getUsersByRoomID", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String getUsersByRoomID(@RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
        String roomid = null;
        try {
            JsonNode root = mapper.readTree(json);
            roomid = root.path("roomid").asText();
        } catch (IOException e) {
            e.printStackTrace();
            log.info("getUsersByRoomID parse json post error");
            return "";
        }

        String users = NuveClient.GetUsersByRoomid(roomid);

        return parseRoomUsersToJson(users);
    }

    @RequestMapping(value = "/getUser", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String getUser(@RequestBody String json) {
        return NuveClient.GetUser(json);
    }

    @RequestMapping(value = "/deleteUser", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String deleteUser(@RequestBody String json) {
        return NuveClient.DeleteUser(json);
    }

    @RequestMapping(value = "/createTokenByRoomName", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String createTokenByRoomName(HttpServletRequest request, @RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
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

        int result = roomResouce.onPrepareEnterRoom(usermap.get(user), roomname, roompassword);

        if (result > 0) {
            String addr = request.getRemoteAddr();
            return NuveClient.createTokenByRoomname(addr , NSServer.getSocketIOIP(), roomname, user,
                    role);
        } else {
            return "";
        }
    }

    @RequestMapping(value = "/createToken", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String createToken(@RequestBody String json) {
        ObjectMapper mapper = new ObjectMapper();
        String roomid = null;
        String user = null;
        String role = null;
        String roompassword = null;
        try {
            JsonNode root = mapper.readTree(json);
            roomid = root.path("roomid").asText();
            user = root.path("user").asText();
            role = root.path("role").asText();
            roompassword = root.path("roompwd").asText();
        } catch (IOException e) {
            log.info("createToken json parse error:{}", e.getMessage());
            return "";
        }
        if (null == roomid || roomid.isEmpty() || null == user || user.isEmpty() || null == role || role.isEmpty())
            return "";

        Map<String, Integer> usermap = useridmap();
        if (!usermap.containsKey(user)) {
            log.info("cant't get current user's userid:username{}", user);
            return "";
        }

        int result = roomResouce.onPrepareEnterRoom(usermap.get(user), roomid, roompassword);

        if (result > 0) {
            return NuveClient.createToken(roomid, user, role);
        } else {
            return "";
        }
    }

    @RequestMapping(value = "inviteRoom", method = RequestMethod.POST, produces = "application/json;charset=utf-8;")
    public String inviteRoom(@RequestBody String json) {

        return "";
    }

    @Override
    public void onNSServerStartUp() {
        NSServer.getIOService().BindRoomResourceInterface(getRoomResourceInterface());

        this.scheduleservice.setRoomResourceInterface(getRoomResourceInterface());
        
        NSServer.getIOService().setRoomResouce(roomResouce);
    }

    @Override
    public ArrayList<Integer> getUsersInRoomId(String roomid) {
        String json = NuveClient.GetUsersByRoomid(roomid);
        Map<String, Integer> mapuserid = useridmap();
        ObjectMapper mapper = new ObjectMapper();
        ArrayList<Integer> userids = new ArrayList<Integer>();

        try {
            JsonNode root = mapper.readTree(json);
            if (!root.isArray()) {
                return userids;
            }

            Iterator<JsonNode> iterator = root.iterator();
            while (iterator.hasNext()) {
                JsonNode node = iterator.next();
                String name = node.get("name").asText();
                int id = mapuserid.get(name);
                userids.add(id);
            }
        } catch (IOException e) {
            e.printStackTrace();
            return userids;
        }

        return userids;
    }
}
