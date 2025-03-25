package com.rosario.boatly.boatly_server.repository;

import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.model.DetectedStolen;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface DetectedStolenRepository extends JpaRepository<DetectedStolen, Integer> {
    List<DetectedStolen> findAll();
}
