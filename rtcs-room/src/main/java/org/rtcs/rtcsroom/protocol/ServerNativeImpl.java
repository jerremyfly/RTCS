package org.rtcs.rtcsroom.protocol;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
public class ServerNativeImpl{
    public interface RTCSAuth extends Library{

        public final static RTCSAuth INSTANCE = new RTCSAuth() {
            @Override
            public void InitServerAuth() {

            }

            @Override
            public int GetEnableUser() {
                return 999;
            }

            @Override
            public int GetSocketIOPort() {
                return 8383;
            }

            @Override
            public int AuthUser(int userid) {
                return 0;
            }

            @Override
            public int PutUser(int userid, String sessionid) {
                return 0;
            }

            @Override
            public int RemoveUser(int userid) {
                return 0;
            }
        };
//                (RTCSAuth)Native.loadLibrary(Platform.isWindows()?("./RTCSAuth.dll"):("./libRTCSAuth.so"),
//                        RTCSAuth.class);
        
        void InitServerAuth();
        //void InitServerAuth(String version);
        
        /**
         * @return value: int get enable total user
         */
        int GetEnableUser();
        
        int GetSocketIOPort();
        
        /**
         * return value: -1 forbid login ,1 has logind, 0 got login access
         */
        int AuthUser(int userid);
        
        int PutUser(int userid, String sessionid);
        
        /**
        * return value:0 remove success,-1 no user valid
        */
        int RemoveUser(int userid);
    }
}

