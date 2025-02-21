package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.model.Trip;
import com.rosario.boatly.boatly_server.repository.TripRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.cglib.core.Local;
import org.springframework.stereotype.Service;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

@Service
public class TripService {

    final Long HOURS_VALID_TRIP = 24L;

    @Autowired
    public TripRepository tripRepository;

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
}
