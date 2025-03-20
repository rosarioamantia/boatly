package com.rosario.boatly.boatly_server.scheduling;

import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.model.Trip;
import com.rosario.boatly.boatly_server.service.BoatService;
import com.rosario.boatly.boatly_server.service.TripService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@Component
@EnableScheduling
public class CheckTask {

    @Autowired
    BoatService boatService;

    @Autowired
    TripService tripService;

    @Scheduled(fixedRate = 600000)  // 300000 ms = 5 minutes
    public void identifyStolenBoats() {
        System.out.println("Routine eseguita!");
        System.out.println(LocalDateTime.now());


        boatService.getAllBoats()
                .stream()
                .filter(boat -> boatService.isLastUpdateOutOfTime(boat.getLastUpdate()) &&
                        boat.isInHarbor() &&
                        !boat.isStolen())
                .forEach(
                        boat -> boatService.setAsStolen(boat));
    }
}
