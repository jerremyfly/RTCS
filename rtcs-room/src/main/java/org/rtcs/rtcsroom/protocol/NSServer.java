package org.rtcs.rtcsroom.protocol;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Enumeration;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.stereotype.Component;

import com.corundumstudio.socketio.SocketIOServer;
import com.corundumstudio.socketio.listener.ConnectListener;
import com.corundumstudio.socketio.listener.DataListener;
import com.corundumstudio.socketio.listener.DisconnectListener;
import com.corundumstudio.socketio.listener.PingListener;
import org.rtcs.rtcsroom.protocol.ServerNativeImpl.RTCSAuth;
import org.rtcs.rtcsroom.rest.TomCatConfig;
import com.corundumstudio.socketio.AckRequest;
import com.corundumstudio.socketio.Configuration;
import com.corundumstudio.socketio.SocketIOClient;

@Component
public final class NSServer implements CommandLineRunner{

    public interface NSServerInterface{
        void onNSServerStartUp();
        ArrayList<Integer> getUsersInRoomId(String roomid);
    }
    
    private static final Logger log = LoggerFactory.getLogger(NSServer.class);
    
    private static SocketIOServer nettysocketio=null;
    
    private static String sockethost=null;
    
    private static int socketport=8383;
    
    private static NSIOService ioservice=null;
    
    private static NSServerInterface callback=null;
    
    public static void setNSInterface(NSServerInterface _callback) {
        callback= _callback;
    }
    
    public static NSIOService getIOService() {
        return ioservice;
    }
    
    public static String getSocketIOIP() {
        return sockethost;
    }
    
    public static int getSocketIOPort() {
        return socketport;
    }
    
    // tomcat config
    private TomCatConfig tomcatconfig;
    
    @Autowired
    public void setTomCatConfig(TomCatConfig config) {
        this.tomcatconfig=config;
    }
    
    private String getuseableInterface() {
        try {
            Enumeration<NetworkInterface> netInterfaces = NetworkInterface.getNetworkInterfaces();
            
            while(netInterfaces.hasMoreElements()) {
                NetworkInterface ni = netInterfaces.nextElement();
                
                Enumeration<InetAddress> ips = ni.getInetAddresses();
                while(ips.hasMoreElements()) {
                    InetAddress address=ips.nextElement();
                    if(address instanceof Inet4Address && !address.isLoopbackAddress()
                            && address.getHostAddress().equals(tomcatconfig.getInterface())){
                        return address.getHostAddress();
                    }
                }
            }
        } catch (SocketException e) {
            log.error("未能获取到可使用网卡，使用默认地址");
            e.printStackTrace();
        }

        try {
            return InetAddress.getLocalHost().getHostAddress();
        } catch (UnknownHostException e) {
            log.error("未能获取到默认地址，使用loopbakck地址");
            e.printStackTrace();
        }
        return InetAddress.getLoopbackAddress().getHostAddress();
    }
    
    private int getuseableport() {
        Socket socket = new java.net.Socket();
        InetSocketAddress inetAddress = new InetSocketAddress(0);
        int randomport=8383;
        try {
            socket.bind(inetAddress);
            randomport=socket.getLocalPort();
            socket.close();
        } catch (IOException e) {
            log.error("为 socketio 获取随机端口失败,使用默认8383端口.");
        }
        return randomport;
    }
    
    @Override
    public void run(String... args) throws UnknownHostException {
        log.info("Start socketio server");
        RTCSAuth.INSTANCE.InitServerAuth();
        log.info("loaded rtcsauth library");
        if(null == nettysocketio) {
            Configuration config = new Configuration();
            // get local ip address
            sockethost=getuseableInterface();
            socketport = RTCSAuth.INSTANCE.GetSocketIOPort();
            
            log.info("socketio 服务端 监听于 {}:{}",sockethost,socketport);
            
            config.setHostname(sockethost);
            config.setPort(socketport);
            config.setMaxFramePayloadLength(Integer.MAX_VALUE);
            config.setMaxHttpContentLength(Integer.MAX_VALUE);
                                    
            nettysocketio = new SocketIOServer(config);
            nettysocketio.addPingListener(new PingListener() {
            
                @Override
                public void onPing(SocketIOClient client) {
                    String sa = client.getRemoteAddress().toString();
                    String clientIp = sa.substring(1, sa.indexOf(":"));// remote device ip
                    log.info("netty socket io client ping event, ip address: {},session id:{}",
                            clientIp,
                            client.getSessionId().toString());
                    ioservice.onPing(client);
                }
            });
            nettysocketio.addConnectListener(new ConnectListener() {
                @Override
                public void onConnect(SocketIOClient client) {
                    String sa = client.getRemoteAddress().toString();
                    String clientIp = sa.substring(1, sa.indexOf(":"));// remote device ip
                    log.info("netty socket io client connected, ip address: {},session id:{}",
                            clientIp,
                            client.getSessionId().toString());
                    ioservice.onConnect(client);
                }
            });
            
            nettysocketio.addDisconnectListener(new DisconnectListener() {
                @Override
                public void onDisconnect(SocketIOClient client) {
                    String sa = client.getRemoteAddress().toString();
                    String clientIp = sa.substring(1, sa.indexOf(":"));// remote device ip
                    log.info("netty socket io client disconnected, ip address: {},session id:{}",
                            clientIp,
                            client.getSessionId().toString());
                    ioservice.onDisconnect(client);
                }
            });

            nettysocketio.addEventListener("buffer", byte[].class, new DataListener<byte[]>() {

                @Override
                public void onData(SocketIOClient client, byte[] data, AckRequest ackSender) throws Exception {
                    if(null == data) {
                        log.info("netty socket io buffer event:no data");
                        ackSender.sendAckData("no data");
                        return;
                    }
                    log.info("netty socket io buffer event");
                    ackSender.sendAckData(ioservice.onBuffer(client, data));
                }
            });
            
            nettysocketio.addEventListener("user", String.class, new DataListener<String>() {

                @Override
                public void onData(SocketIOClient client, String data, AckRequest ackSender) throws Exception {
                    // user json data
                    if(null != data) {
                        log.info("netty socket io user event:{}",data);
                        ackSender.sendAckData(ioservice.onUserData(data,client));
                    }
                }
            });
            
            nettysocketio.addEventListener("socketapi", String.class, new DataListener<String>() {
                @Override
                public void onData(SocketIOClient client, String data, AckRequest ackSender) throws Exception {
                    // login json data
                    if(null != data) {
                        log.info("netty socket io user event:{}",data);
                        ackSender.sendAckData(ioservice.onClientReply(data, client));
                    }
                }
            });
            
            nettysocketio.start();
            if(null == ioservice) {
                ioservice = new NSIOService(nettysocketio,callback);
            }
            callback.onNSServerStartUp();
        }
    }

}
