package org.rtcs.rtcsroom.rest;

import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

@Component
@ConfigurationProperties(prefix = "server")
public class TomCatConfig {
    private int _port=8080;
    private String _interface;
    
    public void setPort(int inport) {
        this._port=inport;
    }
    
    public int getPort() {
        return this._port;
    }
    
    public void setInterface(String inip) {
        this._interface=inip;
    }
    
    public String getInterface() {
        return this._interface;
    }
}
