package com.rosario.boatly.boatly_server.service;


import com.rosario.boatly.boatly_server.BoatlyServerApplication;
import com.rosario.boatly.boatly_server.configuration.OutboundMQTTConfig;
import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.model.Trip;
import com.rosario.boatly.boatly_server.repository.TripRepository;
import jakarta.transaction.Transactional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.WebApplicationType;
import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.cglib.core.Local;
import org.springframework.context.ConfigurableApplicationContext;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Service;
import com.rosario.boatly.boatly_server.repository.BoatRepository;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

@Service
public class TripService {

    final Long HOURS_VALID_TRIP = 24L;

    @Autowired
    public TripRepository tripRepository;

    @Autowired
    public BoatRepository boatRepository;

    @Autowired
    MQTTService mqttService;

    @Autowired
    private SimpMessagingTemplate messagingTemplate;

    public List<Trip> getAllTrips(){
        return tripRepository.findAll();
    }

    public Optional<Trip> getRegisteredTripByBoatId(String boatId){
        Optional<Trip> registeredTrip = tripRepository.findByBoatId(boatId).stream().filter(trip -> isTripCreatedToday(trip.getDate())).findFirst();

        return registeredTrip;
    }

    private boolean isTripCreatedToday(LocalDateTime tripDate){
        LocalDateTime actualTime = LocalDateTime.now();

        return tripDate.isAfter(actualTime.minusHours(HOURS_VALID_TRIP));
    }

    public boolean existRegisteredTrip(String boatId){
        Optional<Trip> registeredTrip = getRegisteredTripByBoatId(boatId);

        return registeredTrip.isPresent();
    }

    @Transactional
    public Boat processTrip(String boatId){
        Optional<Boat> boatToUpdateOptional = boatRepository.findById(boatId);
        if(boatToUpdateOptional.isPresent()){
            Boat boatToUpdate = boatToUpdateOptional.get();

            if(boatToUpdate.isInHarbor()){
                boatToUpdate.setInHarbor(false);

                Trip newTrip = new Trip();
                newTrip.setBoat(boatToUpdate);
                newTrip.setDate(LocalDateTime.now());

                Trip savedTrip = tripRepository.save(newTrip);
                messagingTemplate.convertAndSend("/boatly/trips", newTrip);
            }else{
                boatToUpdate.setInHarbor(true);
            }
            boatRepository.save(boatToUpdate);

            //send message by mqtt outbound
            mqttService.processMessageFor(boatId);


            return boatToUpdate;
        }
        return null;
    }
}
