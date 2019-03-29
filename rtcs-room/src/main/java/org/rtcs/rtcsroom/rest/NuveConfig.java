package org.rtcs.rtcsroom.rest;

import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

@Component
@ConfigurationProperties(prefix="nuve")
public class NuveConfig {
    private String _url="http://10.0.8.239:3000";
    private String _serviceid= "5a67e4357fb28f384401a09e";
    private String _servicekey= "32265";
    
    public String getUrl() {
        return _url;
    }
    
    public void setUrl(String url) {
        this._url=url;
    }
    
    public String getServiceID() {
        return _serviceid;
    }
    
    public void setServiceID(String serviceid) {
        this._serviceid=serviceid;
    }
    
    public String getServiceKey() {
        return _servicekey;
    }
    
    public void setServiceKey(String servicekey) {
        this._servicekey=servicekey;
    }
}
