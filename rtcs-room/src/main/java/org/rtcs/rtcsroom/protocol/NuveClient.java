package org.rtcs.rtcsroom.protocol;


import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.Date;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;

import org.apache.commons.codec.binary.Base64;
import org.apache.commons.codec.binary.Hex;
import org.apache.http.HttpEntity;
import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpDelete;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.util.EntityUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import org.rtcs.rtcsroom.rest.NuveConfig;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;

public class NuveClient {
    private static final Logger log = LoggerFactory.getLogger(NuveClient.class);
    private static final String HMAC_SHA1 = "HmacSHA1";
    private static final String ASCII = "ASCII";
    private static final String NUVE_HEADER="MAuth realm=http://marte3.dit.upm.es,mauth_signature_method=HMAC_SHA1";
    
    private static NuveConfig nuveconfig;
    
    public static void setNuveConfig(NuveConfig config) {
        nuveconfig=config;
        log.info("NuveClient config url:{}",nuveconfig.getUrl());
    }
    
    public static String createTokenByRoomname(String requestip,String serverip,String roomname,String user,String role) {
        if(null == roomname || roomname.isEmpty())
            return "";
        String roomid = getOrCreateRoomByName(requestip,serverip,roomname);
        if(null == roomid || roomid.isEmpty())
            return "";
        return createToken(roomid, user, role);
    }
    
    public static String createToken(String roomid,String user,String role) {
        if(null==roomid || null == user || null == role ||
                roomid.isEmpty()||user.isEmpty()||role.isEmpty())
            return "";
        
        HttpPost httpPost = new HttpPost(nuveconfig.getUrl()+String.format("/rooms/%s/tokens", roomid));
        
        return sendrequest(httpPost,null,user,role);
    }
    
    public String CreateService() {
        return "CreateService API not implemented yet";
    }
    
    public String GetServices() {
        return "GetServices API not implemented yet";
    }
    
    public String GetService() {
        return "GetService API not implemented yet";
    }
    
    public String DeleteService() {
        return "DeleteService API not implemented yet";
    }
    
    public static String GetUsersByRoomid(String roomid) {
        if(null == roomid || roomid.isEmpty())
            return "";
        // create HttpGet
        HttpGet httpGet = new HttpGet(nuveconfig.getUrl()+String.format("/rooms/%s/users/", roomid));
                
        return sendrequest(httpGet);
    }
    
    private static Map<String, String> getNuveRooms(){
        Map<String, String> roommap = new ConcurrentHashMap<String, String>();
        String roomsjson=getRooms();
        ObjectMapper mapper = new ObjectMapper();
        if(null != roomsjson && !roomsjson.isEmpty()){
            
            try {
                JsonNode rootnode=mapper.readTree(roomsjson);
                
                if(null != rootnode && rootnode.size()>0)
                {
                    for(JsonNode node : rootnode) {
                        roommap.put(node.get("name").asText(), node.get("_id").asText());
                    }
                }
            } catch (IOException e1) {
                e1.printStackTrace();
            }
        }
        return roommap;
    }
    
    public static String getUsersByRoomName(String roomname) {
        Map<String, String> roommap=getNuveRooms();
        if(!roommap.containsKey(roomname)) {
            return "";
        }
        return GetUsersByRoomid(roommap.get(roomname));
    }
    
    public static String GetUser(String json) {
        ObjectMapper mapper = new ObjectMapper();
        String roomid=null;
        String user=null;
        try {
            JsonNode root = mapper.readTree(json);
            roomid=root.path("roomid").asText();
            user=root.path("user").asText();
        } catch (IOException e) {
            e.printStackTrace();
            return "";
        }
        if(null==roomid || null == user ||
                roomid.isEmpty()||user.isEmpty())
            return "";
        
        // create HttpGet
        HttpGet httpGet = new HttpGet(nuveconfig.getUrl()+String.format("/rooms/%s/users/%s", roomid,user));
        
        return sendrequest(httpGet);
    }
    
    public static String DeleteUser(String json) {
        ObjectMapper mapper = new ObjectMapper();
        String roomid=null;
        String user=null;
        try {
            JsonNode root = mapper.readTree(json);
            roomid=root.path("roomid").asText();
            user=root.path("user").asText();
        } catch (IOException e) {
            e.printStackTrace();
            return "";
        }
        
        if(null==roomid || null == user || 
                roomid.isEmpty()||user.isEmpty())
            return "";
        // createHttpDelete
        HttpDelete httpDelete = new HttpDelete(nuveconfig.getUrl()+String.format("/rooms/%s/users/%s", roomid,user));
        
        return sendrequest(httpDelete);
    }
    
    public static String UpdateRoom() {
        return "updateRoom not implemented yet";
    }
    
    public static String PatchRoom() {
        return "PatchRoom not implemented yet";
    }
    
    public static String DeleteRoom(String json) {
        ObjectMapper mapper = new ObjectMapper();
        String roomid=null;
        try {
            JsonNode root = mapper.readTree(json);
            roomid=root.path("roomid").asText();
        } catch (IOException e) {
            e.printStackTrace();
            return "";
        }
        if(null == roomid || roomid.isEmpty())
            return "";
        
        // create httpdelete
        HttpDelete httpDelete = new HttpDelete(nuveconfig.getUrl()+String.format("/rooms/%s", roomid));
        
        return sendrequest(httpDelete);
    }
    
    public static String GetRoomInfo(String json) {
        ObjectMapper mapper = new ObjectMapper();
        String roomid=null;
        try {
            JsonNode root = mapper.readTree(json);
            roomid=root.path("roomid").asText();
        } catch (IOException e) {
            e.printStackTrace();
            return "";
        }
        if(null == roomid || roomid.isEmpty())
            return "";
        
        // create httpget
        HttpGet httpGet = new HttpGet(nuveconfig.getUrl()+String.format("/rooms/%s", roomid));
        
        return sendrequest(httpGet);
    }
    
    public synchronized static String getOrCreateRoomByName(String requestip,String serverip,String roomname) {
        // 先获取服务器所有房间列表
        String roomsjson=getRooms();
        String roomid=null;
        ObjectMapper mapper = new ObjectMapper();
        if(null != roomsjson && !roomsjson.isEmpty()){
            
            try {
                JsonNode rootnode=mapper.readTree(roomsjson);
                
                if(null != rootnode && rootnode.size()>0)
                {
                    for(JsonNode node : rootnode) {
                        if(node.get("name").asText().equals(roomname))
                        {
                            roomid=node.get("_id").asText();
                            break;
                        }
                    }
                }
            } catch (IOException e1) {
                e1.printStackTrace();
            }
        }
        if(null == roomid || roomid.isEmpty())
        {
            String roomjson= CreateRoom(requestip,serverip,roomname);
            if(null == roomjson || roomjson.isEmpty()) {
                return "create room fail from MCU";
            }
            try {
                JsonNode roomroot=mapper.readTree(roomjson);
                roomid=roomroot.get("_id").asText();
            }
            catch (IOException e) {
                log.info("parse json fail of create room from MCU");
                return "create room fail from MCU";
            }
        }
        return null == roomid ? "":roomid;
    }
    
    private static String CreateRoom(String requestip,String serverip,String roomname) {
        // create httppost
        HttpPost httpPost=new HttpPost(nuveconfig.getUrl()+"/rooms");
        
        ObjectMapper mapper = new ObjectMapper();
        ObjectNode root = mapper.createObjectNode();
        root.put("name", roomname);
        ObjectNode option=mapper.createObjectNode();
        option.put("mediaConfiguration", "default");
        ObjectNode roomdata=mapper.createObjectNode();
        
        roomdata.put("room_description", String.format("create %s by %s request from server:%s", roomname,requestip,serverip));
        option.set("data", roomdata);
        root.set("options", option);
        String body=null;
        try {
            body = mapper.writeValueAsString(root);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }
        
        return sendrequest(httpPost, body);
    }
    
    public static String getRooms() {
        // create HttpGet
        HttpGet httpGet = new HttpGet(nuveconfig.getUrl()+"/rooms");
        
        return sendrequest(httpGet);
    }
    
    private static String sendrequest(HttpUriRequest request,String body) {
        return sendrequest(request,body,null,null);
    }
    
    private static String sendrequest(HttpUriRequest request) {
        return sendrequest(request,null,null,null);
    }
    
    private static String sendrequest(HttpUriRequest request,String body,String username,String role) {
        CloseableHttpClient httpclient = HttpClients.createDefault();
        StringBuilder appender = new StringBuilder();
        appender.append(NUVE_HEADER);

        long nowtime = new Date().getTime();
        int number = (int) Math.floor(Math.random() * 99999);
        String tosign = String.format("%d,%d", nowtime, number);
        if(null != username && null != role) {
            appender.append(String.format(",mauth_username=%s", username));
            appender.append(String.format(",mauth_role=%s", role));
            
            tosign+=String.format(",%s,%s", username,role);
        }
        String signed = calculateSignature(tosign,nuveconfig.getServiceKey());
        appender.append(String.format(",mauth_serviceid=%s", nuveconfig.getServiceID()));
        appender.append(String.format(",mauth_cnonce=%d", number));
        appender.append(String.format(",mauth_timestamp=%d", nowtime));
        appender.append(String.format(",mauth_signature=%s", signed));

        request.setHeader("Authorization", appender.toString());
        if(null != body) {
            HttpEntityEnclosingRequest entityrequest=(HttpEntityEnclosingRequest)request;
            entityrequest.addHeader("Content-Type", "application/json");
            StringEntity stringEntity=null;
            try {
                stringEntity = new StringEntity(body);
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
            entityrequest.getRequestLine();
            entityrequest.setEntity(stringEntity);
        }
        String respondbody=null;
        try {
            CloseableHttpResponse response = httpclient.execute(request);
            try {
                HttpEntity entity = response.getEntity();
                if (null != entity) {
                    respondbody=EntityUtils.toString(entity);
                    System.out.println(respondbody);
                }
            } finally {
                response.close();
            }
        } catch (ClientProtocolException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            httpclient.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return respondbody;
    }
    
    public static String calculateSignature(String tosign,String key) {
        
        String digest = null;
        try {
            SecretKeySpec keyspec = new SecretKeySpec(key.getBytes("UTF-8"), HMAC_SHA1);
            Mac mac = Mac.getInstance(HMAC_SHA1);
            mac.init(keyspec);

            byte[] bytes = mac.doFinal(tosign.getBytes(ASCII));

            String hexsrt = Hex.encodeHexString(bytes);
            digest=new String(Base64.encodeBase64(hexsrt.getBytes()));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }

        return digest;
    }
}
