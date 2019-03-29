package org.rtcs.rtcsroom;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.web.embedded.tomcat.TomcatServletWebServerFactory;
import org.springframework.boot.web.servlet.server.ServletWebServerFactory;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.scheduling.annotation.EnableScheduling;

import org.rtcs.rtcsroom.rest.TomCatConfig;

@SpringBootApplication
@ComponentScan(basePackages={"org.rtcs.rtcsroom"})
@EnableScheduling
public class RtcsRoomApplication {

    private static final Logger log = LoggerFactory.getLogger(RtcsRoomApplication.class);
    // tomcat config
    private TomCatConfig tomcatconfig;
    
    @Autowired
    public void setTomCatConfig(TomCatConfig config) {
        this.tomcatconfig=config;
    }
    
    @Bean
    public ServletWebServerFactory servletContainer() {
        TomcatServletWebServerFactory tomcat = new TomcatServletWebServerFactory();
        tomcat.setPort(tomcatconfig.getPort());
        if(!tomcatconfig.getInterface().isEmpty()) {
            try {
                Enumeration<NetworkInterface> netInterfaces = NetworkInterface.getNetworkInterfaces();
                
                while(netInterfaces.hasMoreElements()) {
                    NetworkInterface ni = netInterfaces.nextElement();
                    
                    Enumeration<InetAddress> ips = ni.getInetAddresses();
                    while(ips.hasMoreElements()) {
                        InetAddress address=ips.nextElement();
                        if(address instanceof Inet4Address && !address.isLoopbackAddress()
                                && address.getHostAddress().equals(tomcatconfig.getInterface())){
                            log.info("tomcat will starts at {}:{}",tomcatconfig.getInterface(),tomcatconfig.getPort());
                            tomcat.setAddress(address);
                            return tomcat;
                        }
                    }
                }
            } catch (SocketException e) {
                log.error("未能获取到可使用网卡，默认监听loopback地址");
                e.printStackTrace();
            }
        }
        log.info("未正确配置监听网络 interface,{}:{}，使用默认interface",tomcatconfig.getInterface(),tomcatconfig.getPort());
        return tomcat;
    }
    
    public static void main(String[] args) {
        SpringApplication.run(RtcsRoomApplication.class, args);
    }
}
