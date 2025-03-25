package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.model.User;
import com.rosario.boatly.boatly_server.repository.BoatRepository;
import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.handler.annotation.SendTo;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Service;
import java.util.Optional;

import java.time.LocalDateTime;
import java.util.List;

@Service
public class BoatService {

    @Autowired
    private SimpMessagingTemplate messagingTemplate;

    @Autowired
    BoatRepository boatRepository;

    @Autowired
    UserRepository userRepository;

    public List<Boat> getAllBoats(){
        return boatRepository.findAll();
    }

    public Boat updateBoat(Boat boat){
        messagingTemplate.convertAndSend("/boatly/boats", boat);
        return boatRepository.save(boat);
    }

    //if a boat didn't respond for x minutes
    public boolean isLastUpdateOutOfTime(LocalDateTime lastUpdate){
        LocalDateTime actualTime = LocalDateTime.now();

        return lastUpdate.isAfter(actualTime.minusMinutes(1L)) ? false : true;
    }

    public Boat setAsStolen(Boat boat){
        boat.setStolen(true);
        boat.setInHarbor(false);
        updateBoat(boat);
        System.out.println("boat with id " + boat.getId() + " set as stolen and not in harbor");

        return boat;
    }

    public Boat createBoat(Boat newBoat, String userId){
        Optional<User> ownerOptional = userRepository.findByUsername(userId);
        Optional<Boat> boatOptional = boatRepository.findById(newBoat.getId());

        if(ownerOptional.isPresent() && boatOptional.isEmpty()){
            newBoat.setInHarbor(true);
            newBoat.setStolen(false);
            newBoat.setLastUpdate(LocalDateTime.now());
            newBoat.setUser(ownerOptional.get());

            boatRepository.save(newBoat);

            return newBoat;
        }else{
            throw new RuntimeException("Error");
        }
    }

    public List<Boat> getBoatsByUsername(String username){
        return boatRepository.findByUserUsername(username);
    }

    public List<Boat> getStolenBoats(){
        return boatRepository.findByStolenTrue();
    }
}
