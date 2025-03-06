package com.rosario.boatly.boatly_server.repository;

import com.rosario.boatly.boatly_server.model.Boat;
import com.rosario.boatly.boatly_server.model.DetectedStolen;
import org.springframework.data.jpa.repository.JpaRepository;

public interface DetectedStolenRepository extends JpaRepository<DetectedStolen, Integer> {
}
