package com.rosario.boatly.boatly_server.controller;

import com.rosario.boatly.boatly_server.model.Boat;
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

import java.util.List;

@RestController
@RequestMapping("/users")
public class UserController {

    @Autowired
    private UserService userService;

    @GetMapping("")
    public ResponseEntity getUsers(){
        List<User> users = userService.getAllUsers();

        return ResponseEntity.ok(users);
    }
}
