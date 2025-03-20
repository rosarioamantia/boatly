package com.rosario.boatly.boatly_server.controller;

import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.model.User;
import com.rosario.boatly.boatly_server.service.TripService;
import com.rosario.boatly.boatly_server.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/trips")
public class TripController {

    @Autowired
    private TripService tripService;

    @PostMapping("")
    public ResponseEntity registerTrip(@RequestParam String boatId){
        Boat updatedBoat = tripService.processTrip(boatId);
        return ResponseEntity.status(HttpStatus.CREATED).body(updatedBoat);
    }
}
