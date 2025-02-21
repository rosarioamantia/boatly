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

    public boolean isLastUpdateOutOfTime(LocalDateTime lastUpdate){
        LocalDateTime actualTime = LocalDateTime.now();

        return lastUpdate.isAfter(actualTime.minusMinutes(10L)) ? false : true;
    }
}
