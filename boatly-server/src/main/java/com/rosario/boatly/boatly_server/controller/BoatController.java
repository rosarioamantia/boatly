package com.rosario.boatly.boatly_server.controller;

import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.service.BoatService;
import com.rosario.boatly.boatly_server.service.JwtService;
import jakarta.servlet.http.HttpServletRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/boats")
public class BoatController {

    @Autowired
    private JwtService jwtUtil;

    @Autowired
    BoatService boatService;

    @PostMapping("")
    public ResponseEntity createBoat(@RequestBody Boat boat, HttpServletRequest request){
        String username = getUsernameFromToken(request);
        boat = boatService.createBoat(boat, username);
        return ResponseEntity.ok(boat);
    }

    @GetMapping("")
    public ResponseEntity getBoats(){
        List<Boat> boats = boatService.getAllBoats();

        return ResponseEntity.ok(boats);
    }

    @GetMapping("/info")
    public ResponseEntity getMyBoats(HttpServletRequest request){
        String username = getUsernameFromToken(request);
        List<Boat> boats = boatService.getBoatsByUsername(username);

        return ResponseEntity.ok(boats);
    }

    @PatchMapping("")
    public ResponseEntity updateBoat(@RequestBody Boat boat){
        boat = boatService.updateBoat(boat);
        return ResponseEntity.ok(boat);
    }

    private String getUsernameFromToken(HttpServletRequest request) {
        String token = request.getHeader("Authorization").substring(7);
        return jwtUtil.extractUsername(token);
    }
}
