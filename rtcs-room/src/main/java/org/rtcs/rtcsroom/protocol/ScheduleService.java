package org.rtcs.rtcsroom.protocol;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import org.rtcs.rtcsroom.resource.RoomResourceInterface;

@Component
public class ScheduleService {
    private static final Logger log = LoggerFactory.getLogger(ScheduleService.class);
    private RoomResourceInterface roomResourceInterface;
    
    public void setRoomResourceInterface(RoomResourceInterface api) {
        this.roomResourceInterface=api;
    }
    
    public void doScheduleTask() {
        cronDoSchedule();
    }
    
    @Scheduled(cron="${schedule.cron}")
    private void cronDoSchedule() {
        log.info("开始执行计划任务");
        if(null != roomResourceInterface) {
            log.info("扩展组件执行计划任务");
            roomResourceInterface.onCronSchedule();
        }
            
    }
}
