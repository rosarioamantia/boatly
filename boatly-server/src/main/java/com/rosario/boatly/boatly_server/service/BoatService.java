package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.model.User;
import com.rosario.boatly.boatly_server.repository.BoatRepository;
import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import java.util.Optional;

import java.time.LocalDateTime;
import java.util.List;

@Service
public class BoatService {

    @Autowired
    BoatRepository boatRepository;

    @Autowired
    UserRepository userRepository;

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

    public Boat createBoat(Boat newBoat, String userId){
        Optional<User> ownerOptional = userRepository.findByUsername(userId);

        if(ownerOptional.isPresent()){
            newBoat.setInHarbor(true);
            newBoat.setStolen(false);
            newBoat.setLastUpdate(LocalDateTime.now());
            newBoat.setUser(ownerOptional.get());

            boatRepository.save(newBoat);

            return newBoat;
        }
        return null;
    }

    public List<Boat> getBoatsByUsername(String username){
        return boatRepository.findByUserUsername(username);
    }

    public List<Boat> getStolenBoats(){
        return boatRepository.findByStolenTrue();
    }
}
