package com.rosario.boatly.boatly_server.model;

import jakarta.persistence.*;

import java.time.LocalDateTime;

@Entity
@Table(name = "trip")
public class Trip {
    @Id
    @GeneratedValue(strategy= GenerationType.IDENTITY)
    @Column(name = "id")
    private Integer id;
    private LocalDateTime date;

    @OneToOne
    private Boat boat;

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public LocalDateTime getDate() {
        return date;
    }

    public void setDate(LocalDateTime date) {
        this.date = date;
    }

    public Boat getBoat() {
        return boat;
    }

    public void setBoat(Boat boat) {
        this.boat = boat;
    }
}
