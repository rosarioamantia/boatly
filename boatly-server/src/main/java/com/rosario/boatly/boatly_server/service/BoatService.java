package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.repository.BoatRepository;
import com.rosario.boatly.boatly_server.model.Boat;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;

@Service
public class BoatService {

    @Autowired
    BoatRepository boatRepository;

    public List<Boat> getAllBoats(){
        return boatRepository.findAll();
    }

    public Boat updateBoat(Boat boat){
        return boatRepository.save(boat);
    }

    //if a boat didn't respond for x minutes
    public boolean isLastUpdateOutOfTime(LocalDateTime lastUpdate){
        LocalDateTime actualTime = LocalDateTime.now();

        return lastUpdate.isAfter(actualTime.minusMinutes(5L)) ? false : true;
    }

    public void setAsStolen(Boat boat){
        boat.setStolen(true);
        updateBoat(boat);
        System.out.println("boat with id " + boat.getId() + " set as stolen");
    }
}
