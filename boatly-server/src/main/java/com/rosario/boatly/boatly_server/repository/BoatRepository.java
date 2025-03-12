package com.rosario.boatly.boatly_server.repository;

import com.rosario.boatly.boatly_server.model.Boat;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface BoatRepository extends JpaRepository<Boat, String> {
}
