package com.rosario.boatly.boatly_server.repository;

import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.model.Trip;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

public interface TripRepository extends JpaRepository<Trip, String> {
    Optional<Trip> findByBoatId(String boatId);
    List<Trip> findAll();
}
