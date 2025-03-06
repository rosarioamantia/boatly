package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.model.DetectedStolen;
import com.rosario.boatly.boatly_server.repository.BoatRepository;
import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.repository.DetectedStolenRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.Optional;

@Service
public class MQTTService {

    @Autowired
    BoatRepository boatRepository;

    @Autowired
    DetectedStolenRepository detectedStolenRepository;

    public void processDetectedBoat(String boatId){
        Optional<Boat> boatOptional = boatRepository.findById(boatId);

        boatOptional.ifPresent(
            boat -> {
                if(boat.isStolen()){ // means that message source is from stolen
                    registerDetectedStolenBoat(boat);
                }
                //to update only if not stolen boat and is in harbor mode
                else if(boat.isInHarbor()){
                    boat.setLastUpdate(LocalDateTime.now());
                    boatRepository.save(boat);
                }
            }
        );
    }

    public void registerDetectedStolenBoat(Boat detectedStolenBoat){
        DetectedStolen detectedStolen = new DetectedStolen(LocalDateTime.now(), detectedStolenBoat);
        detectedStolenRepository.save(detectedStolen);
    }

    public void parseMessage(String message){
        String[] data = message.split(":");

        // sender id
        if (data.length > 0 && !data[0].isEmpty()) {
            String detectedBoatId = data[0];

            processDetectedBoat(detectedBoatId);
        }

        // stolen id (can be blank)
        if (data.length > 1 && !data[1].isEmpty()) {
            String detectedStolenBoatId = data[1];
            Optional<Boat> detectedStolenBoatOptional = boatRepository.findById(detectedStolenBoatId);

            detectedStolenBoatOptional.ifPresent(
                    boat -> {
                        if(boat.isStolen()){
                            registerDetectedStolenBoat(boat);
                        }
                    }
            );
        }
    }
}
