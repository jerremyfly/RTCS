package org.rtcs.rtcsroom.resource;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

class RoomResource implements RoomResourceInterface {

	Lock lock = new ReentrantLock();// 锁  
	
	private Map<Integer, User> AllUser = null;

	private int useridindex=0;
	
	RoomResource() {
		AllUser = new ConcurrentHashMap<Integer, User>();
//		AllUser.put(1, new User("zhaoj", 1));
//		AllUser.put(2, new User("zhaot", 2));
//		AllUser.put(3, new User("dqk", 3));
//		AllUser.put(4, new User("lzb", 4));
//		AllUser.put(999, new User("admin", 999));
//		AllUser.put(5, new User("ltb", 5));
//		AllUser.put(6, new User("lr", 6));
//		AllUser.put(7, new User("lyk", 7));
//		AllUser.put(8, new User("rnn", 8));
	}
	
	public User onVerifyUser(String username, String pwd) throws UnSupportUserException{
		// TODO Auto-generated method stub
		// check login
		lock.lock();
		User curUser = null;
		for (Map.Entry<Integer, User> entry : AllUser.entrySet()) {
			User cUser = entry.getValue();
			if (cUser.getUsername().equals(username)) {
				curUser = cUser;
				break;
			}
		}
		
		if(username.equals("meinv")) {
			lock.unlock();
			throw new UnSupportUserException();
		}
		
		if(null == curUser) {
			// 没有该用户则新加入
			useridindex++;
			curUser = new User(username, useridindex);
			AllUser.put(useridindex, curUser);
		}
		lock.unlock();
		
		return curUser;
	}

	public void onLoginSuccess(int userid, String ipaddress) {
		// TODO Auto-generated method stub

	}

	public Map<Integer, User> getUsersMap() {
		// TODO Auto-generated method stub
		return AllUser;
	}

	public boolean onUserTransBuffer(int fromuser, byte[] buffer) {
		// TODO Auto-generated method stub
		return false;
	}

	public void onBindServerCallback(ServerCallBack callBack) {
		// TODO Auto-generated method stub
	}

	public int onPrepareEnterRoom(int userid, String roomid, String roompassword) {
		// TODO Auto-generated method stub
		return 1;
	}

	public void onUserLogout(int userid, int errorcode) {
		// TODO Auto-generated method stub
		
	}

	public User onSSOVerifyUser(String json) throws UnSupportUserException {
		// TODO Auto-generated method stub
		return null;
	}

	public void onCronSchedule() {
		// TODO Auto-generated method stub
		
	}

	public String onAPIRouter(String fromip, String json) {
		// TODO Auto-generated method stub
		return null;
	}

	public boolean onUserTransBuffer(int pluginID, int fromuser, byte[] buffer) {
		// TODO Auto-generated method stub
		return false;
	}

}
