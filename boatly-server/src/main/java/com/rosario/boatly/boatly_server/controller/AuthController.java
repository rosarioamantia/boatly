package com.rosario.boatly.boatly_server.controller;

import com.rosario.boatly.boatly_server.model.LoginRequest;
import com.rosario.boatly.boatly_server.model.LoginResponse;
import com.rosario.boatly.boatly_server.model.User;
import com.rosario.boatly.boatly_server.service.JwtService;
import com.rosario.boatly.boatly_server.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.web.bind.annotation.*;
import java.util.Collections;

@RestController
@RequestMapping("/api/auth")
public class AuthController {

    @Autowired
    private UserService userService;
    @Autowired
    private JwtService jwtService;

    @PostMapping("/register")
    public ResponseEntity<?> register(@RequestBody LoginRequest request) {
        System.out.println("entrato");
        try {
            User newUser = userService.registerUser(request.getUsername(), request.getPassword(), "USER");
            return ResponseEntity.ok("Utente registrato con successo");
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED).body(e.getMessage());
        }
    }

    @PostMapping("/register/admin")
    public ResponseEntity<?> registerAdmin(@RequestBody LoginRequest request) {
        try {
            userService.registerUser(request.getUsername(), request.getPassword(), "ADMIN");
            return ResponseEntity.ok("Admin registrato con successo");
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(e.getMessage());
        }
    }

    @PostMapping("/login")
    public ResponseEntity<?> login(@RequestBody LoginRequest request) {
        User user = userService.getUserByUsername(request.getUsername());
        if (new BCryptPasswordEncoder().matches(request.getPassword(), user.getPassword())) {
            String token = jwtService.generateToken(user.getUsername());
            System.out.println(token);
            return ResponseEntity.ok(new LoginResponse(token, user.getRole()));
            //return ResponseEntity.ok(Collections.singletonMap("token", token));
        } else {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED).body("Credenziali non valide");
        }
    }
}
