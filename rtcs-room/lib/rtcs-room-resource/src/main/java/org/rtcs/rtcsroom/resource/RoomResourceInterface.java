package org.rtcs.rtcsroom.resource;

import java.util.Map;

public interface RoomResourceInterface {
	// 基本用户信息定义
	public class User {
		private String user_name;
		private int user_id;
		private String user_role;

		public User(String name, int id) {
			this(name, id, "presenter");
		}

		public User(String name, int id, String role) {
			user_name = name;
			user_id = id;
			user_role = role;
		}

		public String getUsername() {
			return user_name;
		}

		public void setUsername(String name) {
			user_name = name;
		}

		public int getUserid() {
			return user_id;
		}

		public void setUserid(int _id) {
			user_id = _id;
		}

		public String getRole() {
			return user_role;
		}

		public void setRole(String Role) {
			user_role = Role;
		}
	}

	// 用户校验失败异常
	public class UnSupportUserException extends Exception {

		private static final long serialVersionUID = 1L;

		public UnSupportUserException() {
			super("not valid user");
		}
	}
	
	// 超出用户授权数限制
	public class UserOverFlowException extends Exception{
		private static final long serialVersionUID = 1L;

		public UserOverFlowException() {
			super("user overflow");
		}
	}
	
	// 触发计划任务
	public void onCronSchedule();
	
	// http json接口转发
	public String onAPIRouter(String fromip,String json);
	
	// 绑定服务器回调
	public void onBindServerCallback(ServerCallBack callBack);
	
	// 用户登录校验接口，登录成功返回User对象，否则抛出异常
	public User onVerifyUser(String username, String pwd) throws UnSupportUserException,UserOverFlowException;
	
	// 用户单点登录校验接口，同样登录成功返回User对象，否则抛出异常
	public User onSSOVerifyUser(String json) throws UnSupportUserException,UserOverFlowException;

	// 用户登录成功接口
	public void onLoginSuccess(int userid, String ipaddress);
	
	// 用户离线接口 userid=用户id errorcode=0-用户主动离线 errorcode=1-用户掉线
	public void onUserLogout(int userid,int errorcode);
	
	// 用户申请进入房间回调 
	// param:userid 用户id roomid 房间id roompwd 房间密码
	// return 1 房间校验成功 -1 该用户不支持 -2 房间不存在 -3 房间密码错误
	public int onPrepareEnterRoom(int userid,String roomid,String roompassword);

	// 返回所有用户ID与用户对象列表
	public Map<Integer, User> getUsersMap();

	// 收到用户发送给服务器的缓冲区数据 equals pluginID=0
	public boolean onUserTransBuffer(int fromuser, byte[] buffer);
	// 收到用户发送给服务器的缓冲区数据
	public boolean onUserTransBuffer(int pluginID,int fromuser,byte[] buffer);

	// 可调用API
	public class APICAll {
//		public static boolean Server_TransBuffer(int userid, ByteString buffer) {
//			return NSServer.getIOService().TransBufferToUser(userid, buffer);
//		}
	}
}
