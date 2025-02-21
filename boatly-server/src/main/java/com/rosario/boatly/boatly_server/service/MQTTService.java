package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.repository.BoatRepository;
import com.rosario.boatly.boatly_server.model.Boat;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.Optional;

@Service
public class MQTTService {

    @Autowired
    BoatRepository boatRepository;

    public void updatePresence(String boatId){
        Optional<Boat> boatOptional = boatRepository.findById(boatId);

        boatOptional.ifPresent(
            boat -> {
                boat.setLastUpdate(LocalDateTime.now());
                boatRepository.save(boat);
            }
        );
    }
}
