package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.model.DetectedStolen;
import com.rosario.boatly.boatly_server.model.User;
import com.rosario.boatly.boatly_server.repository.BoatRepository;
import com.rosario.boatly.boatly_server.repository.DetectedStolenRepository;
import com.rosario.boatly.boatly_server.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

@Service
public class DetectedStolenService {
    @Autowired
    DetectedStolenRepository detectedStolenRepository;

    public List<DetectedStolen> getAllDetectedStolens(){
        return detectedStolenRepository.findAll();
    }
}
