package com.rosario.boatly.boatly_server.model;


import jakarta.persistence.*;

import java.time.LocalDateTime;

@Entity
@Table(name = "boats")
public class Boat {
    @Id
    @Column(name = "id")
    private String id;
    @Column(unique = true, nullable = false)
    private String name;
    @Column(nullable = false)
    private boolean stolen;
    @Column(nullable = false)
    private LocalDateTime lastUpdate;
    @Column(nullable = false)
    private boolean inHarbor;

    @ManyToOne
    private User user;

    public Boat() {
    }

    public Boat(String id, String name, boolean stolen, LocalDateTime lastUpdate, boolean inHarbor, User user) {
        this.id = id;
        this.name = name;
        this.stolen = stolen;
        this.lastUpdate = lastUpdate;
        this.inHarbor = inHarbor;
        this.user = user;
    }

    // Getter e Setter
    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public boolean isStolen() {
        return stolen;
    }

    public void setStolen(boolean stolen) {
        this.stolen = stolen;
    }

    public LocalDateTime getLastUpdate() {
        return lastUpdate;
    }

    public void setLastUpdate(LocalDateTime lastUpdate) {
        this.lastUpdate = lastUpdate;
    }

    public boolean isInHarbor() {
        return inHarbor;
    }

    public void setInHarbor(boolean inHarbor) {
        this.inHarbor = inHarbor;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }
}
