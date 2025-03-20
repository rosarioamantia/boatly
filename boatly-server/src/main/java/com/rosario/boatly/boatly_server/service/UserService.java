package com.rosario.boatly.boatly_server.service;

import com.rosario.boatly.boatly_server.model.User;
import com.rosario.boatly.boatly_server.model.UserDTO;
import com.rosario.boatly.boatly_server.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class UserService {
    @Autowired
    private UserRepository userRepository;

    @Autowired
    private PasswordEncoder passwordEncoder;

    public User registerUser(UserDTO user) {
        if (userRepository.findByUsername(user.getUsername()).isPresent()) {
            throw new RuntimeException("Username già esistente");
        }
        User newUser = new User();
        newUser.setName(user.getName());
        newUser.setSurname(user.getSurname());
        newUser.setUsername(user.getUsername());
        newUser.setEmail(user.getEmail());
        newUser.setPhoneNumber(user.getPhoneNumber());
        newUser.setPassword(passwordEncoder.encode(user.getPassword()));
        newUser.setRole("USER");
        return userRepository.save(newUser);
    }

    public User registerAdmin(UserDTO user) {
        if (userRepository.findByUsername(user.getUsername()).isPresent()) {
            throw new RuntimeException("Username già esistente");
        }
        User newUser = new User();
        newUser.setName(user.getName());
        newUser.setSurname(user.getSurname());
        newUser.setUsername(user.getUsername());
        newUser.setEmail(user.getEmail());
        newUser.setPhoneNumber(user.getPhoneNumber());
        newUser.setPassword(passwordEncoder.encode(user.getPassword()));
        newUser.setRole("ADMIN");
        return userRepository.save(newUser);
    }

    public List<User> getAllUsers(){
        return userRepository.findAll();
    }

    public User getUserByUsername(String username) {
        return userRepository.findByUsername(username).get();
    }
}
