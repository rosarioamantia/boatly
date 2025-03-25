package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.configuration.OutboundMQTTConfig;
import com.rosario.boatly.boatly_server.model.DetectedStolen;
import com.rosario.boatly.boatly_server.repository.BoatRepository;
import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.repository.DetectedStolenRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

@Service
public class MQTTService {
    @Autowired
    BoatRepository boatRepository;
    @Autowired
    BoatService boatService;
    @Autowired
    private OutboundMQTTConfig.MyGateway gateway;
    @Autowired
    DetectedStolenRepository detectedStolenRepository;
    @Autowired
    private SimpMessagingTemplate messagingTemplate;

    public void processDetectedBoat(String boatId){
        Optional<Boat> boatOptional = boatRepository.findById(boatId);

        if(boatOptional.isPresent()){
            Boat boat = boatOptional.get();

            if(boat.isStolen()){ // means that message source is from stolen
                registerDetectedStolenBoat(boat);
            }

            boat.setLastUpdate(LocalDateTime.now());
            boatService.updateBoat(boat);
        }
    }

    public void registerDetectedStolenBoat(Boat detectedStolenBoat){
        DetectedStolen detectedStolen = new DetectedStolen(LocalDateTime.now(), detectedStolenBoat);
        detectedStolenRepository.save(detectedStolen);
        messagingTemplate.convertAndSend("/boatly/detected-stolens", detectedStolen);
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
            String originalStolenBoatId = data[1];
            String detectedStolenBoatId = originalStolenBoatId.substring(0, originalStolenBoatId.lastIndexOf('$'));
            Optional<Boat> detectedStolenBoatOptional = boatRepository.findById(detectedStolenBoatId);

            if (detectedStolenBoatOptional.isPresent()){
                Boat boat = detectedStolenBoatOptional.get();
                if(boat.isStolen()){
                    registerDetectedStolenBoat(boat);
                }
            }
        }
    }

    public void processMessageFor(String boatId){
        String messageToSend = boatId + ":";
        List<Boat> stolenBoats = boatService.getStolenBoats();

        if(!stolenBoats.isEmpty()){
            messageToSend += stolenBoats.size() + "_";

            for(Boat boat : stolenBoats){
                messageToSend += boat.getId() + "-";
            }

            messageToSend = messageToSend.substring(0, messageToSend.lastIndexOf('-')) + '$';

        }

        gateway.sendToMqtt(messageToSend);
    }
}
