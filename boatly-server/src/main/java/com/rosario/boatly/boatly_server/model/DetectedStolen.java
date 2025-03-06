package com.rosario.boatly.boatly_server.model;


import jakarta.persistence.*;

import java.time.LocalDateTime;

@Entity
@Table(name = "detected_stolen")
public class DetectedStolen {
    @Id
    @GeneratedValue(strategy= GenerationType.IDENTITY)
    @Column(name = "id")
    private Integer id;
    private LocalDateTime time;

    @ManyToOne
    private Boat boat;

    public DetectedStolen() {
    }

    public DetectedStolen(Integer id, LocalDateTime time, Boat boat) {
        this.id = id;
        this.time = time;
        this.boat = boat;
    }

    public DetectedStolen(LocalDateTime time, Boat boat) {
        this.time = time;
        this.boat = boat;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public LocalDateTime getTime() {
        return time;
    }

    public void setTime(LocalDateTime time) {
        this.time = time;
    }

    public Boat getBoat() {
        return boat;
    }

    public void setBoat(Boat boat) {
        this.boat = boat;
    }
}
