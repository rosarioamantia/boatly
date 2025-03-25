package com.rosario.boatly.boatly_server.controller;

import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.model.DetectedStolen;
import com.rosario.boatly.boatly_server.service.DetectedStolenService;
import com.rosario.boatly.boatly_server.service.JwtService;
import jakarta.servlet.http.HttpServletRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/detected-stolens")
public class DetectedStolenController {
    @Autowired
    DetectedStolenService detectedStolenService;

    @GetMapping("")
    public ResponseEntity getDetectedStolens(){
        List<DetectedStolen> boats = detectedStolenService.getAllDetectedStolens();

        return ResponseEntity.ok(boats);
    }
}
