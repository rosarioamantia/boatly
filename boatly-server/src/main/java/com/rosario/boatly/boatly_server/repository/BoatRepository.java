package com.rosario.boatly.boatly_server.repository;

import com.rosario.boatly.boatly_server.model.Boat;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

@Repository
public interface BoatRepository extends JpaRepository<Boat, String> {
    List<Boat> findByUserUsername(String username);
    Optional<Boat> findById(String boatId);
    List<Boat> findByStolenTrue();
}
